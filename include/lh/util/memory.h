/**
 * @file memory.h
 * @brief Memory-span helpers for sized search and sentinel scan.
 *
 * Remaining length after an offset (::lh_memory_size_rest) and the
 * address-space bound for an unbounded scan (::lh_memory_scan_bound).
 * Operation entry points stay in ::lh/memory.h.
 */

#ifndef LH_UTIL_MEMORY_H
#define LH_UTIL_MEMORY_H

#include <lh/size.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

/**
 * @def lh_memory_size_rest(size, off)
 * @brief Bytes remaining in a span of @p size after offset @p off.
 *
 * Returns @c size - off when @p off is in range (@c off <= size), otherwise 0.
 * Used by ::lh_memory_find_step so a huge scan bound cannot wrap an end pointer.
 *
 * @param size Span length in bytes.
 * @param off  Byte offset from the start of the span.
 *
 * @warning Arguments may be evaluated more than once.
 *
 * @see lh_memory_scan_bound
 * @see lh_memory_find_step
 */
#define lh_memory_size_rest(size, off)                                                             \
    (lh_math_le((off), (size)) ? lh_math_sub((size), (off)) : 0)

/**
 * @def lh_memory_scan_bound(ptr)
 * @brief Remaining address space from @p ptr, in bytes.
 *
 * Expands to <code>::LH_USIZE_T_MAX - ::lh_ptr_to_uaddr(ptr)</code>.
 * This is the haystack length used by ::lh_memory_scan / ::lh_memory_scan_step.
 *
 * @param ptr Start of the scan.
 *
 * @warning @p ptr may be evaluated more than once.
 *
 * @see lh_memory_scan
 * @see lh_memory_size_rest
 */
#define lh_memory_scan_bound(ptr) lh_addr_diff(LH_USIZE_T_MAX, lh_ptr_to_uaddr(ptr))

#endif /* LH_UTIL_MEMORY_H */
