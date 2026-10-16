/**
 * @file buf.c
 * @brief Implementation of `lh/ui/web/buf.h`.
 */

#include <lh/ui/web/buf.h>

#include <lh/cast/const.h>
#include <lh/memory.h>
#include <lh/str/cstr.h>

lh_void
lh_ui_web_buf_init(lh_ui_web_buf_t *self, lh_byte_ptr mem, lh_usize_t cap)
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
lh_ui_web_buf_ok(const lh_ui_web_buf_t *self)
{
    return (self != lh_null && self->ok) ? lh_bool_true : lh_bool_false;
}

lh_usize_t
lh_ui_web_buf_len(const lh_ui_web_buf_t *self)
{
    return self == lh_null ? 0u : self->len;
}

lh_byte_cptr
lh_ui_web_buf_bytes(const lh_ui_web_buf_t *self)
{
    return self == lh_null ? lh_null : self->mem;
}

lh_void
lh_ui_web_buf_fail(lh_ui_web_buf_t *self)
{
    if (self != lh_null)
    {
        self->ok = lh_bool_false;
    }
}

lh_void
lh_ui_web_buf_lit(lh_ui_web_buf_t *self, const lh_str_ptr p, lh_usize_t n)
{
    if (self == lh_null || !self->ok)
    {
        return;
    }
    if (p == lh_null || n == 0u)
    {
        return;
    }
    /* Keep one byte for an optional NUL from ::lh_ui_web_buf_finish. */
    if (self->mem == lh_null || n + 1u > self->cap - self->len)
    {
        self->ok = lh_bool_false;
        return;
    }
    lh_memory_copy(self->mem + self->len, self->cap - self->len, lh_cast_const(lh_ptr, p), n);
    self->len += n;
}

lh_void
lh_ui_web_buf_raw(lh_ui_web_buf_t *self, const lh_str_ptr s)
{
    if (s == lh_null)
    {
        return;
    }
    lh_ui_web_buf_lit(self, s, lh_str_len(s));
}

lh_void
lh_ui_web_buf_finish(lh_ui_web_buf_t *self)
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
