/**
 * @file node.h
 * @brief One JavaScript expression or statement and its children
 *        (::lh_web_js_node_t): the tree a script is built as before it is
 *        written out.
 *
 * Every part of a script is a node - an identifier, a literal, a call, an
 * `if`, a function - whose kind (lh/web/js/kind.h) says which fields it uses
 * and what its children are. The shapes follow ESTree: a binary node holds
 * its operator and two operands, a call its callee and arguments, a function
 * its parameters and a body block. The tree holds no JavaScript text: names,
 * values and operators are data, and the writer (::lh_web_js_writer_try_write)
 * is the one place that turns them into text - validating every name,
 * escaping every string, and putting parentheses exactly where precedence
 * needs them.
 *
 * Like ::lh_web_json_node_t, a node owns what it holds - its strings and its
 * children - and frees it in ::lh_web_js_node_deinit, recursively. A node is
 * in at most one parent: an expression used twice is built twice.
 *
 * A node links its children through a list head stored inside it, so it must
 * not be copied or moved once initialized: keep the root where it was
 * initialized, and let the tree allocate the rest (::lh_web_js_node_create
 * and the `create_*` composites, which build each shape right).
 *
 * Example usage:
 * @code{.c}
 * // function hexv(c){return c<58?c-48:(c|32)-87;}
 * lh_web_js_node_t *f = lh_web_js_node_create_function_declaration(lh_str_view_lit("hexv"));
 * lh_web_js_node_push_param(f, lh_str_view_lit("c"));
 * lh_web_js_node_push_statement(
 *     f, lh_web_js_node_create_return(lh_web_js_node_create_conditional(
 *            lh_web_js_node_create_binary(lh_web_js_op_lt, lh_web_js_node_create_ident_lit("c"),
 *                                         lh_web_js_node_create_number(58U)),
 *            lh_web_js_node_create_binary(lh_web_js_op_sub, lh_web_js_node_create_ident_lit("c"),
 *                                         lh_web_js_node_create_number(48U)),
 *            lh_web_js_node_create_binary(
 *                lh_web_js_op_sub,
 *                lh_web_js_node_create_binary(lh_web_js_op_bit_or,
 *                                             lh_web_js_node_create_ident_lit("c"),
 *                                             lh_web_js_node_create_number(32U)),
 *                lh_web_js_node_create_number(87U)))));
 * // ... write it with lh_web_js_writer_try_write, then:
 * lh_web_js_node_destroy(f);
 * @endcode
 */

#ifndef LH_WEB_JS_NODE_H
#define LH_WEB_JS_NODE_H

#include <lh/web/js/kind.h>
#include <lh/web/js/node/fields.h>
#include <lh/web/js/op.h>
#include <lh/list.h>
#include <lh/str.h>

/**
 * @def LH_WEB_JS_NODE_INTEGER_DIGITS_MAX
 * @brief Characters the longest 64-bit unsigned integer takes as text: 20
 *        digits and the terminator (::lh_web_js_node_set_number).
 */
#define LH_WEB_JS_NODE_INTEGER_DIGITS_MAX 21U

/**
 * @struct lh_web_js_node
 * @brief Kind, operator, flag, name, text and children. Fields via
 *        ::lh_web_js_node_fields.
 */
struct lh_web_js_node
{
    /** Members: ::lh_web_js_node_fields. */
    lh_web_js_node_fields(lh_list_node_t, lh_web_js_kind_t, lh_web_js_op_t, lh_bool_t, lh_str_t,
                          lh_list_t);
};
/** @copybrief lh_web_js_node */
typedef struct lh_web_js_node lh_web_js_node_t;

/**
 * @brief ::lh_web_js_node_create_ident for the string literal @p name.
 *
 * @code{.c}
 * lh_web_js_node_t *x = lh_web_js_node_create_ident_lit("window");
 * @endcode
 */
#define lh_web_js_node_create_ident_lit(name) lh_web_js_node_create_ident(lh_str_view_lit(name))

/** @brief ::lh_web_js_node_create_string for the string literal @p value. */
#define lh_web_js_node_create_string_lit(value) lh_web_js_node_create_string(lh_str_view_lit(value))

/**
 * @brief ::lh_web_js_node_create_member for the string literal @p name: `object.name`, the
 *        new node owning @p object.
 */
#define lh_web_js_node_create_member_lit(object, name)                                             \
    lh_web_js_node_create_member((object), lh_str_view_lit(name))

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The link that puts @p self in its parent's children, to change. */
lh_list_node_t *
lh_web_js_node_get_link_mut(lh_web_js_node_t *self);

/** The link that puts @p self in its parent's children. */
const lh_list_node_t *
lh_web_js_node_get_link(const lh_web_js_node_t *self);

/** The node whose link is @p link, to change; ::lh_null for a null @p link. */
lh_web_js_node_t *
lh_web_js_node_get_by_link_mut(lh_list_node_t *link);

/** The node whose link is @p link; ::lh_null for a null @p link. */
const lh_web_js_node_t *
lh_web_js_node_get_by_link(const lh_list_node_t *link);

/** What @p self is. */
lh_web_js_kind_t
lh_web_js_node_get_kind(const lh_web_js_node_t *self);

/** Make @p self a @p kind, touching nothing else. The `reset` / `set_*` composites do it right. */
lh_void
lh_web_js_node_set_kind(lh_web_js_node_t *self, lh_web_js_kind_t kind);

/** The operator of @p self; ::lh_web_js_op_none for a kind without one. */
lh_web_js_op_t
lh_web_js_node_get_op(const lh_web_js_node_t *self);

/** Make @p op the operator of @p self, touching nothing else. */
lh_void
lh_web_js_node_set_op(lh_web_js_node_t *self, lh_web_js_op_t op);

/** The flag of @p self: the value of a boolean, the prefix form of an update. */
lh_bool_t
lh_web_js_node_get_flag(const lh_web_js_node_t *self);

/** Make @p flag the flag of @p self, touching nothing else. */
lh_void
lh_web_js_node_set_flag(lh_web_js_node_t *self, lh_bool_t flag);

/** The name of @p self, to change. */
lh_str_t *
lh_web_js_node_get_name_mut(lh_web_js_node_t *self);

/** The name of @p self: an identifier, a property, a function, or regex flags. */
const lh_str_t *
lh_web_js_node_get_name(const lh_web_js_node_t *self);

/** The text of @p self, to change. */
lh_str_t *
lh_web_js_node_get_text_mut(lh_web_js_node_t *self);

/** The text of @p self: a string's value (unescaped), a number's digits, a regex pattern. */
const lh_str_t *
lh_web_js_node_get_text(const lh_web_js_node_t *self);

/** The children of @p self, to change. */
lh_list_t *
lh_web_js_node_get_children_mut(lh_web_js_node_t *self);

/** The children of @p self, in order. */
const lh_list_t *
lh_web_js_node_get_children(const lh_web_js_node_t *self);

/** True when @p self is a @p kind. */
lh_bool_t
lh_web_js_node_has_kind(const lh_web_js_node_t *self, lh_web_js_kind_t kind);

/** The name of @p self as a view; valid until the name changes. */
lh_str_view_t
lh_web_js_node_get_name_view(const lh_web_js_node_t *self);

/** The text of @p self as a view; valid until the text changes. */
lh_str_view_t
lh_web_js_node_get_text_view(const lh_web_js_node_t *self);

/* ── init / deinit ───────────────────────────────────────────────────────── */

/**
 * @brief Set @p self to hold nothing (::lh_web_js_kind_none): no operator, no
 *        flag, no name, no text, no children. Nothing is allocated.
 */
lh_void
lh_web_js_node_init(lh_web_js_node_t *self);

/* ── children and lifetime ───────────────────────────────────────────────── */

/**
 * @brief Make @p child the last child of @p self. The tree owns it from then
 *        on: @p child must come from ::lh_web_js_node_create and belong to no
 *        other parent.
 *
 * @return @p self, so pushes chain while a tree is built in one expression.
 */
lh_web_js_node_t *
lh_web_js_node_push_back(lh_web_js_node_t *self, lh_web_js_node_t *child);

/** The first child of @p self; ::lh_null without children. */
const lh_web_js_node_t *
lh_web_js_node_get_first(const lh_web_js_node_t *self);

/** The child after @p child among the children of @p self; ::lh_null after the last. */
const lh_web_js_node_t *
lh_web_js_node_get_next(const lh_web_js_node_t *self, const lh_web_js_node_t *child);

/** The last child of @p self, to change; ::lh_null without children. */
lh_web_js_node_t *
lh_web_js_node_get_last_mut(lh_web_js_node_t *self);

/** The last child of @p self; ::lh_null without children. */
const lh_web_js_node_t *
lh_web_js_node_get_last(const lh_web_js_node_t *self);

/** How many children @p self has. */
lh_usize_t
lh_web_js_node_get_size(const lh_web_js_node_t *self);

/**
 * @brief Free what @p self holds: its name, its text and every child,
 *        recursively. @p self itself stays where it is; init it to reuse it.
 */
lh_void
lh_web_js_node_deinit(lh_web_js_node_t *self);

/**
 * @brief A new node from the runtime allocator, initialized. Out of memory:
 *        runtime check failure, as for any lh allocation.
 */
lh_web_js_node_t *lh_web_js_node_create(lh_void);

/**
 * @brief Take @p self out of its parent's children, if it is in any, deinit it
 *        and give it back to the runtime allocator. Only for a node from
 *        ::lh_web_js_node_create: this is how a node and its subtree are deleted.
 */
lh_void
lh_web_js_node_destroy(lh_web_js_node_t *self);

/**
 * @brief Drop what @p self holds - its name, its text, its operator, its flag
 *        and every child, recursively - and leave it holding nothing
 *        (::lh_web_js_kind_none). Its place in the parent stays.
 */
lh_void
lh_web_js_node_clear(lh_web_js_node_t *self);

/** True when @p self is a function, a declaration or an expression: its last child is its body. */
lh_bool_t
lh_web_js_node_is_function(const lh_web_js_node_t *self);

/* ── shapes ──────────────────────────────────────────────────────────────── */

/** Clear @p self and make it a @p kind with nothing else set yet. */
lh_void
lh_web_js_node_reset(lh_web_js_node_t *self, lh_web_js_kind_t kind);

/**
 * @brief Append @p statement to the body of @p self: the body block of a
 *        function, the children of anything else (a program, a block, a case).
 *        The tree owns @p statement from then on.
 *
 * @return @p self, so pushes chain.
 */
lh_web_js_node_t *
lh_web_js_node_push_statement(lh_web_js_node_t *self, lh_web_js_node_t *statement);

/**
 * @brief Clear @p self and make it a @p kind named @p name (copied): an
 *        identifier, a property, a member, a declarator, a function, a catch.
 *        The writer checks the name.
 */
lh_void
lh_web_js_node_reset_named(lh_web_js_node_t *self, lh_web_js_kind_t kind, lh_str_view_t name);

/**
 * @brief Clear @p self and make it a @p kind with the operator @p op: a unary,
 *        update, binary or assignment node, its operands still to push.
 */
lh_void
lh_web_js_node_reset_operator(lh_web_js_node_t *self, lh_web_js_kind_t kind, lh_web_js_op_t op);

/** Clear @p self and make it the number @p value, kept as its decimal digits. */
lh_void
lh_web_js_node_set_number(lh_web_js_node_t *self, lh_ullong_t value);

/** Clear @p self and make it the string @p value (copied, unescaped; the writer escapes it). */
lh_void
lh_web_js_node_set_string(lh_web_js_node_t *self, lh_str_view_t value);

/** Clear @p self and make it `true` or `false`. */
lh_void
lh_web_js_node_set_bool(lh_web_js_node_t *self, lh_bool_t value);

/* ── composites ──────────────────────────────────────────────────────────── */

/** A new node of @p kind with nothing else set: `null`, `[]`, `{}`, a program, a block, ... */
lh_web_js_node_t *
lh_web_js_node_create_with_kind(lh_web_js_kind_t kind);

/** Clear @p self and make it the identifier @p name (copied; the writer checks it). */
lh_void
lh_web_js_node_set_ident(lh_web_js_node_t *self, lh_str_view_t name);

/**
 * @brief Clear @p self and make it the regular expression `/pattern/flags`
 *        (both copied). The writer checks both (::lh_web_js_writer_is_regex).
 */
lh_void
lh_web_js_node_set_regex(lh_web_js_node_t *self, lh_str_view_t pattern, lh_str_view_t flags);

/** A new node of @p kind named @p name (::lh_web_js_node_reset_named). */
lh_web_js_node_t *
lh_web_js_node_create_named(lh_web_js_kind_t kind, lh_str_view_t name);

/** A new node of @p kind with the operator @p op (::lh_web_js_node_reset_operator). */
lh_web_js_node_t *
lh_web_js_node_create_with_operator(lh_web_js_kind_t kind, lh_web_js_op_t op);

/** A new number @p value. */
lh_web_js_node_t *
lh_web_js_node_create_number(lh_ullong_t value);

/** A new string @p value. */
lh_web_js_node_t *
lh_web_js_node_create_string(lh_str_view_t value);

/** A new `true` or `false`. */
lh_web_js_node_t *
lh_web_js_node_create_bool(lh_bool_t value);

/** A new `object[index]`; it owns both. */
lh_web_js_node_t *
lh_web_js_node_create_index(lh_web_js_node_t *object, lh_web_js_node_t *index);

/** A new `callee()`; it owns @p callee. Push the arguments with ::lh_web_js_node_push_back. */
lh_web_js_node_t *
lh_web_js_node_create_call(lh_web_js_node_t *callee);

/** A new `new callee()`; it owns @p callee. Push the arguments with ::lh_web_js_node_push_back. */
lh_web_js_node_t *
lh_web_js_node_create_new(lh_web_js_node_t *callee);

/** A new `test?consequent:alternate`; it owns all three. */
lh_web_js_node_t *
lh_web_js_node_create_conditional(lh_web_js_node_t *test, lh_web_js_node_t *consequent,
                                  lh_web_js_node_t *alternate);

/** A new expression statement `expression;`; it owns @p expression. */
lh_web_js_node_t *
lh_web_js_node_create_expression(lh_web_js_node_t *expression);

/** A new `if(test)consequent`, with `else alternate` when @p alternate is not ::lh_null; it owns
 * them. */
lh_web_js_node_t *
lh_web_js_node_create_if(lh_web_js_node_t *test, lh_web_js_node_t *consequent,
                         lh_web_js_node_t *alternate);

/**
 * @brief A new `for(init;test;update)body`; it owns all four. A null clause
 *        becomes ::lh_web_js_kind_empty; @p body must not be null.
 */
lh_web_js_node_t *
lh_web_js_node_create_for(lh_web_js_node_t *init, lh_web_js_node_t *test, lh_web_js_node_t *update,
                          lh_web_js_node_t *body);

/** A new `return argument;`, or `return;` for a null @p argument; it owns @p argument. */
lh_web_js_node_t *
lh_web_js_node_create_return(lh_web_js_node_t *argument);

/** A new `switch(discriminant){}`; push its cases with ::lh_web_js_node_push_back. */
lh_web_js_node_t *
lh_web_js_node_create_switch(lh_web_js_node_t *discriminant);

/** A new `case test:`; push its statements with ::lh_web_js_node_push_statement. */
lh_web_js_node_t *
lh_web_js_node_create_case(lh_web_js_node_t *test);

/** A new identifier @p name. */
lh_web_js_node_t *
lh_web_js_node_create_ident(lh_str_view_t name);

/** A new regular expression `/pattern/flags`. */
lh_web_js_node_t *
lh_web_js_node_create_regex(lh_str_view_t pattern, lh_str_view_t flags);

/** A new object member `key:value`; the property owns @p value. */
lh_web_js_node_t *
lh_web_js_node_create_property(lh_str_view_t key, lh_web_js_node_t *value);

/** A new `function(){}` with no name, no parameters and an empty body. */
lh_web_js_node_t *lh_web_js_node_create_function_expression(lh_void);

/** A new `function name(){}` statement with no parameters and an empty body. */
lh_web_js_node_t *
lh_web_js_node_create_function_declaration(lh_str_view_t name);

/** A new `object.name`; it owns @p object. */
lh_web_js_node_t *
lh_web_js_node_create_member(lh_web_js_node_t *object, lh_str_view_t name);

/** A new unary `op operand` (::lh_web_js_op_not, ::lh_web_js_op_typeof, ...); it owns @p operand.
 */
lh_web_js_node_t *
lh_web_js_node_create_unary(lh_web_js_op_t op, lh_web_js_node_t *operand);

/** A new `++operand` / `--operand`; it owns @p operand. */
lh_web_js_node_t *
lh_web_js_node_create_prefix(lh_web_js_op_t op, lh_web_js_node_t *operand);

/** A new `operand++` / `operand--`; it owns @p operand. */
lh_web_js_node_t *
lh_web_js_node_create_postfix(lh_web_js_op_t op, lh_web_js_node_t *operand);

/** A new `left op right`; it owns both. */
lh_web_js_node_t *
lh_web_js_node_create_binary(lh_web_js_op_t op, lh_web_js_node_t *left, lh_web_js_node_t *right);

/** A new `target op value` (::lh_web_js_op_assign, ::lh_web_js_op_add_assign, ...); it owns both.
 */
lh_web_js_node_t *
lh_web_js_node_create_assign(lh_web_js_op_t op, lh_web_js_node_t *target, lh_web_js_node_t *value);

/** A new `name=init` of a `var`; it owns @p init, which may be ::lh_null for a bare name. */
lh_web_js_node_t *
lh_web_js_node_create_declarator(lh_str_view_t name, lh_web_js_node_t *init);

/** A new `try block catch(name) handler`; it owns @p block (a block) and @p handler (a block). */
lh_web_js_node_t *
lh_web_js_node_create_try(lh_web_js_node_t *block, lh_str_view_t name, lh_web_js_node_t *handler);

/**
 * @brief Add the parameter @p name to the function @p self, after the ones it
 *        has and before its body.
 *
 * @return @p self, so pushes chain.
 */
lh_web_js_node_t *
lh_web_js_node_push_param(lh_web_js_node_t *self, lh_str_view_t name);

/** Append `name=init` (or the bare `name` for a null @p init) to the `var` @p self; it owns
   @p init. Returns @p self, so declarators chain. */
lh_web_js_node_t *
lh_web_js_node_add_declarator(lh_web_js_node_t *self, lh_str_view_t name, lh_web_js_node_t *init);

/** A new `var` statement with the one declarator `name=init` (bare for a null @p init). */
lh_web_js_node_t *
lh_web_js_node_create_var(lh_str_view_t name, lh_web_js_node_t *init);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_JS_NODE_H */
