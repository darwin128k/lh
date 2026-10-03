#include <lh/os/system/error/kind.h>
#include <lh/cast/static.h>
#include <lh/math.h>

#include <errno.h>

lh_bool_t
lh_os_system_error_code_is_not_found(lh_os_system_error_code_t code)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(code, lh_cast_static(lh_os_system_error_code_t, ENOENT)));
}

lh_bool_t
lh_os_system_error_code_is_not_dir(lh_os_system_error_code_t code)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(code, lh_cast_static(lh_os_system_error_code_t, ENOTDIR)));
}
