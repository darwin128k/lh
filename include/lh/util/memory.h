/**
 * @file memory.h
 * @brief Memory-span helpers for sized search and sentinel scan.
 *
 * Remaining length (::lh_memory_size_rest) and the address-space bound
 * (::lh_memory_scan_bound) are macros — they are expressions.
 * Byte predicates (::lh_memory_bytes_eq, ::lh_memory_bytes_eq_cstr,
 * ::lh_memory_bytes_find, ::lh_memory_bytes_any_eq, ::lh_memory_bytes_any_ne)
 * are force-inlined here rather than a `#define`. Operation entry points stay
 * in ::lh/memory.h.
 */

#ifndef LH_UTIL_MEMORY_H
#define LH_UTIL_MEMORY_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/size/limits.h>

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
#define lh_memory_size_rest(size, off) (lh_math_le((off), (size)) ? lh_math_sub((size), (off)) : 0)

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
lh_memory_bytes_eq(const lh_byte_ptr lhs, const lh_byte_ptr rhs, lh_usize_t n)
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
 * @brief True if the @p size bytes at @p bytes begin with the @p prefix_size bytes of @p prefix.
 *
 * An empty prefix is a prefix of anything; a prefix longer than @p bytes is not.
 *
 * @param bytes       Buffer to look at.
 * @param size        Bytes in @p bytes.
 * @param prefix      Bytes it should start with.
 * @param prefix_size Bytes in @p prefix.
 *
 * @return ::lh_bool_true if @p bytes starts with @p prefix, ::lh_bool_false otherwise.
 *
 * @see lh_memory_bytes_eq
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_bytes_starts_with(const lh_byte_ptr bytes, lh_usize_t size, const lh_byte_ptr prefix,
                            lh_usize_t prefix_size)
{
    return lh_math_ge(size, prefix_size) ? lh_memory_bytes_eq(bytes, prefix, prefix_size)
                                         : lh_bool_false;
}

/**
 * @brief True if two NUL-terminated byte strings are equal.
 *
 * Byte prologue until @p lhs is word-aligned, then SWAR words when @p rhs is
 * aligned too (same zero-byte mask as ::lh_memory_bytes_any_eq). Aligned word
 * reads stay inside a page. Differing alignments fall back to a byte walk.
 *
 * @param lhs First NUL-terminated string.
 * @param rhs Second NUL-terminated string.
 *
 * @return ::lh_bool_true if both match through a shared NUL, else ::lh_bool_false.
 *
 * @see lh_memory_bytes_eq
 * @see lh_str_ptr_equals
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_bytes_eq_cstr(const lh_byte_ptr lhs, const lh_byte_ptr rhs)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr a = lhs;
    lh_byte_cptr b = rhs;

    while ((lh_ptr_to_uaddr(a) & (w - 1U)) != 0U)
    {
        const lh_byte_t ac = lh_ptr_deref(a);
        const lh_byte_t bc = lh_ptr_deref(b);

        if (ac != bc)
        {
            return lh_bool_false;
        }
        if (ac == 0U)
        {
            return lh_bool_true;
        }
        ++a;
        ++b;
    }

    if ((lh_ptr_to_uaddr(b) & (w - 1U)) == 0U)
    {
        const lh_usize_t ones = (~(lh_usize_t)0) / 0xFFU;
        const lh_usize_t high = ones << 7;

        for (;;)
        {
            const lh_usize_t wa = *lh_ptr_rcast(const lh_usize_t, a);
            const lh_usize_t wb = *lh_ptr_rcast(const lh_usize_t, b);

            if (wa != wb)
            {
                break;
            }
            if (((wa - ones) & ~wa & high) != 0U)
            {
                return lh_bool_true;
            }
            a += w;
            b += w;
        }
    }

    for (;;)
    {
        const lh_byte_t ac = lh_ptr_deref(a);
        const lh_byte_t bc = lh_ptr_deref(b);

        if (ac != bc)
        {
            return lh_bool_false;
        }
        if (ac == 0U)
        {
            return lh_bool_true;
        }
        ++a;
        ++b;
    }
}

/**
 * @brief First of the first @p n bytes of @p lhs equal to @p val, or ::lh_null.
 *
 * Machine words first (same SWAR family as ::lh_memory_std_find), then a byte
 * tail. A lit lane is rechecked: the subtract can light a neighbor. Zero @p n
 * is not a hit.
 *
 * @param lhs Buffer to scan.
 * @param n   Number of bytes.
 * @param val Byte to look for.
 *
 * @return Pointer to the first match, or ::lh_null.
 *
 * @see lh_memory_bytes_any_eq
 * @see lh_memory_find_step
 */
LH_ATTRIBUTE_FORCE_INLINE
const lh_byte_t *
lh_memory_bytes_find(const lh_byte_ptr lhs, lh_usize_t n, lh_byte_t val)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr p = lhs;

    while (n > 0U && (lh_ptr_to_uaddr(p) & (w - 1U)) != 0U)
    {
        if (lh_ptr_deref(p) == val)
        {
            return p;
        }
        ++p;
        --n;
    }

    if (n >= w)
    {
        const lh_usize_t ones = (~(lh_usize_t)0) / 0xFFU;
        const lh_usize_t high = ones << 7;
        const lh_usize_t vw = ones * val;

        do
        {
            const lh_usize_t x = (*lh_ptr_rcast(const lh_usize_t, p)) ^ vw;
            const lh_usize_t mask = (x - ones) & ~x & high;

            if (mask != 0U)
            {
                lh_usize_t lane;

                for (lane = 0U; lane < w; ++lane)
                {
                    if (p[lane] == val)
                    {
                        return p + lane;
                    }
                }
            }
            p += w;
            n -= w;
        } while (n >= w);
    }

    while (n > 0U)
    {
        if (lh_ptr_deref(p) == val)
        {
            return p;
        }
        ++p;
        --n;
    }
    return lh_null;
}

/**
 * @brief True if any of the first @p n bytes of @p lhs equals @p val.
 *
 * @see lh_memory_bytes_find
 * @see lh_memory_find_step
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_bytes_any_eq(const lh_byte_ptr lhs, lh_usize_t n, lh_byte_t val)
{
    return lh_null_ne(lh_memory_bytes_find(lhs, n, val));
}

/**
 * @brief True if any @p rhs_size-byte unit of @p lhs matches @p rhs on a @p step grid.
 *
 * Used by the wchar / unit path of ::lh_memory_find_step over a scan block.
 * Callers must pass a span where each candidate @c i, @c i+step, … has
 * @p rhs_size bytes in range (block size a multiple of @p step, @p rhs_size
 * equal to @p step). The first needle byte is checked before a full unit compare.
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
lh_memory_bytes_any_eq_step(const lh_byte_ptr lhs, lh_usize_t n, const lh_byte_ptr rhs,
                            lh_usize_t rhs_size, lh_usize_t step)
{
    const lh_byte_t first = lh_ptr_deref(rhs);
    lh_usize_t i;

    for (i = 0; i < n; i += step)
    {
        if (lhs[i] == first && lh_memory_bytes_eq(lhs + i, rhs, rhs_size))
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

/**
 * @brief True if any of the first @p n bytes of @p lhs and @p rhs differ.
 *
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
lh_memory_bytes_any_ne(const lh_byte_ptr lhs, const lh_byte_ptr rhs, lh_usize_t n)
{
    lh_usize_t i;

    for (i = 0; i < n; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_MEMORY_H */
