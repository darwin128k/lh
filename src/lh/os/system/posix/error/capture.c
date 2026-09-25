/*
 * Force the XSI signature of strerror_r (int-returning, always writes into
 * our buffer) before any system header can pull in <string.h> and lock in
 * glibc's default feature-test macros. Without this, a project build that
 * enables GNU extensions (CMAKE_C_EXTENSIONS, on by default) sees glibc's
 * incompatible GNU variant instead (char*-returning; may ignore our buffer
 * for a known error and hand back a static string).
 */
#if !defined(_POSIX_C_SOURCE)
#    define _POSIX_C_SOURCE 200112L
#endif

#include <lh/os/system/error/capture.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/util/str/ptr.h>

#include <errno.h>
#include <string.h>

#if LH_LIBRARY_OPTION_OS_WERROR
#    error "lh/os/system/posix/error/capture.c: LH_LIBRARY_OPTION_OS_WERROR is Windows-only for now"
#endif

lh_os_system_error_code_t
lh_os_system_error_get_native_code(void)
{
    return lh_cast_static(lh_os_system_error_code_t, errno);
}

lh_os_error_desc_ptr
lh_os_system_error_format(lh_os_system_error_code_t code, lh_os_error_desc_ptr dest, lh_usize_t dest_size)
{
    lh_assert_runtime_ref(dest);
    if (strerror_r(lh_cast_static(int, code), dest, dest_size) != 0)
    {
        return lh_str_ptr_clear(dest);
    }
    return lh_str_ptr_rtrim(dest);
}
