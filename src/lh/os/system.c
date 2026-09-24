#include <lh/os/system.h>
#include <lh/attribute/thread_local.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

/* No explicit initializer: static zero-init already matches
 * lh_os_system_error_empty_initializer() (code 0 == lh_os_system_error_code_ok,
 * desc a null view). Kept implicit (rather than relying on
 * lh_os_system_error_code_ok being a literal) to match lh/os.c's TLS
 * initializer style - see the comment there. */
LH_ATTRIBUTE_THREAD_LOCAL
lh_os_system_error_t m_os_system_last_error;

lh_os_system_error_t *
lh_os_system_last_error(void)
{
    return lh_addr_of(m_os_system_last_error);
}

void
lh_os_system_set_last_error(lh_os_system_error_t error)
{
    lh_os_system_error_assign(lh_os_system_last_error(), lh_addr_of(error));
}

lh_os_system_error_t
lh_os_system_get_last_error(void)
{
    return lh_ptr_deref(lh_os_system_last_error());
}

lh_os_system_error_code_t
lh_os_system_get_last_error_code(void)
{
    return lh_os_system_error_get_code(lh_os_system_last_error());
}

lh_os_error_desc_t
lh_os_system_get_last_error_desc(void)
{
    return lh_os_system_error_get_desc(lh_os_system_last_error());
}
