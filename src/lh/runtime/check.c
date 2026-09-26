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
#    include <lh/str/view.h>
#    include <lh/util/type.h>

#    include <stdio.h>

LH_ATTRIBUTE_STATIC
void
lh_runtime_check_fail_default(const lh_exception_t *exception)
{
    const lh_runtime_error_t *error = lh_exception_get_error(exception);
    const lh_runtime_check_site_t *site = lh_exception_get_site(exception);
    const lh_str_view_t desc = lh_runtime_error_get_desc(error);

    (void)fprintf(stderr, "lh: runtime check failed: error %d", lh_runtime_error_get_code(error));
    if (!lh_str_view_is_empty(&desc))
    {
        (void)fprintf(stderr, ", %.*s", lh_type_cast(int, lh_str_view_get_size(&desc)),
                      lh_str_view_get_data(&desc));
    }
    (void)fputc('\n', stderr);
    if (lh_ptr_is_set(site) && lh_ptr_is_set(site->condition))
    {
        (void)fprintf(stderr, "  condition: %s\n", site->condition);
    }
    if (lh_ptr_is_set(site) && lh_ptr_is_set(site->file))
    {
        (void)fprintf(stderr, "  at %s:%u%s%s\n", site->file, site->line,
                      lh_ptr_is_set(site->function) ? " in " : "",
                      lh_ptr_is_set(site->function) ? site->function : "");
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
lh_runtime_check_fail(const lh_runtime_check_site_t *site, lh_runtime_error_code_t code)
{
    const lh_exception_t exception = lh_exception_initializer(
        lh_runtime_error_initializer(code,
                                     lh_ptr_is_set(site) ? site->message : lh_str_view_empty()),
        site);

    m_runtime_check_fail(lh_addr_of(exception));
    lh_runtime_terminate();
}
