#include <lh/os.h>
#include <lh/attribute/thread_local.h>
#include <lh/os/error/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

LH_ATTRIBUTE_THREAD_LOCAL
lh_os_error_t m_os_last_error = lh_os_error_empty_initializer();

lh_os_error_t *
lh_os_last_error(void)
{
    return lh_addr_of(m_os_last_error);
}

void
lh_os_set_last_error(lh_os_error_t error)
{
    lh_os_error_assign(lh_os_last_error(), lh_addr_of(error));
}

lh_os_error_t
lh_os_get_last_error(void)
{
    return lh_ptr_deref(lh_os_last_error());
}

lh_error_code_t
lh_os_get_last_error_code(void)
{
    return lh_os_error_get_code(lh_os_last_error());
}

lh_os_error_desc_t
lh_os_get_last_error_desc(void)
{
    return lh_os_error_get_desc(lh_os_last_error());
}
