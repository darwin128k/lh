#include <lh/os/alloc.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/runtime/allocator.h>

lh_ptr
lh_os_alloc(lh_usize_t size)
{
    lh_ptr block;

    block = lh_runtime_allocator_alloc(size);
    if (lh_null_eq(block))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_out_of_memory, lh_os_error_desc_lit("out of memory")));
    }
    return block;
}
