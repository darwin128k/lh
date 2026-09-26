#include <lh/runtime/check.h>
#include <lh/attribute/static.h>
#include <lh/attribute/thread_local.h>
#include <lh/exception/initializer.h>
#include <lh/null.h>
#include <lh/runtime/error/initializer.h>
#include <lh/runtime/terminate.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#if (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include <lh/util/math.h>

#    include <stdio.h>

/* Room for a report with a long source path; a longer one falls back to the
   code alone rather than being cut. */
#    define LH_RUNTIME_CHECK_REPORT_TEXT_MAX 1024U

LH_ATTRIBUTE_STATIC
void
lh_runtime_check_fail_default(const lh_exception_t *exception)
{
    lh_char_t text[LH_RUNTIME_CHECK_REPORT_TEXT_MAX];
    const lh_usize_t n =
        lh_exception_format(exception, text, LH_RUNTIME_CHECK_REPORT_TEXT_MAX - 1U);

    if (lh_math_is_zero(n))
    {
        (void)fprintf(stderr, "lh: runtime check failed: error %d\n",
                      lh_exception_get_code(exception));
    }
    else
    {
        text[n] = '\0';
        (void)fputs(text, stderr);
    }
    (void)fflush(stderr);
}
#else
LH_ATTRIBUTE_STATIC
void
lh_runtime_check_fail_default(const lh_exception_t *exception)
{
    (void)exception; /* no C library to report through: the host installs its own */
}
#endif

LH_ATTRIBUTE_THREAD_LOCAL
lh_runtime_check_fail_cb m_runtime_check_fail = lh_runtime_check_fail_default;

lh_runtime_check_fail_cb
lh_runtime_check_get(void)
{
    return m_runtime_check_fail;
}

void
lh_runtime_check_set(lh_runtime_check_fail_cb fn)
{
    m_runtime_check_fail = lh_ptr_is_set(fn) ? fn : lh_runtime_check_fail_default;
}

void
lh_runtime_check_fail(const lh_exception_origin_t *origin, lh_runtime_error_code_t code)
{
    const lh_exception_t exception = lh_exception_initializer(
        lh_runtime_error_initializer(code,
                                     lh_ptr_is_set(origin) ? origin->message : lh_str_view_empty()),
        origin);

    m_runtime_check_fail(lh_addr_of(exception));
    lh_runtime_terminate();
}
