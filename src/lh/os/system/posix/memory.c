#include <lh/os/system/memory.h>
#include <lh/assert.h>
#include <lh/os/system/error/capture.h>
#include <lh/math.h>

#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

lh_bool_t
lh_os_system_memory_protect(lh_ptr address, lh_usize_t size, lh_os_memory_access_t access)
{
    int prot = PROT_NONE;

    lh_assert_runtime_ref(address);
    /* mprotect takes whole pages from a page-aligned start. */
    const uintptr_t page = (uintptr_t)sysconf(_SC_PAGESIZE);
    const uintptr_t start = (uintptr_t)address & ~(page - 1U);
    const uintptr_t end = (uintptr_t)address + (uintptr_t)size;

    if (access & lh_os_memory_access_read)
    {
        prot |= PROT_READ;
    }
    if (access & lh_os_memory_access_write)
    {
        prot |= PROT_READ | PROT_WRITE;
    }
    if (access & lh_os_memory_access_execute)
    {
        prot |= PROT_EXEC;
    }
    if (lh_math_ne(mprotect((void *)start, (size_t)(end - start), prot), 0))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    return lh_bool_true;
}
