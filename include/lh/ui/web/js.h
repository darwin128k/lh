/**
 * @file js.h
 * @brief JS statement constructor: vars, functions, stmts → ::lh_ui_web_buf.
 *
 * Expressions are ::lh_ui_web_x; emit them with `lh/ui/web/js/x.h`.
 * This header does not include expression entities.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_JS_H
#define LH_UI_WEB_JS_H

#include <lh/config.h>
#include <lh/ui/web/buf.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/js.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/** `var @p name=@p expr;\n` */
lh_void
lh_ui_web_js_var(lh_ui_web_buf_t *self, const lh_str_ptr name, const lh_str_ptr expr);

/** `var @p decls;\n` — @p decls is the list after `var `, e.g. `a=1,b=2`. */
lh_void
lh_ui_web_js_vars(lh_ui_web_buf_t *self, const lh_str_ptr decls);

/** `function @p name(@p params){@p body}\n` — one-shot body. */
lh_void
lh_ui_web_js_fun(lh_ui_web_buf_t *self, const lh_str_ptr name, const lh_str_ptr params,
                 const lh_str_ptr body);

/** `function @p name(@p params){\n` — body via stmt/if/ret, then ::lh_ui_web_js_fun_close. */
lh_void
lh_ui_web_js_fun_open(lh_ui_web_buf_t *self, const lh_str_ptr name, const lh_str_ptr params);

/** `}\n` closing a ::lh_ui_web_js_fun_open. */
lh_void
lh_ui_web_js_fun_close(lh_ui_web_buf_t *self);

/** `return @p expr;\n` — @p expr may be ::lh_null for a bare `return;`. */
lh_void
lh_ui_web_js_ret(lh_ui_web_buf_t *self, const lh_str_ptr expr);

/** `if(@p cond){\n` */
lh_void
lh_ui_web_js_if(lh_ui_web_buf_t *self, const lh_str_ptr cond);

/** `}else{\n` */
lh_void
lh_ui_web_js_else(lh_ui_web_buf_t *self);

/** `for(@p head){\n` — @p head is the three-clause list without parentheses. */
lh_void
lh_ui_web_js_for(lh_ui_web_buf_t *self, const lh_str_ptr head);

/** `}\n` closing if/else/for. */
lh_void
lh_ui_web_js_end(lh_ui_web_buf_t *self);

/** `@p lhs=@p rhs;\n` */
lh_void
lh_ui_web_js_assign(lh_ui_web_buf_t *self, const lh_str_ptr lhs, const lh_str_ptr rhs);

/** `@p obj.@p prop=@p expr;\n` */
lh_void
lh_ui_web_js_prop(lh_ui_web_buf_t *self, const lh_str_ptr obj, const lh_str_ptr prop,
                  const lh_str_ptr expr);

/** `@p stmt;\n` */
lh_void
lh_ui_web_js_stmt(lh_ui_web_buf_t *self, const lh_str_ptr stmt);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_JS_H */
