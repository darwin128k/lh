/**
 * @file js.c
 * @brief Implementation of `lh/ui/web/js.h`.
 */

#include <lh/ui/web/js.h>

lh_void
lh_ui_web_js_var(lh_ui_web_buf_t *self, const lh_str_ptr name, const lh_str_ptr expr)
{
    lh_ui_web_buf_lit(self, "var ", 4u);
    lh_ui_web_buf_raw(self, name);
    lh_ui_web_buf_lit(self, "=", 1u);
    lh_ui_web_buf_raw(self, expr);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_vars(lh_ui_web_buf_t *self, const lh_str_ptr decls)
{
    lh_ui_web_buf_lit(self, "var ", 4u);
    lh_ui_web_buf_raw(self, decls);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_fun_open(lh_ui_web_buf_t *self, const lh_str_ptr name, const lh_str_ptr params)
{
    lh_ui_web_buf_lit(self, "function ", 9u);
    lh_ui_web_buf_raw(self, name);
    lh_ui_web_buf_lit(self, "(", 1u);
    lh_ui_web_buf_raw(self, params);
    lh_ui_web_buf_lit(self, "){\n", 3u);
}

lh_void
lh_ui_web_js_fun_close(lh_ui_web_buf_t *self)
{
    lh_ui_web_buf_lit(self, "}\n", 2u);
}

lh_void
lh_ui_web_js_fun(lh_ui_web_buf_t *self, const lh_str_ptr name, const lh_str_ptr params,
                 const lh_str_ptr body)
{
    lh_ui_web_js_fun_open(self, name, params);
    lh_ui_web_buf_raw(self, body);
    lh_ui_web_buf_lit(self, "\n", 1u);
    lh_ui_web_js_fun_close(self);
}

lh_void
lh_ui_web_js_ret(lh_ui_web_buf_t *self, const lh_str_ptr expr)
{
    lh_ui_web_buf_lit(self, "return", 6u);
    if (expr != lh_null && expr[0] != '\0')
    {
        lh_ui_web_buf_lit(self, " ", 1u);
        lh_ui_web_buf_raw(self, expr);
    }
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_if(lh_ui_web_buf_t *self, const lh_str_ptr cond)
{
    lh_ui_web_buf_lit(self, "if(", 3u);
    lh_ui_web_buf_raw(self, cond);
    lh_ui_web_buf_lit(self, "){\n", 3u);
}

lh_void
lh_ui_web_js_else(lh_ui_web_buf_t *self)
{
    lh_ui_web_buf_lit(self, "}else{\n", 7u);
}

lh_void
lh_ui_web_js_for(lh_ui_web_buf_t *self, const lh_str_ptr head)
{
    lh_ui_web_buf_lit(self, "for(", 4u);
    lh_ui_web_buf_raw(self, head);
    lh_ui_web_buf_lit(self, "){\n", 3u);
}

lh_void
lh_ui_web_js_end(lh_ui_web_buf_t *self)
{
    lh_ui_web_buf_lit(self, "}\n", 2u);
}

lh_void
lh_ui_web_js_assign(lh_ui_web_buf_t *self, const lh_str_ptr lhs, const lh_str_ptr rhs)
{
    lh_ui_web_buf_raw(self, lhs);
    lh_ui_web_buf_lit(self, "=", 1u);
    lh_ui_web_buf_raw(self, rhs);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_prop(lh_ui_web_buf_t *self, const lh_str_ptr obj, const lh_str_ptr prop,
                  const lh_str_ptr expr)
{
    lh_ui_web_buf_raw(self, obj);
    lh_ui_web_buf_lit(self, ".", 1u);
    lh_ui_web_buf_raw(self, prop);
    lh_ui_web_buf_lit(self, "=", 1u);
    lh_ui_web_buf_raw(self, expr);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_stmt(lh_ui_web_buf_t *self, const lh_str_ptr stmt)
{
    lh_ui_web_buf_raw(self, stmt);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}
