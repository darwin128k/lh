/**
 * @file writer.h
 * @brief Write an ::lh_web_js_node_t tree as JavaScript text (::lh_web_js_writer_t).
 *
 * The tree holds no JavaScript text; the writer is the one place that makes
 * it, appended to an ::lh_str_t it does not own. What it guarantees:
 *
 * - **Names are checked.** Every identifier, property, function, parameter
 *   and declarator name must be an ASCII IdentifierName
 *   (::lh_web_js_lexer_name_is_ascii_identifier); anything else fails the
 *   write. Reserved words are not told apart from other names: keep them out
 *   of identifier positions.
 * - **Strings are escaped.** A string is written in single quotes with `\`,
 *   `'`, every control character and `<` escaped (`<` as `\x3c`), so no
 *   string can end its quotes, break its line or close the `script` element
 *   it stands in.
 * - **Object keys keep their meaning.** A key that is an identifier is written
 *   bare; any other key, and `__proto__`, as a computed string key
 *   (`['key']`), so it names an own property.
 * - **Regular expressions are checked** (::lh_web_js_writer_is_regex): no
 *   line break, no unescaped `/`, no `<`, known flags only.
 * - **Parentheses follow precedence.** The tree has no parentheses; the writer
 *   puts them around an operand that binds more loosely than its place needs
 *   (lh/web/js/precedence.h), and where two tokens would otherwise run
 *   together (`a- -b`, `a/ /re/`, `a<!--b`, a statement that starts with
 *   `function` or `{`, `new (f())()`).
 *
 * Layout: no indentation; every statement ends its line (`;` and a line
 * break), a block opens with `{` and a line break and closes with `}`;
 * `}else{` and `}catch(e){` stay on one line. An `else` whose `if` has an
 * `if` or a `for` as its consequent gets that consequent wrapped in braces
 * (::lh_web_js_writer_needs_braces), so the `else` cannot be taken by an inner
 * `if`. An `in` inside the initializer of a
 * `for` is not parenthesized: do not build one.
 *
 * ::lh_web_js_writer_try_write reports a tree it cannot write by returning
 * false; the text appended up to that point stays in the output.
 * ::lh_web_js_writer_write is the same with a runtime check.
 *
 * Example usage:
 * @code{.c}
 * lh_web_js_writer_t writer;
 * lh_str_t text;
 *
 * lh_str_init(&text);
 * lh_web_js_writer_init(&writer, &text);
 * if (lh_web_js_writer_try_write(&writer, program)) {
 *     // text: the script, one statement a line
 * }
 * lh_str_deinit(&text);
 * @endcode
 */

#ifndef LH_WEB_JS_WRITER_H
#define LH_WEB_JS_WRITER_H

#include <lh/web/js/node.h>
#include <lh/web/js/writer/fields.h>

/**
 * @struct lh_web_js_writer
 * @brief The output. Fields via ::lh_web_js_writer_fields.
 */
struct lh_web_js_writer
{
    /** Members: ::lh_web_js_writer_fields. */
    lh_web_js_writer_fields(lh_str_t *);
};
/** @copybrief lh_web_js_writer */
typedef struct lh_web_js_writer lh_web_js_writer_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The string @p self appends to, to change. */
lh_str_t *
lh_web_js_writer_get_out_mut(lh_web_js_writer_t *self);

/** Make @p out the string @p self appends to. */
lh_void
lh_web_js_writer_set_out(lh_web_js_writer_t *self, lh_str_t *out);

/* ── checks ──────────────────────────────────────────────────────────────── */

/** True when @p ch must be escaped inside a string: `\`, `'`, `<` and every control character. */
lh_bool_t
lh_web_js_writer_needs_escape(lh_char_t ch);

/** True when @p name can be written as an identifier: an ASCII IdentifierName. */
lh_bool_t
lh_web_js_writer_is_name(lh_str_view_t name);

/** True when @p text is the digits of a number: not empty, ASCII digits only. */
lh_bool_t
lh_web_js_writer_is_number(lh_str_view_t text);

/**
 * @brief True when `/pattern/flags` can be written as it is: @p pattern not
 *        empty, not starting with `*`, without line breaks, `<` or an
 *        unescaped `/`, not ending in a lone `\`; @p flags only from `dgimsuyv`.
 */
lh_bool_t
lh_web_js_writer_is_regex(lh_str_view_t pattern, lh_str_view_t flags);

/**
 * @brief How tightly @p node binds as an expression (lh/web/js/precedence.h):
 *        its operator's level for a unary, update, binary or assignment node,
 *        ::lh_web_js_precedence_primary for a name or a literal.
 */
lh_web_js_precedence_t
lh_web_js_writer_get_precedence(const lh_web_js_node_t *node);

/**
 * @brief True when the text of @p node starts with `+` or `-`: a unary plus or
 *        minus, a prefix `++` or `--`.
 */
lh_bool_t
lh_web_js_writer_has_leading_sign(const lh_web_js_node_t *node);

/** True when the callee @p node of a `new` holds a call in its member chain: `new (f().g)()`. */
lh_bool_t
lh_web_js_writer_needs_new_parens(const lh_web_js_node_t *node);

/**
 * @brief True when the statement @p node, the consequent of an `if` that has
 *        an `else`, must be written in braces: an `if` or a `for` (whose body
 *        may end in an `if`) would otherwise take the `else` for itself.
 */
lh_bool_t
lh_web_js_writer_needs_braces(const lh_web_js_node_t *node);

/* ── init and parenthesis checks ─────────────────────────────────────────── */

/**
 * @brief Start writing at the end of @p out. @p out is appended to, never
 *        cleared, and stays the caller's.
 */
lh_void
lh_web_js_writer_init(lh_web_js_writer_t *self, lh_str_t *out);

/**
 * @brief True when @p right, the right operand of the binary @p op, needs
 *        parentheses for its tokens not to run into the operator's: a sign
 *        after `+` or `-`, a regular expression after `/` (a comment) or after
 *        `<` (`</`), a `!` after `<` (`<!--`).
 */
lh_bool_t
lh_web_js_writer_needs_parens_after(lh_web_js_op_t op, const lh_web_js_node_t *right);

/**
 * @brief True when the expression statement @p node would start with
 *        `function` or `{` and so be read as a declaration or a block: it is
 *        then written in parentheses.
 */
lh_bool_t
lh_web_js_writer_needs_statement_parens(const lh_web_js_node_t *node);

/* ── text ────────────────────────────────────────────────────────────────── */

/** Append @p ch as it is. */
lh_void
lh_web_js_writer_put_char(lh_web_js_writer_t *self, lh_char_t ch);

/** Append @p text as it is. */
lh_void
lh_web_js_writer_put_view(lh_web_js_writer_t *self, lh_str_view_t text);

/** Append the escape that stands for @p ch inside a string: `\\`, `\'`, `\n`, `\x3c`, ... */
lh_void
lh_web_js_writer_put_escape(lh_web_js_writer_t *self, lh_char_t ch);

/**
 * @brief Append @p name as an identifier.
 * @return False, appending nothing, when ::lh_web_js_writer_is_name refuses it.
 */
lh_bool_t
lh_web_js_writer_try_put_name(lh_web_js_writer_t *self, lh_str_view_t name);

/** Append @p text as a single-quoted string literal, escapes inside. */
lh_void
lh_web_js_writer_put_string(lh_web_js_writer_t *self, lh_str_view_t text);

/** Append @p key as an object key: bare when it is a name other than `__proto__`, else `['key']`.
 */
lh_void
lh_web_js_writer_put_key(lh_web_js_writer_t *self, lh_str_view_t key);

/* ── tree ────────────────────────────────────────────────────────────────── */

/**
 * @brief Append the statement @p node, with its line break.
 * @return False for a node that is no statement, or one that cannot be written.
 */
lh_bool_t
lh_web_js_writer_try_write_statement(lh_web_js_writer_t *self, const lh_web_js_node_t *node);

/**
 * @brief Append the statements from @p first to the last child of @p node.
 * @return False at the first one that cannot be written.
 */
lh_bool_t
lh_web_js_writer_try_write_statements(lh_web_js_writer_t *self, const lh_web_js_node_t *node,
                                      const lh_web_js_node_t *first);

/**
 * @brief Append the expressions from @p first to the last child of @p node,
 *        separated by `,`, each at ::lh_web_js_precedence_assign.
 * @return False at the first one that cannot be written.
 */
lh_bool_t
lh_web_js_writer_try_write_items(lh_web_js_writer_t *self, const lh_web_js_node_t *node,
                                 const lh_web_js_node_t *first);

/**
 * @brief Append `{`, a line break, every statement of the block @p node and
 *        `}` - no line break after it.
 * @return False when @p node is no block or a statement cannot be written.
 */
lh_bool_t
lh_web_js_writer_try_write_body(lh_web_js_writer_t *self, const lh_web_js_node_t *node);

/**
 * @brief Append the function @p node: `function`, its name if it has one, its
 *        parameters in parentheses and its body.
 * @return False for a parameter or name that is not valid, or a body that
 *         cannot be written.
 */
lh_bool_t
lh_web_js_writer_try_write_function(lh_web_js_writer_t *self, const lh_web_js_node_t *node);

/**
 * @brief Append the expression @p node without parentheses around it.
 * @return False for a node that is no expression, or one whose shape or
 *         names cannot be written.
 */
lh_bool_t
lh_web_js_writer_try_write_bare(lh_web_js_writer_t *self, const lh_web_js_node_t *node);

/**
 * @brief Append the expression @p node where at least @p min is needed: in
 *        parentheses when it binds more loosely.
 * @return False for a null node, and as ::lh_web_js_writer_try_write_bare.
 */
lh_bool_t
lh_web_js_writer_try_write_expression(lh_web_js_writer_t *self, const lh_web_js_node_t *node,
                                      lh_web_js_precedence_t min);

/**
 * @brief Append the `var` @p node without its `;`: `var a=1,b`.
 * @return False for a child that is no declarator, or a name or value that
 *         cannot be written.
 */
lh_bool_t
lh_web_js_writer_try_write_declarations(lh_web_js_writer_t *self, const lh_web_js_node_t *node);

/**
 * @brief Append one clause of a `for`: nothing for ::lh_web_js_kind_empty, the
 *        declarations of a `var`, an expression otherwise.
 */
lh_bool_t
lh_web_js_writer_try_write_clause(lh_web_js_writer_t *self, const lh_web_js_node_t *node);

/**
 * @brief Append @p node: every statement of a program, or the one statement
 *        @p node is.
 * @return False when the tree cannot be written; what was appended before stays.
 */
lh_bool_t
lh_web_js_writer_try_write(lh_web_js_writer_t *self, const lh_web_js_node_t *node);

/**
 * @brief ::lh_web_js_writer_try_write, checked: a tree that cannot be written
 *        is a runtime check failure (::lh_runtime_error_code_invalid_argument).
 */
lh_void
lh_web_js_writer_write(lh_web_js_writer_t *self, const lh_web_js_node_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_JS_WRITER_H */
