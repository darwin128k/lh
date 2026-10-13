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
#include <lh/ui/web/x.h>
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

/** `function @p name(@p params){@p body}\n` — one-shot body. */
lh_void
lh_ui_web_js_fun(lh_ui_web_js_t *self, const lh_char_t *name, const lh_char_t *params,
                 const lh_char_t *body);

/** `function @p name(@p params){\n` — body follows via stmt/if/ret, then ::lh_ui_web_js_fun_close. */
lh_void
lh_ui_web_js_fun_open(lh_ui_web_js_t *self, const lh_char_t *name, const lh_char_t *params);

/** `}\n` closing a ::lh_ui_web_js_fun_open. */
lh_void
lh_ui_web_js_fun_close(lh_ui_web_js_t *self);

/** `return @p expr;\n` — @p expr may be ::lh_null for a bare `return;`. */
lh_void
lh_ui_web_js_ret(lh_ui_web_js_t *self, const lh_char_t *expr);

/** `if(@p cond){\n` */
lh_void
lh_ui_web_js_if(lh_ui_web_js_t *self, const lh_char_t *cond);

/** `}else{\n` */
lh_void
lh_ui_web_js_else(lh_ui_web_js_t *self);

/** `for(@p head){\n` — @p head is the three-clause list without parentheses. */
lh_void
lh_ui_web_js_for(lh_ui_web_js_t *self, const lh_char_t *head);

/** `}\n` closing if/else/for. */
lh_void
lh_ui_web_js_end(lh_ui_web_js_t *self);

/** `@p lhs=@p rhs;\n` */
lh_void
lh_ui_web_js_assign(lh_ui_web_js_t *self, const lh_char_t *lhs, const lh_char_t *rhs);

/** `@p obj.@p prop=@p expr;\n` */
lh_void
lh_ui_web_js_prop(lh_ui_web_js_t *self, const lh_char_t *obj, const lh_char_t *prop,
                  const lh_char_t *expr);

/** `@p stmt;\n` */
lh_void
lh_ui_web_js_stmt(lh_ui_web_js_t *self, const lh_char_t *stmt);

/** Emit a ::lh_ui_web_x expression (no trailing semicolon). */
lh_void
lh_ui_web_js_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *x);

/** `return <x>;\n` — @p x may be ::lh_null for a bare `return;`. */
lh_void
lh_ui_web_js_ret_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *x);

/** `if(<x>){\n` */
lh_void
lh_ui_web_js_if_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *cond);

/** `var @p name=<x>;\n` */
lh_void
lh_ui_web_js_var_x(lh_ui_web_js_t *self, const lh_char_t *name, const lh_ui_web_x_t *x);

/** `<lhs>=<rhs>;\n` */
lh_void
lh_ui_web_js_assign_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *lhs, const lh_ui_web_x_t *rhs);

/** `<x>;\n` */
lh_void
lh_ui_web_js_stmt_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *x);

/** Write a trailing NUL without counting it in ::lh_ui_web_js_len. */
lh_void
lh_ui_web_js_finish(lh_ui_web_js_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_JS_H */
