#include <lh/runtime/check.h>
#include <lh/attribute/static.h>
#include <lh/attribute/thread_local.h>
#include <lh/null.h>
#include <lh/runtime/terminate.h>
#include <lh/util/ptr.h>

#if (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include <stdio.h>

LH_ATTRIBUTE_STATIC
void
lh_runtime_check_fail_default(const lh_runtime_check_site_t *site)
{
    if (lh_ptr_is_null(site))
    {
        (void)fputs("lh: runtime check failed\n", stderr);
        return;
    }
    (void)fprintf(stderr, "lh: runtime check failed: %s\n  at %s:%u%s%s\n",
                  lh_ptr_is_set(site->condition) ? site->condition : "(condition not recorded)",
                  lh_ptr_is_set(site->file) ? site->file : "?", site->line,
                  lh_ptr_is_set(site->function) ? " in " : "",
                  lh_ptr_is_set(site->function) ? site->function : "");
    (void)fflush(stderr);
}
#else
LH_ATTRIBUTE_STATIC
void
lh_runtime_check_fail_default(const lh_runtime_check_site_t *site)
{
    (void)site; /* no C library to report through: the host installs its own */
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
lh_runtime_check_fail(const lh_runtime_check_site_t *site)
{
    m_runtime_check_fail(site);
    lh_runtime_terminate();
}
