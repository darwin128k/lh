/**
 * @file x.h
 * @brief Emit ::lh_ui_web_x into a ::lh_ui_web_buf / JS statements.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_JS_X_H
#define LH_UI_WEB_JS_X_H

#include <lh/config.h>
#include <lh/ui/web/buf.h>
#include <lh/ui/web/x.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/js/x.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/** Append @p n in decimal. */
lh_void
lh_ui_web_js_put_s64(lh_ui_web_buf_t *self, lh_s64_t n);

/** Append @p s as a single-quoted JavaScript string literal, escaped. */
lh_void
lh_ui_web_js_put_str_lit(lh_ui_web_buf_t *self, const lh_str_ptr s);

/** Append an argument list `(a,b,...)` of @p n expressions. */
lh_void
lh_ui_web_js_emit_args(lh_ui_web_buf_t *self, const lh_ui_web_x_t *const *args, lh_u16_t n);

/** Append expression node @p x as JavaScript, parenthesised so precedence never matters. */
lh_void
lh_ui_web_js_emit_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x);

/** Emit a ::lh_ui_web_x expression (no trailing semicolon). */
lh_void
lh_ui_web_js_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x);

/** `return <x>;\n` — @p x may be ::lh_null for a bare `return;`. */
lh_void
lh_ui_web_js_ret_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x);

/** `if(<x>){\n` */
lh_void
lh_ui_web_js_if_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *cond);

/** `var @p name=<x>;\n` */
lh_void
lh_ui_web_js_var_x(lh_ui_web_buf_t *self, const lh_str_ptr name, const lh_ui_web_x_t *x);

/** `<lhs>=<rhs>;\n` */
lh_void
lh_ui_web_js_assign_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *lhs, const lh_ui_web_x_t *rhs);

/** `<x>;\n` */
lh_void
lh_ui_web_js_stmt_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_JS_X_H */
