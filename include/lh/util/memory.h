/**
 * @file memory.h
 * @brief Memory-span helpers for sized search and sentinel scan.
 *
 * Remaining length (::lh_memory_size_rest) and the address-space bound
 * (::lh_memory_scan_bound) are macros — they are expressions.
 * Byte predicates (::lh_memory_bytes_eq, ::lh_memory_bytes_any_eq,
 * ::lh_memory_bytes_any_ne) are loops, so they are
 * ::LH_ATTRIBUTE_FORCE_INLINE in this header rather than a `#define`.
 * Operation entry points stay in ::lh/memory.h.
 */

#ifndef LH_UTIL_MEMORY_H
#define LH_UTIL_MEMORY_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
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

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True if the first @p n bytes of @p lhs and @p rhs are equal.
 *
 * Zero @p n is equal. Used by the unit-grid path of ::lh_memory_find_step
 * (wide NUL / ::lh_wchar_t) so a two-byte needle does not go through
 * ::lh_memory_compare at every step.
 *
 * @param lhs First buffer.
 * @param rhs Second buffer.
 * @param n   Number of bytes to compare.
 *
 * @return ::lh_bool_true if all compared bytes match, ::lh_bool_false otherwise.
 *
 * @see lh_memory_find_step
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_bytes_eq(const lh_uchar_t *lhs, const lh_uchar_t *rhs, lh_usize_t n)
{
    lh_usize_t i;
    for (i = 0; i < n; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

/**
 * @brief Branchless: true if any of the first @p n bytes of @p lhs equals @p val.
 *
 * No early exit, so the compiler can vectorize the block walk in
 * ::lh_memory_find_step. Zero @p n is not a hit.
 *
 * @param lhs Buffer to scan.
 * @param n   Number of bytes.
 * @param val Byte to look for.
 *
 * @return ::lh_bool_true if @p val occurs, ::lh_bool_false otherwise.
 *
 * @see lh_memory_bytes_eq
 * @see lh_memory_find_step
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_bytes_any_eq(const lh_uchar_t *lhs, lh_usize_t n, lh_uchar_t val)
{
    lh_bool_t hit = lh_bool_false;
    lh_usize_t i;
    for (i = 0; i < n; ++i)
    {
        hit = lh_cast_static(lh_bool_t, (hit | (lhs[i] == val)));
    }
    return hit;
}

/**
 * @brief Branchless: true if any @p rhs_size-byte unit of @p lhs matches @p rhs
 *        on a @p step grid.
 *
 * Used by the wchar / unit path of ::lh_memory_find_step over a scan block.
 * Callers must pass a span where each candidate @c i, @c i+step, … has
 * @p rhs_size bytes in range (block size a multiple of @p step, @p rhs_size
 * equal to @p step).
 *
 * @param lhs       Haystack.
 * @param n         Bytes available at @p lhs.
 * @param rhs       Needle.
 * @param rhs_size  Needle size in bytes.
 * @param step      Byte stride between candidates.
 *
 * @return ::lh_bool_true if some unit matches, ::lh_bool_false otherwise.
 *
 * @see lh_memory_bytes_eq
 * @see lh_memory_bytes_any_eq
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_bytes_any_eq_step(const lh_uchar_t *lhs, lh_usize_t n, const lh_uchar_t *rhs,
                            lh_usize_t rhs_size, lh_usize_t step)
{
    lh_bool_t hit = lh_bool_false;
    lh_usize_t i;
    for (i = 0; i < n; i += step)
    {
        hit = lh_cast_static(lh_bool_t, (hit | lh_memory_bytes_eq(lhs + i, rhs, rhs_size)));
    }
    return hit;
}

/**
 * @brief Branchless: true if any of the first @p n bytes of @p lhs and @p rhs differ.
 *
 * No early exit — same shape as the block walk in ::lh_memory_std_compare.
 * Zero @p n is not a difference.
 *
 * @param lhs First buffer.
 * @param rhs Second buffer.
 * @param n   Number of bytes to compare.
 *
 * @return ::lh_bool_true if a mismatch exists, ::lh_bool_false if all equal.
 *
 * @see lh_memory_bytes_eq
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_bytes_any_ne(const lh_uchar_t *lhs, const lh_uchar_t *rhs, lh_usize_t n)
{
    lh_bool_t hit = lh_bool_false;
    lh_usize_t i;
    for (i = 0; i < n; ++i)
    {
        hit = lh_cast_static(lh_bool_t, (hit | (lhs[i] != rhs[i])));
    }
    return hit;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_MEMORY_H */
