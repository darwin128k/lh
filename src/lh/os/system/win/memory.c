#include <lh/os/system/memory.h>
#include <lh/assert.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/math.h>

/* PAGE_* protection constants: Windows takes one value, not flags. */
#define LH_OS_SYSTEM_WIN_PAGE_NOACCESS 0x01UL
#define LH_OS_SYSTEM_WIN_PAGE_READONLY 0x02UL
#define LH_OS_SYSTEM_WIN_PAGE_READWRITE 0x04UL
#define LH_OS_SYSTEM_WIN_PAGE_EXECUTE 0x10UL
#define LH_OS_SYSTEM_WIN_PAGE_EXECUTE_READ 0x20UL
#define LH_OS_SYSTEM_WIN_PAGE_EXECUTE_READWRITE 0x40UL

lh_bool_t
lh_os_system_memory_protect(lh_ptr address, lh_usize_t size, lh_os_memory_access_t access)
{
    lh_os_system_win_dword_t protect;
    lh_os_system_win_dword_t old_protect;

    lh_assert_runtime_ref(address);
    if (access & lh_os_memory_access_execute)
    {
        protect = (access & lh_os_memory_access_write)  ? LH_OS_SYSTEM_WIN_PAGE_EXECUTE_READWRITE
                  : (access & lh_os_memory_access_read) ? LH_OS_SYSTEM_WIN_PAGE_EXECUTE_READ
                                                        : LH_OS_SYSTEM_WIN_PAGE_EXECUTE;
    }
    else
    {
        protect = (access & lh_os_memory_access_write)  ? LH_OS_SYSTEM_WIN_PAGE_READWRITE
                  : (access & lh_os_memory_access_read) ? LH_OS_SYSTEM_WIN_PAGE_READONLY
                                                        : LH_OS_SYSTEM_WIN_PAGE_NOACCESS;
    }
    if (lh_math_is_zero(VirtualProtect(address, size, protect, lh_addr_of(old_protect))))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    return lh_bool_true;
}
