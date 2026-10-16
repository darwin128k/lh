#include <lh/os.h>
#include <lh/assert/runtime.h>
#include <lh/attribute/thread_local.h>

/* No explicit initializer: static zero-init already matches
 * lh_os_error_empty_initializer() (code 0 == lh_error_code_ok, desc a null
 * view). lh_error_code_ok itself is not a compile-time constant expression
 * (it reads a field of a static const object), which MSVC's __declspec(thread)
 * rejects as a TLS initializer even though GCC/Clang's __thread tolerates it. */
LH_ATTRIBUTE_THREAD_LOCAL
lh_os_error_t m_os_last_error;

lh_os_error_t *
lh_os_last_error(void)
{
    return lh_addr_of(m_os_last_error);
}

lh_void
lh_os_set_last_error(lh_os_error_t error)
{
    lh_os_error_assign(lh_os_last_error(), lh_addr_of(error));
}

lh_void
lh_os_get_last_error(lh_os_error_t *out)
{
    lh_assert_runtime_ref(out);
    *out = lh_ptr_deref(lh_os_last_error());
    return;
}

lh_error_code_t
lh_os_get_last_error_code(void)
{
    return lh_os_error_get_code(lh_os_last_error());
}

lh_void
lh_os_get_last_error_desc(lh_os_error_desc_t *out)
{
    lh_os_error_get_desc(lh_os_last_error(), out);
    return;
}
