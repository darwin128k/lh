#include <lh/core/runtime/check.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/runtime/check/handler/initializer.h>
#include <lh/core/char/map.h>
#include <lh/core/exception/initializer.h>
#include <lh/core/runtime/error/initializer.h>
#include <lh/core/runtime/terminate.h>
#include <lh/core/math.h>
#include <lh/core/runtime/logger.h>

/* Room for a report with a long source path; a longer one falls back to the
   code alone rather than being cut. */
#define LH_RUNTIME_CHECK_REPORT_TEXT_MAX 1024U

lh_void
lh_runtime_check_fail_default(lh_self_ptr self, const lh_exception_t *exception)
{
    LH_ATTRIBUTE_UNUSED(self);
    lh_logger_t *logger = lh_runtime_logger_get_mut();
    lh_char_t text[LH_RUNTIME_CHECK_REPORT_TEXT_MAX];
    const lh_usize_t n =
        lh_exception_format(exception, text, LH_RUNTIME_CHECK_REPORT_TEXT_MAX - 1U);
    if (lh_math_is_zero(n))
    {
        LH_ATTRIBUTE_UNUSED(lh_logger_critical(logger, "lh: runtime check failed: error %d",
                                               lh_exception_get_code(exception)));
        return;
    }
    /* The report ends its last line; the logger ends the record itself. */
    text[lh_math_sub_one(n)] = lh_char_map_nul;
    LH_ATTRIBUTE_UNUSED(lh_logger_critical(logger, "%s", text));
}

/* One object for the whole program, empty until the host fills it: the core
   has no C library and no defaults. The host fills it before the first use
   and before it starts other threads, so every thread sees the same one. */
lh_runtime_check_handler_t m_runtime_check =
    lh_runtime_check_handler_initializer(lh_runtime_check_fail_default, lh_null);

lh_runtime_check_handler_t *
lh_runtime_check_get_mut(void)
{
    return lh_addr_of(m_runtime_check);
}

const lh_runtime_check_handler_t *
lh_runtime_check_get(void)
{
    return lh_addr_of(m_runtime_check);
}

lh_void
lh_runtime_check_fail(const lh_exception_origin_t *origin, lh_runtime_error_code_t code)
{
    const lh_exception_t exception = lh_exception_initializer(
        lh_runtime_error_initializer(
            code, lh_ptr_is_set(origin) ? lh_exception_origin_get_message(origin) : lh_null),
        origin);

    lh_runtime_check_handler_call(lh_runtime_check_get(), lh_addr_of(exception));
    lh_runtime_terminate();
}
