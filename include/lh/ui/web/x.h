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
#include <lh/numeric/fixed/types.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/x.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

typedef enum lh_ui_web_x_kind
{
    lh_ui_web_x_kind_id = 0,
    lh_ui_web_x_kind_num,
    lh_ui_web_x_kind_str,
    lh_ui_web_x_kind_null,
    lh_ui_web_x_kind_true,
    lh_ui_web_x_kind_false,
    lh_ui_web_x_kind_dot,
    lh_ui_web_x_kind_idx,
    lh_ui_web_x_kind_call,
    lh_ui_web_x_kind_call_id,
    lh_ui_web_x_kind_bin,
    lh_ui_web_x_kind_un,
    lh_ui_web_x_kind_tern,
    lh_ui_web_x_kind_typeof,
    lh_ui_web_x_kind_obj
} lh_ui_web_x_kind_t;

/**
 * @brief One JS expression node.
 *
 * Pointers in the union refer to other nodes the caller keeps alive.
 */
typedef struct lh_ui_web_x
{
    lh_ui_web_x_kind_t kind;
    union
    {
        const lh_str_ptr id;
        lh_s64_t num;
        const lh_str_ptr str;
        struct
        {
            const struct lh_ui_web_x *obj;
            const lh_str_ptr name;
        } dot;
        struct
        {
            const struct lh_ui_web_x *obj;
            const struct lh_ui_web_x *ix;
        } idx;
        struct
        {
            const struct lh_ui_web_x *fn;
            const struct lh_ui_web_x *const *args;
            lh_u16_t n;
        } call;
        struct
        {
            const lh_str_ptr name;
            const struct lh_ui_web_x *const *args;
            lh_u16_t n;
        } call_id;
        struct
        {
            const struct lh_ui_web_x *a;
            const lh_str_ptr op;
            const struct lh_ui_web_x *b;
        } bin;
        struct
        {
            const lh_str_ptr op;
            const struct lh_ui_web_x *a;
        } un;
        struct
        {
            const struct lh_ui_web_x *c;
            const struct lh_ui_web_x *t;
            const struct lh_ui_web_x *f;
        } tern;
        const struct lh_ui_web_x *typeof_a;
        struct
        {
            const lh_str_ptr *keys;
            const struct lh_ui_web_x *const *vals;
            lh_u16_t n;
        } obj;
    } u;
} lh_ui_web_x_t;

lh_ui_web_x_t
lh_ui_web_x_id(const lh_str_ptr name);

lh_ui_web_x_t
lh_ui_web_x_num(lh_s64_t n);

lh_ui_web_x_t
lh_ui_web_x_str(const lh_str_ptr s);

lh_ui_web_x_t
lh_ui_web_x_null(void);

lh_ui_web_x_t
lh_ui_web_x_true(void);

lh_ui_web_x_t
lh_ui_web_x_false(void);

lh_ui_web_x_t
lh_ui_web_x_dot(const lh_ui_web_x_t *obj, const lh_str_ptr name);

lh_ui_web_x_t
lh_ui_web_x_idx(const lh_ui_web_x_t *obj, const lh_ui_web_x_t *ix);

lh_ui_web_x_t
lh_ui_web_x_call(const lh_ui_web_x_t *fn, const lh_ui_web_x_t *const *args, lh_u16_t n);

/** Call a bare identifier: `name(args…)`. */
lh_ui_web_x_t
lh_ui_web_x_call_id(const lh_str_ptr name, const lh_ui_web_x_t *const *args, lh_u16_t n);

lh_ui_web_x_t
lh_ui_web_x_bin(const lh_ui_web_x_t *a, const lh_str_ptr op, const lh_ui_web_x_t *b);

lh_ui_web_x_t
lh_ui_web_x_un(const lh_str_ptr op, const lh_ui_web_x_t *a);

lh_ui_web_x_t
lh_ui_web_x_tern(const lh_ui_web_x_t *c, const lh_ui_web_x_t *t, const lh_ui_web_x_t *f);

lh_ui_web_x_t
lh_ui_web_x_typeof(const lh_ui_web_x_t *a);

/** Object literal `{keys[i]:vals[i],…}` — @p keys and @p vals must both have @p n entries. */
lh_ui_web_x_t
lh_ui_web_x_obj(const lh_str_ptr *keys, const lh_ui_web_x_t *const *vals, lh_u16_t n);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_X_H */
