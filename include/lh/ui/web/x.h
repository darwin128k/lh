/**
 * @file x.h
 * @brief JS expression entities for ::lh_ui_web_js — no source fragments.
 *
 * Build a small tree on the stack (`lh_ui_web_x_id`, `lh_ui_web_x_bin`, …) and
 * emit it with ::lh_ui_web_js_x / ::lh_ui_web_js_ret_x / ::lh_ui_web_js_if_x.
 * Children must outlive the emit call (usual stack pattern).
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_X_H
#define LH_UI_WEB_X_H

#include <lh/bool.h>
#include <lh/str/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/ui/web/x/kind.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/x.h requires LH_LIBRARY_OPTION_UI"
#endif

/**
 * @brief One JS expression node.
 *
 * Pointers in the union refer to other nodes the caller keeps alive.
 */
typedef struct lh_ui_web_x
{
    lh_ui_web_x_kind_t kind; /**< Which member of @c u is set. */
    /** @brief The operands of the node, by @c kind. */
    union
    {
        /** @brief Identifier text, for ::lh_ui_web_x_kind_id. */
        const lh_str_ptr id;
        /** @brief Whole number, for ::lh_ui_web_x_kind_num. */
        lh_s64_t num;
        /** @brief String text, for ::lh_ui_web_x_kind_str. */
        const lh_str_ptr str;
        /** @brief Member access, for ::lh_ui_web_x_kind_dot. */
        struct
        {
            /** @brief Object expression. */
            const struct lh_ui_web_x *obj;
            /** @brief Member name. */
            const lh_str_ptr name;
        } dot;
        /** @brief Index access, for ::lh_ui_web_x_kind_idx. */
        struct
        {
            /** @brief Object expression. */
            const struct lh_ui_web_x *obj;
            /** @brief Index expression. */
            const struct lh_ui_web_x *ix;
        } idx;
        /** @brief Call of an expression, for ::lh_ui_web_x_kind_call. */
        struct
        {
            /** @brief Callee expression. */
            const struct lh_ui_web_x *fn;
            /** @brief Argument expressions, @c n of them. */
            const struct lh_ui_web_x *const *args;
            /** @brief How many arguments. */
            lh_u16_t n;
        } call;
        /** @brief Call of a bare identifier, for ::lh_ui_web_x_kind_call_id. */
        struct
        {
            /** @brief Name of the function to call. */
            const lh_str_ptr name;
            /** @brief Argument expressions, @c n of them. */
            const struct lh_ui_web_x *const *args;
            /** @brief How many arguments. */
            lh_u16_t n;
        } call_id;
        /** @brief Binary operation, for ::lh_ui_web_x_kind_bin. */
        struct
        {
            /** @brief Left operand. */
            const struct lh_ui_web_x *a;
            /** @brief Operator text. */
            const lh_str_ptr op;
            /** @brief Right operand. */
            const struct lh_ui_web_x *b;
        } bin;
        /** @brief Unary operation, for ::lh_ui_web_x_kind_un. */
        struct
        {
            /** @brief Operator text. */
            const lh_str_ptr op;
            /** @brief Operand. */
            const struct lh_ui_web_x *a;
        } un;
        /** @brief Conditional, for ::lh_ui_web_x_kind_tern. */
        struct
        {
            /** @brief Condition. */
            const struct lh_ui_web_x *c;
            /** @brief Value when the condition is truthy. */
            const struct lh_ui_web_x *t;
            /** @brief Value otherwise. */
            const struct lh_ui_web_x *f;
        } tern;
        /** @brief Operand of ::lh_ui_web_x_kind_typeof. */
        const struct lh_ui_web_x *typeof_a;
        /** @brief Object literal, for ::lh_ui_web_x_kind_obj. */
        struct
        {
            /** @brief Key names, @c n of them. */
            const lh_str_ptr *keys;
            /** @brief Values, one per key, @c n of them. */
            const struct lh_ui_web_x *const *vals;
            /** @brief How many entries the object holds. */
            lh_u16_t n;
        } obj;
    } u;
} lh_ui_web_x_t; /**< One JS expression node. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Identifier @p name, written as is.
 * @param name Identifier text. Not copied.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_id(const lh_str_ptr name, lh_ui_web_x_t *out);

/**
 * @brief Number literal @p n.
 * @param n The value.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_num(lh_s64_t n, lh_ui_web_x_t *out);

/**
 * @brief String literal @p s, quoted and escaped on emit.
 * @param s NUL-terminated text. Not copied.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_str(const lh_str_ptr s, lh_ui_web_x_t *out);

/**
 * @brief The `null` literal.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_null(lh_ui_web_x_t *out);

/**
 * @brief The `true` literal.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_true(lh_ui_web_x_t *out);

/**
 * @brief The `false` literal.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_false(lh_ui_web_x_t *out);

/**
 * @brief Member access `(obj).name`.
 * @param obj  Object expression. Not copied.
 * @param name Member name. Not copied.
 * @param out  Receives the node.
 */
lh_void
lh_ui_web_x_dot(const lh_ui_web_x_t *obj, const lh_str_ptr name, lh_ui_web_x_t *out);

/**
 * @brief Index `(obj)[ix]`.
 * @param obj Object expression. Not copied.
 * @param ix  Index expression. Not copied.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_idx(const lh_ui_web_x_t *obj, const lh_ui_web_x_t *ix, lh_ui_web_x_t *out);

/**
 * @brief Call `(fn)(args...)`.
 * @param fn   Callee expression. Not copied.
 * @param args @p n argument expressions. Not copied.
 * @param n    How many arguments.
 * @param out  Receives the node.
 */
lh_void
lh_ui_web_x_call(const lh_ui_web_x_t *fn, const lh_ui_web_x_t *const *args, lh_u16_t n,
                 lh_ui_web_x_t *out);

/**
 * @brief Call of a bare identifier, written as `name(args...)`.
 * @param name Name of the function to call. Not copied.
 * @param args @p n argument expressions. Not copied.
 * @param n    How many arguments.
 * @param out  Receives the node.
 */
lh_void
lh_ui_web_x_call_id(const lh_str_ptr name, const lh_ui_web_x_t *const *args, lh_u16_t n,
                    lh_ui_web_x_t *out);

/**
 * @brief Binary operation `(a)op(b)`.
 * @param a  Left operand. Not copied.
 * @param op Operator text, e.g. `"+"` or `"==="`.
 * @param b  Right operand. Not copied.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_bin(const lh_ui_web_x_t *a, const lh_str_ptr op, const lh_ui_web_x_t *b,
                lh_ui_web_x_t *out);

/**
 * @brief Unary operation `op(a)`.
 * @param op  Operator text, e.g. `"!"` or `"-"`.
 * @param a   Operand. Not copied.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_un(const lh_str_ptr op, const lh_ui_web_x_t *a, lh_ui_web_x_t *out);

/**
 * @brief Conditional `(c)?(t):(f)`.
 * @param c   Condition. Not copied.
 * @param t   Value when @p c is truthy. Not copied.
 * @param f   Value otherwise. Not copied.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_tern(const lh_ui_web_x_t *c, const lh_ui_web_x_t *t, const lh_ui_web_x_t *f,
                 lh_ui_web_x_t *out);

/**
 * @brief `typeof(a)`.
 * @param a   Operand. Not copied.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_x_typeof(const lh_ui_web_x_t *a, lh_ui_web_x_t *out);

/**
 * @brief Object literal, written as `{key:value,...}`.
 *
 * @p keys and @p vals must both have @p n entries.
 *
 * @param keys Key names, in order. Not copied.
 * @param vals Values, one per key, in order. Not copied.
 * @param n    How many entries the object holds.
 * @param out  Receives the node.
 */
lh_void
lh_ui_web_x_obj(const lh_str_ptr *keys, const lh_ui_web_x_t *const *vals, lh_u16_t n,
                lh_ui_web_x_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_X_H */
