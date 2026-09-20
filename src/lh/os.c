#include <lh/os.h>
#include <lh/attribute/thread_local.h>
#include <lh/util/addr.h>

static LH_ATTRIBUTE_THREAD_LOCAL lh_os_error_t g_lh_os_last_error;

void
lh_os_set_last_error(lh_os_error_t error)
{
    g_lh_os_last_error = error;
}

lh_os_error_t
lh_os_get_last_error(void)
{
    return g_lh_os_last_error;
}

lh_error_code_t
lh_os_get_last_error_code(void)
{
    return lh_os_error_get_code(lh_addr_of(g_lh_os_last_error));
}

lh_os_error_desc_t
lh_os_get_last_error_desc(void)
{
    return lh_os_error_get_desc(lh_addr_of(g_lh_os_last_error));
}
