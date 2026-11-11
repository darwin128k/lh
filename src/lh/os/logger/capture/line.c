/**
 * @file line.c
 * @brief ::lh_os_logger_capture_line_t: its fields.
 */

#include <lh/os/logger/capture/line.h>
#include <lh/core/assert/runtime.h>

lh_cptr_of(lh_void) lh_os_logger_capture_line_get_target(const lh_os_logger_capture_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->target;
}

lh_void
lh_os_logger_capture_line_set_target(lh_os_logger_capture_line_t *self, lh_cptr_of(lh_void) target)
{
    lh_assert_runtime_ref(self);
    self->target = target;
}

lh_ptr
lh_os_logger_capture_line_get_context(const lh_os_logger_capture_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_os_logger_capture_line_set_context(lh_os_logger_capture_line_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

const lh_os_logger_record_t *
lh_os_logger_capture_line_get_record(const lh_os_logger_capture_line_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->record);
}

lh_os_logger_record_t *
lh_os_logger_capture_line_get_record_mut(lh_os_logger_capture_line_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->record);
}
