#include <lh/os/alloc.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/runtime/allocator.h>

lh_ptr
lh_os_alloc(lh_usize_t size)
{
    lh_ptr const block = lh_runtime_allocator_alloc(size);

    if (lh_null_eq(block))
    {
        lh_os_set_last_error_lit(lh_os_error_code_out_of_memory, "out of memory");
    }
    return block;
}
