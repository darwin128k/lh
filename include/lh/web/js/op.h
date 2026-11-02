/**
 * @file op.h
 * @brief The JavaScript operators an ::lh_web_js_node_t can carry
 *        (::lh_web_js_op_t), with their text and their precedence.
 *
 * A unary, update, binary or assignment node names its operator with one of
 * these constants, never with text: the writer takes the text from
 * ::lh_web_js_op_get_text, so a tree cannot carry a code fragment in place of
 * an operator. `+` and `-` exist twice, as unary (::lh_web_js_op_plus,
 * ::lh_web_js_op_minus) and as binary operators (::lh_web_js_op_add,
 * ::lh_web_js_op_sub), since they bind differently. The logical `&&` and `||`
 * are binary operators here. `**` and `??` are not in the set: no page needs
 * them, and they carry grammar exceptions of their own (a unary operand of
 * `**`, `??` mixed with `||`) that the writer would have to know.
 */

#ifndef LH_WEB_JS_OP_H
#define LH_WEB_JS_OP_H

#include <lh/web/js/precedence.h>
#include <lh/str/view.h>

/**
 * @typedef lh_web_js_op_t
 * @brief An operator. Alias for ::lh_u8_t; values are the `lh_web_js_op_*`
 *        constants below.
 */
typedef lh_u8_t lh_web_js_op_t;

/**
 * @def lh_web_js_op_none
 * @brief No operator: the `op` of a node that has none.
 */
#define lh_web_js_op_none 0U

/**
 * @def lh_web_js_op_not
 * @brief `!a`.
 */
#define lh_web_js_op_not 1U

/**
 * @def lh_web_js_op_bit_not
 * @brief `~a`.
 */
#define lh_web_js_op_bit_not 2U

/**
 * @def lh_web_js_op_plus
 * @brief `+a`.
 */
#define lh_web_js_op_plus 3U

/**
 * @def lh_web_js_op_minus
 * @brief `-a`.
 */
#define lh_web_js_op_minus 4U

/**
 * @def lh_web_js_op_typeof
 * @brief `typeof a`.
 */
#define lh_web_js_op_typeof 5U

/**
 * @def lh_web_js_op_void
 * @brief `void a`.
 */
#define lh_web_js_op_void 6U

/**
 * @def lh_web_js_op_delete
 * @brief `delete a`.
 */
#define lh_web_js_op_delete 7U

/**
 * @def lh_web_js_op_increment
 * @brief `++a` / `a++` (::lh_web_js_kind_update).
 */
#define lh_web_js_op_increment 8U

/**
 * @def lh_web_js_op_decrement
 * @brief `--a` / `a--` (::lh_web_js_kind_update).
 */
#define lh_web_js_op_decrement 9U

/**
 * @def lh_web_js_op_add
 * @brief `a+b`.
 */
#define lh_web_js_op_add 10U

/**
 * @def lh_web_js_op_sub
 * @brief `a-b`.
 */
#define lh_web_js_op_sub 11U

/**
 * @def lh_web_js_op_mul
 * @brief `a*b`.
 */
#define lh_web_js_op_mul 12U

/**
 * @def lh_web_js_op_div
 * @brief `a/b`.
 */
#define lh_web_js_op_div 13U

/**
 * @def lh_web_js_op_mod
 * @brief `a%b`.
 */
#define lh_web_js_op_mod 14U

/**
 * @def lh_web_js_op_shl
 * @brief `a<<b`.
 */
#define lh_web_js_op_shl 15U

/**
 * @def lh_web_js_op_shr
 * @brief `a>>b`.
 */
#define lh_web_js_op_shr 16U

/**
 * @def lh_web_js_op_ushr
 * @brief `a>>>b`.
 */
#define lh_web_js_op_ushr 17U

/**
 * @def lh_web_js_op_lt
 * @brief `a<b`.
 */
#define lh_web_js_op_lt 18U

/**
 * @def lh_web_js_op_gt
 * @brief `a>b`.
 */
#define lh_web_js_op_gt 19U

/**
 * @def lh_web_js_op_le
 * @brief `a<=b`.
 */
#define lh_web_js_op_le 20U

/**
 * @def lh_web_js_op_ge
 * @brief `a>=b`.
 */
#define lh_web_js_op_ge 21U

/**
 * @def lh_web_js_op_in
 * @brief `a in b`.
 */
#define lh_web_js_op_in 22U

/**
 * @def lh_web_js_op_instanceof
 * @brief `a instanceof b`.
 */
#define lh_web_js_op_instanceof 23U

/**
 * @def lh_web_js_op_eq
 * @brief `a==b`.
 */
#define lh_web_js_op_eq 24U

/**
 * @def lh_web_js_op_ne
 * @brief `a!=b`.
 */
#define lh_web_js_op_ne 25U

/**
 * @def lh_web_js_op_strict_eq
 * @brief `a===b`.
 */
#define lh_web_js_op_strict_eq 26U

/**
 * @def lh_web_js_op_strict_ne
 * @brief `a!==b`.
 */
#define lh_web_js_op_strict_ne 27U

/**
 * @def lh_web_js_op_bit_and
 * @brief `a&b`.
 */
#define lh_web_js_op_bit_and 28U

/**
 * @def lh_web_js_op_bit_xor
 * @brief `a^b`.
 */
#define lh_web_js_op_bit_xor 29U

/**
 * @def lh_web_js_op_bit_or
 * @brief `a|b`.
 */
#define lh_web_js_op_bit_or 30U

/**
 * @def lh_web_js_op_and
 * @brief `a&&b`.
 */
#define lh_web_js_op_and 31U

/**
 * @def lh_web_js_op_or
 * @brief `a||b`.
 */
#define lh_web_js_op_or 32U

/**
 * @def lh_web_js_op_assign
 * @brief `a=b`.
 */
#define lh_web_js_op_assign 33U

/**
 * @def lh_web_js_op_add_assign
 * @brief `a+=b`.
 */
#define lh_web_js_op_add_assign 34U

/**
 * @def lh_web_js_op_sub_assign
 * @brief `a-=b`.
 */
#define lh_web_js_op_sub_assign 35U

/**
 * @def lh_web_js_op_mul_assign
 * @brief `a*=b`.
 */
#define lh_web_js_op_mul_assign 36U

/**
 * @def lh_web_js_op_div_assign
 * @brief `a/=b`.
 */
#define lh_web_js_op_div_assign 37U

/**
 * @def lh_web_js_op_mod_assign
 * @brief `a%=b`.
 */
#define lh_web_js_op_mod_assign 38U

/**
 * @def lh_web_js_op_shl_assign
 * @brief `a<<=b`.
 */
#define lh_web_js_op_shl_assign 39U

/**
 * @def lh_web_js_op_shr_assign
 * @brief `a>>=b`.
 */
#define lh_web_js_op_shr_assign 40U

/**
 * @def lh_web_js_op_ushr_assign
 * @brief `a>>>=b`.
 */
#define lh_web_js_op_ushr_assign 41U

/**
 * @def lh_web_js_op_bit_and_assign
 * @brief `a&=b`.
 */
#define lh_web_js_op_bit_and_assign 42U

/**
 * @def lh_web_js_op_bit_xor_assign
 * @brief `a^=b`.
 */
#define lh_web_js_op_bit_xor_assign 43U

/**
 * @def lh_web_js_op_bit_or_assign
 * @brief `a|=b`.
 */
#define lh_web_js_op_bit_or_assign 44U

/**
 * @def LH_WEB_JS_OP_COUNT
 * @brief How many operators there are: every ::lh_web_js_op_t is below it.
 */
#define LH_WEB_JS_OP_COUNT 45U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The text of @p op as JavaScript writes it: `+`, `===`, `typeof`, ...
 *
 * @param op The operator.
 * @return A view of static text; the empty view for ::lh_web_js_op_none and
 *         for a value that is no operator.
 */
lh_str_view_t
lh_web_js_op_get_text(lh_web_js_op_t op);

/**
 * @brief How tightly an expression with @p op binds: ::lh_web_js_precedence_unary
 *        for the unary and update operators, the operator's own level for a
 *        binary one, ::lh_web_js_precedence_assign for an assignment.
 *
 * @param op The operator.
 * @return Its level; ::lh_web_js_precedence_lowest for ::lh_web_js_op_none and
 *         for a value that is no operator.
 */
lh_web_js_precedence_t
lh_web_js_op_get_precedence(lh_web_js_op_t op);

/**
 * @brief True when @p op is a word (`typeof`, `void`, `delete`, `in`,
 *        `instanceof`): the writer then separates it from its operands with
 *        spaces.
 */
lh_bool_t
lh_web_js_op_is_word(lh_web_js_op_t op);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_JS_OP_H */
