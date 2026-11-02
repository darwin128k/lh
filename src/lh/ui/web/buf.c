/**
 * @file buf.c
 * @brief Implementation of `lh/ui/web/buf.h`.
 */
#include <lh/ui/web/buf.h>

#include <lh/attribute/unused.h>
#include <lh/char/map.h>

#include <lh/cast/const.h>
#include <lh/memory.h>
#include <lh/util/str/ptr.h>

lh_byte_ptr
lh_ui_web_buf_get_mem(const lh_ui_web_buf_t *self)
{
    return self->mem;
}

lh_void
lh_ui_web_buf_set_mem(lh_ui_web_buf_t *self, lh_byte_ptr mem)
{
    self->mem = mem;
}

lh_usize_t
lh_ui_web_buf_get_cap(const lh_ui_web_buf_t *self)
{
    return self->cap;
}

lh_void
lh_ui_web_buf_set_cap(lh_ui_web_buf_t *self, lh_usize_t cap)
{
    self->cap = cap;
}

lh_usize_t
lh_ui_web_buf_get_len(const lh_ui_web_buf_t *self)
{
    return self->len;
}

lh_void
lh_ui_web_buf_set_len(lh_ui_web_buf_t *self, lh_usize_t len)
{
    self->len = len;
}

lh_bool_t
lh_ui_web_buf_is_ok(const lh_ui_web_buf_t *self)
{
    return self->ok;
}

lh_void
lh_ui_web_buf_set_ok(lh_ui_web_buf_t *self, lh_bool_t ok)
{
    self->ok = ok;
}

lh_void
lh_ui_web_buf_init(lh_ui_web_buf_t *self, lh_byte_ptr mem, lh_usize_t cap)
{
    if (self == lh_null)
    {
        return;
    }
    lh_ui_web_buf_set_mem(self, mem);
    lh_ui_web_buf_set_cap(self, cap);
    lh_ui_web_buf_set_len(self, 0u);
    lh_ui_web_buf_set_ok(self, (mem != lh_null && cap > 1u) ? lh_bool_true : lh_bool_false);
}

lh_bool_t
lh_ui_web_buf_ok(const lh_ui_web_buf_t *self)
{
    return (self != lh_null && lh_ui_web_buf_is_ok(self)) ? lh_bool_true : lh_bool_false;
}

lh_usize_t
lh_ui_web_buf_len(const lh_ui_web_buf_t *self)
{
    return self == lh_null ? 0u : lh_ui_web_buf_get_len(self);
}

lh_byte_cptr
lh_ui_web_buf_bytes(const lh_ui_web_buf_t *self)
{
    return self == lh_null ? lh_null : lh_ui_web_buf_get_mem(self);
}

lh_void
lh_ui_web_buf_fail(lh_ui_web_buf_t *self)
{
    if (self != lh_null)
    {
        lh_ui_web_buf_set_ok(self, lh_bool_false);
    }
}

lh_void
lh_ui_web_buf_lit(lh_ui_web_buf_t *self, const lh_str_ptr p, lh_usize_t n)
{
    if (self == lh_null || !lh_ui_web_buf_is_ok(self))
    {
        return;
    }
    if (p == lh_null || n == 0u)
    {
        return;
    }
    /* Keep one byte for an optional NUL without overflowing n + 1. */
    const lh_usize_t len = lh_ui_web_buf_get_len(self);
    const lh_usize_t cap = lh_ui_web_buf_get_cap(self);
    lh_byte_ptr mem = lh_ui_web_buf_get_mem(self);
    if (mem == lh_null || len >= cap || n >= cap - len)
    {
        lh_ui_web_buf_set_ok(self, lh_bool_false);
        return;
    }
    LH_ATTRIBUTE_UNUSED(lh_memory_std_copy(mem + len, lh_cast_const(lh_ptr, p), n));
    lh_ui_web_buf_set_len(self, len + n);
}

lh_void
lh_ui_web_buf_finish(lh_ui_web_buf_t *self)
{
    /* A failed buffer is terminated too: every write keeps one byte free for this. */
    if (self == lh_null || lh_ui_web_buf_get_mem(self) == lh_null)
    {
        return;
    }
    if (lh_ui_web_buf_get_len(self) >= lh_ui_web_buf_get_cap(self))
    {
        lh_ui_web_buf_set_ok(self, lh_bool_false);
        return;
    }
    lh_ui_web_buf_get_mem(self)[lh_ui_web_buf_get_len(self)] = lh_char_map_nul;
}

lh_void
lh_ui_web_buf_raw(lh_ui_web_buf_t *self, const lh_str_ptr s)
{
    if (s == lh_null)
    {
        return;
    }
    lh_ui_web_buf_lit(self, s, lh_str_ptr_len(s));
}
