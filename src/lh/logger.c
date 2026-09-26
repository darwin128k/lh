#include <lh/logger.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/runtime/error.h>
#include <lh/util/bit.h>
#include <lh/util/ptr.h>
#include <lh/void.h>

void
lh_logger_set(lh_logger_t *self, lh_logger_level_flags_t flags, lh_logger_emit_cb emergency_cb,
              lh_logger_emit_cb alert_cb, lh_logger_emit_cb critical_cb, lh_logger_emit_cb error_cb,
              lh_logger_emit_cb warning_cb, lh_logger_emit_cb notice_cb, lh_logger_emit_cb info_cb,
              lh_logger_emit_cb debug_cb, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->flags = flags;
    self->emergency_cb = emergency_cb;
    self->alert_cb = alert_cb;
    self->critical_cb = critical_cb;
    self->error_cb = error_cb;
    self->warning_cb = warning_cb;
    self->notice_cb = notice_cb;
    self->info_cb = info_cb;
    self->debug_cb = debug_cb;
    self->context = context;
}

void
lh_logger_assign(lh_logger_t *self, const lh_logger_t *other)
{
    lh_logger_set(self, lh_logger_get_flags(other),
                  lh_logger_get_emit_cb(other, lh_logger_level_emergency),
                  lh_logger_get_emit_cb(other, lh_logger_level_alert),
                  lh_logger_get_emit_cb(other, lh_logger_level_critical),
                  lh_logger_get_emit_cb(other, lh_logger_level_error),
                  lh_logger_get_emit_cb(other, lh_logger_level_warning),
                  lh_logger_get_emit_cb(other, lh_logger_level_notice),
                  lh_logger_get_emit_cb(other, lh_logger_level_info),
                  lh_logger_get_emit_cb(other, lh_logger_level_debug),
                  lh_logger_get_context(other));
}

void
lh_logger_init_of(lh_logger_t *self, lh_logger_level_flags_t flags, lh_logger_emit_cb emergency_cb,
                  lh_logger_emit_cb alert_cb, lh_logger_emit_cb critical_cb,
                  lh_logger_emit_cb error_cb, lh_logger_emit_cb warning_cb,
                  lh_logger_emit_cb notice_cb, lh_logger_emit_cb info_cb,
                  lh_logger_emit_cb debug_cb, lh_ptr context)
{
    lh_logger_set(self, flags, emergency_cb, alert_cb, critical_cb, error_cb, warning_cb, notice_cb,
                  info_cb, debug_cb, context);
}

void
lh_logger_init(lh_logger_t *self, lh_logger_level_flags_t flags, lh_logger_emit_cb emit_cb,
               lh_ptr context)
{
    lh_logger_init_of(self, flags, emit_cb, emit_cb, emit_cb, emit_cb, emit_cb, emit_cb, emit_cb,
                      emit_cb, context);
}

void
lh_logger_deinit(lh_logger_t *self)
{
    lh_logger_set(self, lh_logger_level_flags_none, lh_null, lh_null, lh_null, lh_null, lh_null,
                  lh_null, lh_null, lh_null, lh_null);
}

lh_logger_level_flags_t
lh_logger_get_flags(const lh_logger_t *self)
{
    lh_assert_runtime_ref(self);
    return self->flags;
}

lh_logger_emit_cb
lh_logger_get_emit_cb(const lh_logger_t *self, lh_logger_level_t level)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(level > lh_logger_level_debug, lh_runtime_error_code_invalid_argument);

    switch (level)
    {
    case lh_logger_level_emergency:
        return self->emergency_cb;
    case lh_logger_level_alert:
        return self->alert_cb;
    case lh_logger_level_critical:
        return self->critical_cb;
    case lh_logger_level_error:
        return self->error_cb;
    case lh_logger_level_warning:
        return self->warning_cb;
    case lh_logger_level_notice:
        return self->notice_cb;
    case lh_logger_level_info:
        return self->info_cb;
    case lh_logger_level_debug:
        return self->debug_cb;
    default:
        return lh_null;
    }
}

lh_ptr
lh_logger_get_context(const lh_logger_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_ssize_t
lh_logger_log_v(lh_logger_t *self, lh_logger_level_t level, lh_str_cptr fmt, va_list args)
{
    lh_logger_emit_cb emit_cb;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(fmt);
    lh_assert_runtime_if(level > lh_logger_level_debug, lh_runtime_error_code_invalid_argument);

    if (lh_bit_disjoint(self->flags, lh_bit_mask(level)))
    {
        return 0;
    }

    emit_cb = lh_logger_get_emit_cb(self, level);
    if (lh_null_eq(lh_ptr_rcast(lh_void, emit_cb)))
    {
        return 0;
    }

    return emit_cb(lh_logger_get_context(self), level, fmt, args);
}

lh_ssize_t
lh_logger_log(lh_logger_t *self, lh_logger_level_t level, lh_str_cptr fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    const lh_ssize_t n = lh_logger_log_v(self, level, fmt, args);
    va_end(args);
    return n;
}

/* lh_logger_emergency … lh_logger_debug are macros; see logger.h. */
