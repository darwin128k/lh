/**
 * @file js.h
 * @brief Tiny JS source constructor: vars, functions, stmts → bytes.
 *
 * The web player is built with these calls and then served as HTML. It is not
 * a JS engine and not cJSON: fixed pieces of source, written into a
 * ::lh_byte_t buffer the caller owns.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_JS_H
#define LH_UI_WEB_JS_H

#include <lh/bool.h>
#include <lh/byte/ptr.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/size.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/js.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Scratch that accumulates JS (or HTML) text.
 *
 * One byte of @p cap is reserved for a trailing NUL when ::lh_ui_web_js_finish
 * is used; writes that would not leave that byte set ::ok false.
 */
typedef struct lh_ui_web_js
{
    lh_byte_ptr mem;
    lh_usize_t cap;
    lh_usize_t len;
    lh_bool_t ok;
} lh_ui_web_js_t;

lh_void
lh_ui_web_js_init(lh_ui_web_js_t *self, lh_byte_ptr mem, lh_usize_t cap);

lh_bool_t
lh_ui_web_js_ok(const lh_ui_web_js_t *self);

lh_usize_t
lh_ui_web_js_len(const lh_ui_web_js_t *self);

lh_byte_cptr
lh_ui_web_js_bytes(const lh_ui_web_js_t *self);

/** Append @p n bytes of @p p. */
lh_void
lh_ui_web_js_lit(lh_ui_web_js_t *self, const lh_char_t *p, lh_usize_t n);

/** Append a NUL-terminated C string. */
lh_void
lh_ui_web_js_raw(lh_ui_web_js_t *self, const lh_char_t *s);

/** `var @p name=@p expr;\n` */
lh_void
lh_ui_web_js_var(lh_ui_web_js_t *self, const lh_char_t *name, const lh_char_t *expr);

/** `var @p decls;\n` — @p decls is the list after `var `, e.g. `a=1,b=2`. */
lh_void
lh_ui_web_js_vars(lh_ui_web_js_t *self, const lh_char_t *decls);

/** `function @p name(@p params){@p body}\n` */
lh_void
lh_ui_web_js_fun(lh_ui_web_js_t *self, const lh_char_t *name, const lh_char_t *params,
                 const lh_char_t *body);

/** `@p lhs=@p rhs;\n` */
lh_void
lh_ui_web_js_assign(lh_ui_web_js_t *self, const lh_char_t *lhs, const lh_char_t *rhs);

/** `@p obj.@p prop=@p expr;\n` */
lh_void
lh_ui_web_js_prop(lh_ui_web_js_t *self, const lh_char_t *obj, const lh_char_t *prop,
                  const lh_char_t *expr);

/** One statement plus `;\n` when @p stmt has no terminator of its own. */
lh_void
lh_ui_web_js_stmt(lh_ui_web_js_t *self, const lh_char_t *stmt);

/** Write a trailing NUL without counting it in ::lh_ui_web_js_len. */
lh_void
lh_ui_web_js_finish(lh_ui_web_js_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_JS_H */
