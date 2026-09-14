#include <lh/logger.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/optional/ref.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/ptr.h>
#include <lh/void.h>

void
lh_logger_pack(lh_logger_t *self, const lh_logger_level_flags_t *flags,
               lh_logger_emit_cb *emergency_cb, lh_logger_emit_cb *alert_cb,
               lh_logger_emit_cb *critical_cb, lh_logger_emit_cb *error_cb,
               lh_logger_emit_cb *warning_cb, lh_logger_emit_cb *notice_cb,
               lh_logger_emit_cb *info_cb, lh_logger_emit_cb *debug_cb, lh_ptr *context)
{
    lh_assert_runtime_ref(self);
    lh_optional_ref(flags)
    {
        self->flags = lh_ptr_deref(flags);
    }
    lh_optional_ref(emergency_cb)
    {
        self->emergency_cb = lh_ptr_deref(emergency_cb);
    }
    lh_optional_ref(alert_cb)
    {
        self->alert_cb = lh_ptr_deref(alert_cb);
    }
    lh_optional_ref(critical_cb)
    {
        self->critical_cb = lh_ptr_deref(critical_cb);
    }
    lh_optional_ref(error_cb)
    {
        self->error_cb = lh_ptr_deref(error_cb);
    }
    lh_optional_ref(warning_cb)
    {
        self->warning_cb = lh_ptr_deref(warning_cb);
    }
    lh_optional_ref(notice_cb)
    {
        self->notice_cb = lh_ptr_deref(notice_cb);
    }
    lh_optional_ref(info_cb)
    {
        self->info_cb = lh_ptr_deref(info_cb);
    }
    lh_optional_ref(debug_cb)
    {
        self->debug_cb = lh_ptr_deref(debug_cb);
    }
    lh_optional_ref(context)
    {
        self->context = lh_ptr_deref(context);
    }
}

void
lh_logger_unpack(const lh_logger_t *self, lh_logger_level_flags_t *flags,
                 lh_logger_emit_cb *emergency_cb, lh_logger_emit_cb *alert_cb,
                 lh_logger_emit_cb *critical_cb, lh_logger_emit_cb *error_cb,
                 lh_logger_emit_cb *warning_cb, lh_logger_emit_cb *notice_cb,
                 lh_logger_emit_cb *info_cb, lh_logger_emit_cb *debug_cb, lh_ptr *context)
{
    lh_assert_runtime_ref(self);
    lh_optional_ref(flags)
    {
        lh_ptr_deref(flags) = self->flags;
    }
    lh_optional_ref(emergency_cb)
    {
        lh_ptr_deref(emergency_cb) = self->emergency_cb;
    }
    lh_optional_ref(alert_cb)
    {
        lh_ptr_deref(alert_cb) = self->alert_cb;
    }
    lh_optional_ref(critical_cb)
    {
        lh_ptr_deref(critical_cb) = self->critical_cb;
    }
    lh_optional_ref(error_cb)
    {
        lh_ptr_deref(error_cb) = self->error_cb;
    }
    lh_optional_ref(warning_cb)
    {
        lh_ptr_deref(warning_cb) = self->warning_cb;
    }
    lh_optional_ref(notice_cb)
    {
        lh_ptr_deref(notice_cb) = self->notice_cb;
    }
    lh_optional_ref(info_cb)
    {
        lh_ptr_deref(info_cb) = self->info_cb;
    }
    lh_optional_ref(debug_cb)
    {
        lh_ptr_deref(debug_cb) = self->debug_cb;
    }
    lh_optional_ref(context)
    {
        lh_ptr_deref(context) = self->context;
    }
}

void
lh_logger_set(lh_logger_t *self, lh_logger_level_flags_t flags, lh_logger_emit_cb emergency_cb,
              lh_logger_emit_cb alert_cb, lh_logger_emit_cb critical_cb,
              lh_logger_emit_cb error_cb, lh_logger_emit_cb warning_cb, lh_logger_emit_cb notice_cb,
              lh_logger_emit_cb info_cb, lh_logger_emit_cb debug_cb, lh_ptr context)
{
    lh_logger_pack(self, lh_addr_of(flags), lh_addr_of(emergency_cb), lh_addr_of(alert_cb),
                   lh_addr_of(critical_cb), lh_addr_of(error_cb), lh_addr_of(warning_cb),
                   lh_addr_of(notice_cb), lh_addr_of(info_cb), lh_addr_of(debug_cb),
                   lh_addr_of(context));
}

void
lh_logger_assign(lh_logger_t *self, const lh_logger_t *other)
{
    lh_logger_level_flags_t flags;
    lh_logger_emit_cb emergency_cb;
    lh_logger_emit_cb alert_cb;
    lh_logger_emit_cb critical_cb;
    lh_logger_emit_cb error_cb;
    lh_logger_emit_cb warning_cb;
    lh_logger_emit_cb notice_cb;
    lh_logger_emit_cb info_cb;
    lh_logger_emit_cb debug_cb;
    lh_ptr context;

    lh_logger_unpack(other, lh_addr_of(flags), lh_addr_of(emergency_cb), lh_addr_of(alert_cb),
                     lh_addr_of(critical_cb), lh_addr_of(error_cb), lh_addr_of(warning_cb),
                     lh_addr_of(notice_cb), lh_addr_of(info_cb), lh_addr_of(debug_cb),
                     lh_addr_of(context));
    lh_logger_set(self, flags, emergency_cb, alert_cb, critical_cb, error_cb, warning_cb, notice_cb,
                  info_cb, debug_cb, context);
}

void
lh_logger_init(lh_logger_t *self, lh_logger_level_flags_t flags, lh_logger_emit_cb emit_cb,
               lh_ptr context)
{
    lh_logger_set(self, flags, emit_cb, emit_cb, emit_cb, emit_cb, emit_cb, emit_cb, emit_cb,
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
    lh_logger_level_flags_t flags;
    lh_logger_unpack(self, lh_addr_of(flags), lh_null, lh_null, lh_null, lh_null, lh_null, lh_null,
                     lh_null, lh_null, lh_null);
    return flags;
}

lh_logger_emit_cb
lh_logger_get_emit_cb(const lh_logger_t *self, lh_logger_level_t level)
{
    lh_logger_emit_cb emergency_cb;
    lh_logger_emit_cb alert_cb;
    lh_logger_emit_cb critical_cb;
    lh_logger_emit_cb error_cb;
    lh_logger_emit_cb warning_cb;
    lh_logger_emit_cb notice_cb;
    lh_logger_emit_cb info_cb;
    lh_logger_emit_cb debug_cb;

    lh_assert_runtime_if(level > lh_logger_level_debug,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    lh_logger_unpack(self, lh_null, lh_addr_of(emergency_cb), lh_addr_of(alert_cb),
                     lh_addr_of(critical_cb), lh_addr_of(error_cb), lh_addr_of(warning_cb),
                     lh_addr_of(notice_cb), lh_addr_of(info_cb), lh_addr_of(debug_cb), lh_null);

    switch (level)
    {
    case lh_logger_level_emergency:
        return emergency_cb;
    case lh_logger_level_alert:
        return alert_cb;
    case lh_logger_level_critical:
        return critical_cb;
    case lh_logger_level_error:
        return error_cb;
    case lh_logger_level_warning:
        return warning_cb;
    case lh_logger_level_notice:
        return notice_cb;
    case lh_logger_level_info:
        return info_cb;
    case lh_logger_level_debug:
        return debug_cb;
    default:
        return lh_null;
    }
}

lh_ptr
lh_logger_get_context(const lh_logger_t *self)
{
    lh_ptr context;
    lh_logger_unpack(self, lh_null, lh_null, lh_null, lh_null, lh_null, lh_null, lh_null, lh_null,
                     lh_null, lh_addr_of(context));
    return context;
}

lh_ssize_t
lh_logger_log_v(lh_logger_t *self, lh_logger_level_t level, lh_str_cptr fmt, va_list args)
{
    lh_logger_emit_cb emit_cb;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(fmt);
    lh_assert_runtime_if(level > lh_logger_level_debug,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

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
    lh_ssize_t n;

    va_start(args, fmt);
    n = lh_logger_log_v(self, level, fmt, args);
    va_end(args);
    return n;
}

#define LH_LOGGER_DEFINE_LEVEL_FN(fn, level)                                                       \
    lh_ssize_t fn(lh_logger_t *self, lh_str_cptr fmt, ...)                                         \
    {                                                                                              \
        va_list args;                                                                              \
        lh_ssize_t n;                                                                              \
        va_start(args, fmt);                                                                       \
        n = lh_logger_log_v(self, level, fmt, args);                                               \
        va_end(args);                                                                              \
        return n;                                                                                  \
    }

LH_LOGGER_DEFINE_LEVEL_FN(lh_logger_emergency, lh_logger_level_emergency)
LH_LOGGER_DEFINE_LEVEL_FN(lh_logger_alert, lh_logger_level_alert)
LH_LOGGER_DEFINE_LEVEL_FN(lh_logger_critical, lh_logger_level_critical)
LH_LOGGER_DEFINE_LEVEL_FN(lh_logger_error, lh_logger_level_error)
LH_LOGGER_DEFINE_LEVEL_FN(lh_logger_warning, lh_logger_level_warning)
LH_LOGGER_DEFINE_LEVEL_FN(lh_logger_notice, lh_logger_level_notice)
LH_LOGGER_DEFINE_LEVEL_FN(lh_logger_info, lh_logger_level_info)
LH_LOGGER_DEFINE_LEVEL_FN(lh_logger_debug, lh_logger_level_debug)
