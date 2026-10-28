#include <lh/os/memory.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/memory.h>
#include <lh/math.h>

lh_bool_t
lh_os_memory_protect(lh_ptr address, lh_usize_t size, lh_os_memory_access_t access)
{
    if (lh_null_eq(address) || lh_math_is_zero(size))
    {
        lh_os_set_last_error_lit(lh_os_error_code_empty_range, "empty memory range");
        return lh_bool_false;
    }
    return lh_os_system_memory_protect(address, size, access);
}
