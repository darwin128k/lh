/**
 * @file kind.h
 * @brief What an ::lh_web_js_node_t is: one JavaScript expression or statement.
 *
 * The kinds are the parts of a script the writer can print, after the ESTree
 * shapes: identifiers and literals, the compound expressions, and the
 * statements. Each says which fields of the node it uses (`name`, `text`,
 * `op`, the flag) and what its children are, in order; the `create_*`
 * composites of lh/core/web/js/node.h build each shape right, and
 * ::lh_web_js_writer_try_write refuses a node whose shape does not match.
 */

#ifndef LH_WEB_JS_KIND_H
#define LH_WEB_JS_KIND_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_web_js_kind_t
 * @brief Discriminator of ::lh_web_js_node_t. Alias for ::lh_u8_t; values are
 *        the `lh_web_js_kind_*` constants below.
 */
typedef lh_u8_t lh_web_js_kind_t;

/**
 * @def lh_web_js_kind_none
 * @brief Nothing yet. What ::lh_web_js_node_init leaves; the writer refuses it.
 */
#define lh_web_js_kind_none 0U

/**
 * @def lh_web_js_kind_ident
 * @brief An identifier, its name in `name`: `fw`.
 */
#define lh_web_js_kind_ident 1U

/**
 * @def lh_web_js_kind_number
 * @brief A non-negative whole number, its digits in `text`: `255`. A negative one is
 * ::lh_web_js_op_minus over it.
 */
#define lh_web_js_kind_number 2U

/**
 * @def lh_web_js_kind_string
 * @brief A string literal, its value (unescaped) in `text`: written `'...'`, escaped.
 */
#define lh_web_js_kind_string 3U

/**
 * @def lh_web_js_kind_regex
 * @brief A regular expression literal: the pattern in `text`, the flags in `name`: `/scale=fit/`.
 */
#define lh_web_js_kind_regex 4U

/**
 * @def lh_web_js_kind_null
 * @brief `null`.
 */
#define lh_web_js_kind_null 5U

/**
 * @def lh_web_js_kind_bool
 * @brief `true` or `false`, in the node's flag.
 */
#define lh_web_js_kind_bool 6U

/**
 * @def lh_web_js_kind_array
 * @brief `[a,b]`: the elements are the children.
 */
#define lh_web_js_kind_array 7U

/**
 * @def lh_web_js_kind_object
 * @brief `{k:v}`: the children are ::lh_web_js_kind_property nodes.
 */
#define lh_web_js_kind_object 8U

/**
 * @def lh_web_js_kind_property
 * @brief One member of an object: the key in `name`, the value its one child.
 */
#define lh_web_js_kind_property 9U

/**
 * @def lh_web_js_kind_function_expression
 * @brief `function name(params){body}` as a value: the name in `name` (may be empty), the
 * parameters (identifiers) as the first children, the body (a block) as the last.
 */
#define lh_web_js_kind_function_expression 10U

/**
 * @def lh_web_js_kind_member
 * @brief `object.name`: the object its one child, the property name in `name`.
 */
#define lh_web_js_kind_member 11U

/**
 * @def lh_web_js_kind_index
 * @brief `object[index]`: two children.
 */
#define lh_web_js_kind_index 12U

/**
 * @def lh_web_js_kind_call
 * @brief `callee(args)`: the callee the first child, the arguments the rest.
 */
#define lh_web_js_kind_call 13U

/**
 * @def lh_web_js_kind_new
 * @brief `new callee(args)`: the callee the first child, the arguments the rest.
 */
#define lh_web_js_kind_new 14U

/**
 * @def lh_web_js_kind_unary
 * @brief `op operand` (`!a`, `typeof a`): the operator in `op`, one child.
 */
#define lh_web_js_kind_unary 15U

/**
 * @def lh_web_js_kind_update
 * @brief `++a` / `a++`: the operator in `op`, the flag set for the prefix form, one child.
 */
#define lh_web_js_kind_update 16U

/**
 * @def lh_web_js_kind_binary
 * @brief `left op right`, logical operators included: the operator in `op`, two children.
 */
#define lh_web_js_kind_binary 17U

/**
 * @def lh_web_js_kind_assign
 * @brief `target op value` (`=`, `+=`, ...): the operator in `op`, two children.
 */
#define lh_web_js_kind_assign 18U

/**
 * @def lh_web_js_kind_conditional
 * @brief `test?consequent:alternate`: three children.
 */
#define lh_web_js_kind_conditional 19U

/**
 * @def lh_web_js_kind_sequence
 * @brief `a,b,c`: the expressions are the children.
 */
#define lh_web_js_kind_sequence 20U

/**
 * @def lh_web_js_kind_program
 * @brief A whole script: the statements are the children, written one after another.
 */
#define lh_web_js_kind_program 21U

/**
 * @def lh_web_js_kind_var
 * @brief `var a=1,b;`: the children are ::lh_web_js_kind_declarator nodes.
 */
#define lh_web_js_kind_var 22U

/**
 * @def lh_web_js_kind_declarator
 * @brief One name of a `var`: the name in `name`, the initial value its child, if any.
 */
#define lh_web_js_kind_declarator 23U

/**
 * @def lh_web_js_kind_function_declaration
 * @brief `function name(params){body}` as a statement: as ::lh_web_js_kind_function_expression,
 * with a name.
 */
#define lh_web_js_kind_function_declaration 24U

/**
 * @def lh_web_js_kind_expression
 * @brief An expression statement, `expr;`: one child.
 */
#define lh_web_js_kind_expression 25U

/**
 * @def lh_web_js_kind_block
 * @brief `{...}`: the statements are the children.
 */
#define lh_web_js_kind_block 26U

/**
 * @def lh_web_js_kind_if
 * @brief `if(test)consequent else alternate`: two or three children.
 */
#define lh_web_js_kind_if 27U

/**
 * @def lh_web_js_kind_for
 * @brief `for(init;test;update)body`: four children; an absent clause is ::lh_web_js_kind_empty, an
 * `init` may be a `var`.
 */
#define lh_web_js_kind_for 28U

/**
 * @def lh_web_js_kind_return
 * @brief `return;` or `return argument;`: no child or one.
 */
#define lh_web_js_kind_return 29U

/**
 * @def lh_web_js_kind_break
 * @brief `break;`.
 */
#define lh_web_js_kind_break 30U

/**
 * @def lh_web_js_kind_switch
 * @brief `switch(discriminant){cases}`: the discriminant the first child, ::lh_web_js_kind_case and
 * ::lh_web_js_kind_default nodes the rest.
 */
#define lh_web_js_kind_switch 31U

/**
 * @def lh_web_js_kind_case
 * @brief `case test:` and its statements: the test the first child, the statements the rest.
 */
#define lh_web_js_kind_case 32U

/**
 * @def lh_web_js_kind_default
 * @brief `default:` and its statements: the statements are the children.
 */
#define lh_web_js_kind_default 33U

/**
 * @def lh_web_js_kind_try
 * @brief `try{block}catch(name){block}`: the block and a ::lh_web_js_kind_catch node.
 */
#define lh_web_js_kind_try 34U

/**
 * @def lh_web_js_kind_catch
 * @brief `catch(name){block}`: the name in `name`, the block its child.
 */
#define lh_web_js_kind_catch 35U

/**
 * @def lh_web_js_kind_empty
 * @brief Nothing: the empty statement `;`, or an absent clause of a `for`.
 */
#define lh_web_js_kind_empty 36U

#endif /* LH_WEB_JS_KIND_H */
