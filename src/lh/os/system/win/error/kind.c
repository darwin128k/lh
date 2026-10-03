#include <lh/os/system/error/kind.h>
#include <lh/cast/static.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/math.h>

lh_bool_t
lh_os_system_error_code_is_not_found(lh_os_system_error_code_t code)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(code, LH_OS_SYSTEM_WIN_ERROR_FILE_NOT_FOUND) ||
                                         lh_math_eq(code, LH_OS_SYSTEM_WIN_ERROR_PATH_NOT_FOUND));
}

lh_bool_t
lh_os_system_error_code_is_not_dir(lh_os_system_error_code_t code)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(code, LH_OS_SYSTEM_WIN_ERROR_DIRECTORY));
}
