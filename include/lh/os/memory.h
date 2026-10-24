/**
 * @file memory.h
 * @brief Page protection: ::lh_os_memory_protect.
 *
 * Changes what may be done with the pages that already back a range of this
 * process — typically making code writable to patch it, then executable again.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_MEMORY_H
#define LH_OS_MEMORY_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/memory/access.h>
#include <lh/ptr.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/memory.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Give the pages covering [@p address, @p address + @p size) the
 *        @p access flags.
 *
 * Whole pages change: the range is widened to page boundaries.
 *
 * @param address Start of the range; must not be ::lh_null.
 * @param size    Bytes in the range; 0 is an error.
 * @param access  `lh_os_memory_access_*` flags.
 * @return ::lh_bool_true on success; ::lh_bool_false with the OS error set.
 */
lh_bool_t
lh_os_memory_protect(lh_ptr address, lh_usize_t size, lh_os_memory_access_t access);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MEMORY_H */
