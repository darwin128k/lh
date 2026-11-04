/**
 * @file precedence.h
 * @brief How tightly a JavaScript expression binds (::lh_web_js_precedence_t).
 *
 * The writer (::lh_web_js_writer_try_write_expression) asks every operand for
 * the precedence its context needs and puts parentheses around an expression
 * that binds more loosely, and nowhere else: the tree itself has no
 * parentheses, it has structure. The levels follow the ECMAScript grammar,
 * from the loosest (`,`) to the tightest (an identifier). The operators of
 * one level are in ::lh_web_js_op_get_precedence.
 */

#ifndef LH_WEB_JS_PRECEDENCE_H
#define LH_WEB_JS_PRECEDENCE_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_web_js_precedence_t
 * @brief A binding level. Alias for ::lh_u8_t; values are the
 *        `lh_web_js_precedence_*` constants below, a higher one binding tighter.
 */
typedef lh_u8_t lh_web_js_precedence_t;

/**
 * @def lh_web_js_precedence_lowest
 * @brief Below every expression: the context that takes any expression, a sequence included.
 */
#define lh_web_js_precedence_lowest 0U

/**
 * @def lh_web_js_precedence_sequence
 * @brief `a,b`.
 */
#define lh_web_js_precedence_sequence 1U

/**
 * @def lh_web_js_precedence_assign
 * @brief `a=b`, `a+=b`, ..., right to left.
 */
#define lh_web_js_precedence_assign 2U

/**
 * @def lh_web_js_precedence_conditional
 * @brief `a?b:c`.
 */
#define lh_web_js_precedence_conditional 3U

/**
 * @def lh_web_js_precedence_or
 * @brief `a||b`.
 */
#define lh_web_js_precedence_or 4U

/**
 * @def lh_web_js_precedence_and
 * @brief `a&&b`.
 */
#define lh_web_js_precedence_and 5U

/**
 * @def lh_web_js_precedence_bit_or
 * @brief `a|b`.
 */
#define lh_web_js_precedence_bit_or 6U

/**
 * @def lh_web_js_precedence_bit_xor
 * @brief `a^b`.
 */
#define lh_web_js_precedence_bit_xor 7U

/**
 * @def lh_web_js_precedence_bit_and
 * @brief `a&b`.
 */
#define lh_web_js_precedence_bit_and 8U

/**
 * @def lh_web_js_precedence_equality
 * @brief `==`, `!=`, `===`, `!==`.
 */
#define lh_web_js_precedence_equality 9U

/**
 * @def lh_web_js_precedence_relational
 * @brief `<`, `>`, `<=`, `>=`, `in`, `instanceof`.
 */
#define lh_web_js_precedence_relational 10U

/**
 * @def lh_web_js_precedence_shift
 * @brief `<<`, `>>`, `>>>`.
 */
#define lh_web_js_precedence_shift 11U

/**
 * @def lh_web_js_precedence_additive
 * @brief `+`, `-`.
 */
#define lh_web_js_precedence_additive 12U

/**
 * @def lh_web_js_precedence_multiplicative
 * @brief `*`, `/`, `%`.
 */
#define lh_web_js_precedence_multiplicative 13U

/**
 * @def lh_web_js_precedence_unary
 * @brief `!a`, `-a`, `typeof a`, `++a`, ...
 */
#define lh_web_js_precedence_unary 14U

/**
 * @def lh_web_js_precedence_postfix
 * @brief `a++`, `a--`.
 */
#define lh_web_js_precedence_postfix 15U

/**
 * @def lh_web_js_precedence_call
 * @brief `a.b`, `a[b]`, `a(b)`, `new a(b)`.
 */
#define lh_web_js_precedence_call 16U

/**
 * @def lh_web_js_precedence_primary
 * @brief Identifiers, literals, `function(){}`, `[...]`, `{...}`: never parenthesized.
 */
#define lh_web_js_precedence_primary 17U

#endif /* LH_WEB_JS_PRECEDENCE_H */
