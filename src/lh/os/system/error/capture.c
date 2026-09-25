#include <lh/os/system/error/capture.h>
#include <lh/attribute/thread_local.h>
#include <lh/os/system.h>

#if LH_LIBRARY_OPTION_OS_WERROR
LH_ATTRIBUTE_THREAD_LOCAL
lh_wchar_t m_os_system_error_buffer[LH_OS_SYSTEM_ERROR_BUFFER_SIZE];
#else
LH_ATTRIBUTE_THREAD_LOCAL
lh_char_t m_os_system_error_buffer[LH_OS_SYSTEM_ERROR_BUFFER_SIZE];
#endif

void
lh_os_system_error_capture(void)
{
    lh_os_system_error_code_t code;
    lh_os_error_desc_t desc;

    code = lh_os_system_error_get_native_code();
    (void)lh_os_system_error_format(code, m_os_system_error_buffer, LH_OS_SYSTEM_ERROR_BUFFER_SIZE);
#if LH_LIBRARY_OPTION_OS_WERROR
    desc = lh_wstr_view_make(m_os_system_error_buffer);
#else
    desc = lh_str_view_make(m_os_system_error_buffer);
#endif
    lh_os_system_set_last_error(lh_os_system_error_make(code, desc));
}
