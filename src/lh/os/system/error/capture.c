#include <lh/os/system/error/capture.h>
#include <lh/attribute/thread_local.h>
#include <lh/os/system.h>
#include <lh/util/addr.h>

#if LH_LIBRARY_OPTION_OS_WERROR
LH_ATTRIBUTE_THREAD_LOCAL
lh_wchar_t m_os_system_error_buffer[LH_OS_SYSTEM_ERROR_BUFFER_SIZE];
#else
LH_ATTRIBUTE_THREAD_LOCAL
lh_char_t m_os_system_error_buffer[LH_OS_SYSTEM_ERROR_BUFFER_SIZE];
#endif

lh_void
lh_os_system_error_capture(void)
{
    const lh_os_system_error_code_t code = lh_os_system_error_get_native_code();

    lh_os_system_error_t err;
    lh_os_system_error_init(lh_addr_of(err), code,
                            lh_os_system_error_format(code, m_os_system_error_buffer, LH_OS_SYSTEM_ERROR_BUFFER_SIZE));
    lh_os_system_set_last_error(err);
}
