/**
 * @file js.c
 * @brief Implementation of `lh/ui/web/js.h`.
 */

#include <lh/ui/web/js.h>

#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/str/cstr.h>
#include <lh/util/addr.h>

static lh_void
js_put(lh_ui_web_js_t *self, const lh_char_t *p, lh_usize_t n)
{
    if (self == lh_null || !self->ok)
    {
        return;
    }
    /* Keep one byte for an optional NUL from ::lh_ui_web_js_finish. */
    if (self->mem == lh_null || n + 1u > self->cap - self->len)
    {
        self->ok = lh_bool_false;
        return;
    }
    lh_memory_copy(self->mem + self->len, self->cap - self->len, lh_cast_const(lh_ptr, p), n);
    self->len += n;
}

lh_void
lh_ui_web_js_init(lh_ui_web_js_t *self, lh_byte_ptr mem, lh_usize_t cap)
{
    if (self == lh_null)
    {
        return;
    }
    self->mem = mem;
    self->cap = cap;
    self->len = 0u;
    self->ok = (mem != lh_null && cap > 1u) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_ui_web_js_ok(const lh_ui_web_js_t *self)
{
    return (self != lh_null && self->ok) ? lh_bool_true : lh_bool_false;
}

lh_usize_t
lh_ui_web_js_len(const lh_ui_web_js_t *self)
{
    return self == lh_null ? 0u : self->len;
}

lh_byte_cptr
lh_ui_web_js_bytes(const lh_ui_web_js_t *self)
{
    return self == lh_null ? lh_null : self->mem;
}

lh_void
lh_ui_web_js_lit(lh_ui_web_js_t *self, const lh_char_t *p, lh_usize_t n)
{
    if (p == lh_null || n == 0u)
    {
        return;
    }
    js_put(self, p, n);
}

lh_void
lh_ui_web_js_raw(lh_ui_web_js_t *self, const lh_char_t *s)
{
    if (s == lh_null)
    {
        return;
    }
    js_put(self, s, lh_str_len(s));
}

lh_void
lh_ui_web_js_var(lh_ui_web_js_t *self, const lh_char_t *name, const lh_char_t *expr)
{
    js_put(self, "var ", 4u);
    lh_ui_web_js_raw(self, name);
    js_put(self, "=", 1u);
    lh_ui_web_js_raw(self, expr);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_vars(lh_ui_web_js_t *self, const lh_char_t *decls)
{
    js_put(self, "var ", 4u);
    lh_ui_web_js_raw(self, decls);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_fun(lh_ui_web_js_t *self, const lh_char_t *name, const lh_char_t *params,
                 const lh_char_t *body)
{
    js_put(self, "function ", 9u);
    lh_ui_web_js_raw(self, name);
    js_put(self, "(", 1u);
    lh_ui_web_js_raw(self, params);
    js_put(self, "){", 2u);
    lh_ui_web_js_raw(self, body);
    js_put(self, "}\n", 2u);
}

lh_void
lh_ui_web_js_assign(lh_ui_web_js_t *self, const lh_char_t *lhs, const lh_char_t *rhs)
{
    lh_ui_web_js_raw(self, lhs);
    js_put(self, "=", 1u);
    lh_ui_web_js_raw(self, rhs);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_prop(lh_ui_web_js_t *self, const lh_char_t *obj, const lh_char_t *prop,
                  const lh_char_t *expr)
{
    lh_ui_web_js_raw(self, obj);
    js_put(self, ".", 1u);
    lh_ui_web_js_raw(self, prop);
    js_put(self, "=", 1u);
    lh_ui_web_js_raw(self, expr);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_stmt(lh_ui_web_js_t *self, const lh_char_t *stmt)
{
    lh_ui_web_js_raw(self, stmt);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_finish(lh_ui_web_js_t *self)
{
    if (self == lh_null || !self->ok || self->mem == lh_null)
    {
        return;
    }
    if (self->len >= self->cap)
    {
        self->ok = lh_bool_false;
        return;
    }
    self->mem[self->len] = 0;
}
