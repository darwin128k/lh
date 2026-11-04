/**
 * @file tap.c
 * @brief Implementation of `lh/ui/web/host/tap.h`.
 */

#include <lh/ui/web/host/tap.h>
#include <lh/core/assert/runtime.h>

const lh_io_reader_t *
lh_ui_web_host_tap_get_source(const lh_ui_web_host_tap_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->source);
}

lh_io_reader_t *
lh_ui_web_host_tap_get_source_mut(lh_ui_web_host_tap_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->source);
}

lh_ssize_t
lh_ui_web_host_tap_get_last(const lh_ui_web_host_tap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last;
}

lh_void
lh_ui_web_host_tap_set_last(lh_ui_web_host_tap_t *self, lh_ssize_t value)
{
    lh_assert_runtime_ref(self);
    self->last = value;
}

lh_void
lh_ui_web_host_tap_get_reader(lh_ui_web_host_tap_t *self, lh_io_reader_t *out)
{
    lh_io_reader_init(out, lh_ui_web_host_tap_read, self);
}

lh_void
lh_ui_web_host_tap_init(lh_ui_web_host_tap_t *self, const lh_io_reader_t *source)
{
    lh_io_reader_assign(lh_ui_web_host_tap_get_source_mut(self), source);
    lh_ui_web_host_tap_set_last(self, LH_UI_WEB_HOST_TAP_NOTHING_YET);
}

lh_ssize_t
lh_ui_web_host_tap_read(lh_self_ptr context, lh_ptr out, lh_usize_t size)
{
    lh_ui_web_host_tap_t *self = lh_ptr_cast(lh_ui_web_host_tap_t, context);
    if (lh_math_is_zero(size))
    {
        return 0;
    }
    const lh_ssize_t got = lh_io_reader_read(lh_ui_web_host_tap_get_source_mut(self), out, size);
    lh_ui_web_host_tap_set_last(self, got);
    return got;
}

lh_bool_t
lh_ui_web_host_tap_is_at_end(const lh_ui_web_host_tap_t *self)
{
    return lh_math_is_zero(lh_ui_web_host_tap_get_last(self));
}

lh_bool_t
lh_ui_web_host_tap_is_pending(const lh_ui_web_host_tap_t *self)
{
    return lh_math_eq(lh_ui_web_host_tap_get_last(self), LH_IO_PENDING);
}

lh_bool_t
lh_ui_web_host_tap_is_failed(const lh_ui_web_host_tap_t *self)
{
    return lh_math_lt(lh_ui_web_host_tap_get_last(self), 0) && !lh_ui_web_host_tap_is_pending(self);
}
