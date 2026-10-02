/**
 * @file alloc.h
 * @brief Allocation that reports failure the OS-layer way.
 *
 * Every OS-layer object that needs memory fails the same way when it gets
 * none: ::lh_null back, and ::lh_os_error_code_out_of_memory in
 * ::lh_os_last_error — never an assertion. ::lh_os_alloc is that one place.
 *
 * Memory comes from the runtime allocator (`lh/runtime/allocator.h`);
 * release it with ::lh_runtime_allocator_free.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_ALLOC_H
#define LH_OS_ALLOC_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/ptr.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/alloc.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Allocate @p size bytes from the runtime allocator.
 *
 * @param size Bytes to allocate.
 * @return The block, or ::lh_null with ::lh_os_error_code_out_of_memory in
 *         ::lh_os_last_error.
 */
lh_ptr
lh_os_alloc(lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_ALLOC_H */
