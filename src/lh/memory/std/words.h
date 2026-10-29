/**
 * @file words.h
 * @brief Portable ::lh_memory_std_* kernels (no CPUID, no SIMD ISA).
 */

#ifndef LH_MEMORY_STD_WORDS_H
#define LH_MEMORY_STD_WORDS_H

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/** Copy fewer than 16 bytes with two overlapping loads and stores of the widest fitting word. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_copy_tiny(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n >= 8U)
    {
        const lh_u64_t first = lh_memory_load_u64(src);
        const lh_u64_t last = lh_memory_load_u64(src + n - 8U);
        lh_memory_store_u64(dst, first);
        lh_memory_store_u64(dst + n - 8U, last);
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = lh_memory_load_u32(src);
        const lh_u32_t last = lh_memory_load_u32(src + n - 4U);
        lh_memory_store_u32(dst, first);
        lh_memory_store_u32(dst + n - 4U, last);
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first = lh_memory_load_u16(src);
        const lh_u16_t last = lh_memory_load_u16(src + n - 2U);
        lh_memory_store_u16(dst, first);
        lh_memory_store_u16(dst + n - 2U, last);
        return;
    }

    if (n != 0U)
    {
        lh_ptr_deref(dst) = lh_ptr_deref(src);
    }
}

/** Scalar forward copy: 8-byte words, then ::lh_memory_std_copy_tiny for the tail. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_copy_bytes(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n < 16U)
    {
        lh_memory_std_copy_tiny(dst, src, n);
        return;
    }

    do
    {
        lh_memory_store_u64(dst, lh_memory_load_u64(src));
        dst += 8;
        src += 8;
        n -= 8U;
    } while (n >= 8U);

    lh_memory_std_copy_tiny(dst, src, n);
}

/** ::lh_memory_std_copy_rev for fewer than 16 bytes: @p src byte-reversed into @p dst. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_copy_rev_tiny(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n >= 8U)
    {
        const lh_u64_t first = lh_bit_bswap_u64(lh_memory_load_u64(src));
        const lh_u64_t last = lh_bit_bswap_u64(lh_memory_load_u64(src + n - 8U));
        lh_memory_store_u64(dst + n - 8U, first);
        lh_memory_store_u64(dst, last);
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = lh_bit_bswap_u32(lh_memory_load_u32(src));
        const lh_u32_t last = lh_bit_bswap_u32(lh_memory_load_u32(src + n - 4U));
        lh_memory_store_u32(dst + n - 4U, first);
        lh_memory_store_u32(dst, last);
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first_rev = lh_bit_bswap_u16(lh_memory_load_u16(src));
        const lh_u16_t last_rev = lh_bit_bswap_u16(lh_memory_load_u16(src + n - 2U));
        lh_memory_store_u16(dst + n - 2U, first_rev);
        lh_memory_store_u16(dst, last_rev);
        return;
    }

    if (n != 0U)
    {
        lh_ptr_deref(dst) = lh_ptr_deref(src);
    }
}

/** Scalar ::lh_memory_std_copy_rev: @p src byte-reversed into @p dst. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_copy_rev_bytes(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
#if LH_COMPILER_ARCH_FAMILY_IS_X86
    if (n >= 8U)
    {
        lh_byte_ptr d_end = dst + n;

        do
        {
            d_end -= 8;
            lh_memory_store_u64(d_end, lh_bit_bswap_u64(lh_memory_load_u64(src)));
            src += 8;
            n -= 8U;
        } while (n >= 8U);

        lh_memory_std_copy_rev_tiny(dst, src, n);
        return;
    }

    lh_memory_std_copy_rev_tiny(dst, src, n);
#else
    {
        lh_byte_ptr d_end = dst + n;

        while (n >= 4U)
        {
            d_end -= 4;
            lh_memory_store_u32(d_end, lh_bit_bswap_u32(lh_memory_load_u32(src)));
            src += 4;
            n -= 4U;
        }
        lh_memory_std_copy_rev_tiny(dst, src, n);
    }
#endif
}

/** ::lh_memory_std_rcopy for fewer than 16 bytes: copy high to low, safe for overlap. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_rcopy_tiny(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n >= 8U)
    {
        const lh_u64_t first = lh_memory_load_u64(src);
        const lh_u64_t last = lh_memory_load_u64(src + n - 8U);
        lh_memory_store_u64(dst + n - 8U, last);
        lh_memory_store_u64(dst, first);
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = lh_memory_load_u32(src);
        const lh_u32_t last = lh_memory_load_u32(src + n - 4U);
        lh_memory_store_u32(dst + n - 4U, last);
        lh_memory_store_u32(dst, first);
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first = lh_memory_load_u16(src);
        const lh_u16_t last = lh_memory_load_u16(src + n - 2U);
        lh_memory_store_u16(dst + n - 2U, last);
        lh_memory_store_u16(dst, first);
        return;
    }

    if (n != 0U)
    {
        lh_ptr_deref(dst) = lh_ptr_deref(src);
    }
}

/** Scalar ::lh_memory_std_rcopy: copy from the high end down, safe for overlap. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_rcopy_bytes(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
#if LH_COMPILER_ARCH_FAMILY_IS_X86
    if (n >= 8U)
    {
        lh_byte_ptr d = dst + n;
        lh_byte_cptr s = src + n;

        do
        {
            d -= 8;
            s -= 8;
            lh_memory_store_u64(d, lh_memory_load_u64(s));
            n -= 8U;
        } while (n >= 8U);

        lh_memory_std_rcopy_tiny(dst, src, n);
        return;
    }

    lh_memory_std_rcopy_tiny(dst, src, n);
#else
    {
        const lh_usize_t w = sizeof(lh_usize_t);

        while (n > 0U && ((lh_ptr_to_uaddr(dst + n) | lh_ptr_to_uaddr(src + n)) & (w - 1U)) != 0U)
        {
            --n;
            dst[n] = src[n];
        }
        while (n >= w)
        {
            n -= w;
            lh_memory_word_store(dst + n, lh_memory_word_load(src + n));
        }
        while (n > 0U)
        {
            --n;
            dst[n] = src[n];
        }
    }
#endif
}

/** Scalar fill of @p n bytes with @p val. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_set_bytes(lh_byte_ptr dst, lh_byte_t val, lh_usize_t n)
{
#if LH_COMPILER_ARCH_FAMILY_IS_X86
    if (n >= 8U)
    {
        const lh_u64_t v = lh_cast_static(lh_u64_t, val) * 0x0101010101010101ULL;
        do
        {
            lh_memory_store_u64(dst, v);
            dst += 8;
            n -= 8U;
        } while (n >= 8U);

        if (n != 0U)
        {
            lh_memory_store_u64(dst + n - 8U, v);
        }
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t v = lh_cast_static(lh_u32_t, val) * 0x01010101U;
        lh_memory_store_u32(dst, v);
        lh_memory_store_u32(dst + n - 4U, v);
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t v = lh_cast_static(lh_u16_t, lh_cast_static(lh_u16_t, val) * 0x0101U);
        lh_memory_store_u16(dst, v);
        lh_memory_store_u16(dst + n - 2U, v);
        return;
    }

    if (n != 0U)
    {
        lh_ptr_deref(dst) = val;
    }
#else
    {
        const lh_usize_t w = sizeof(lh_usize_t);

        if (n >= w)
        {
            const lh_usize_t v = lh_bit_swar_broadcast(val);

            do
            {
                lh_memory_store_usize(dst, v);
                dst += w;
                n -= w;
            } while (n >= w);

            if (n != 0U)
            {
                lh_memory_store_usize(dst + n - w, v);
            }
            return;
        }

        while (n > 0U)
        {
            lh_ptr_deref(dst++) = val;
            --n;
        }
    }
#endif
}

/** First byte of a machine word already known to differ. */
LH_ATTRIBUTE_FORCE_INLINE
const lh_ptr
lh_memory_std_compare_word_byte(lh_byte_cptr lhs, lh_byte_cptr rhs)
{
    while (lh_ptr_deref(lhs) == lh_ptr_deref(rhs))
    {
        ++lhs;
        ++rhs;
    }
    return lhs;
}

/** Last byte of a machine word already known to differ. @p n is the word size. */
LH_ATTRIBUTE_FORCE_INLINE
const lh_ptr
lh_memory_std_rcompare_word_byte(lh_byte_cptr lhs, lh_byte_cptr rhs, lh_usize_t n)
{
    lh_usize_t i = n;

    do
    {
        --i;
    } while (lhs[i] == rhs[i]);
    return lhs + i;
}

LH_COMPILER_EXTERN_C_END

/** Scalar ::lh_memory_std_compare: the first differing byte of @p lhs, or ::lh_null.
 *  Equal runs go by machine word once both pointers are aligned. */
static const lh_ptr
lh_memory_std_compare_bytes(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr l = lh_ptr_ccast(lh_byte_t, lhs);
    lh_byte_cptr r = lh_ptr_ccast(lh_byte_t, rhs);

    while (n > 0U && ((lh_ptr_to_uaddr(l) | lh_ptr_to_uaddr(r)) & (w - 1U)) != 0U)
    {
        if (lh_ptr_deref(l) != lh_ptr_deref(r))
        {
            return l;
        }
        ++l;
        ++r;
        --n;
    }

    while (n >= w * 2U)
    {
        const lh_usize_t a0 = lh_memory_word_load(l);
        const lh_usize_t b0 = lh_memory_word_load(r);
        const lh_usize_t a1 = lh_memory_word_load(l + w);
        const lh_usize_t b1 = lh_memory_word_load(r + w);

        if (a0 != b0)
        {
            return lh_memory_std_compare_word_byte(l, r);
        }
        if (a1 != b1)
        {
            return lh_memory_std_compare_word_byte(l + w, r + w);
        }
        l += w * 2U;
        r += w * 2U;
        n -= w * 2U;
    }

    if (n >= w)
    {
        if (lh_memory_word_load(l) != lh_memory_word_load(r))
        {
            return lh_memory_std_compare_word_byte(l, r);
        }
        l += w;
        r += w;
        n -= w;
    }

    while (n > 0U)
    {
        if (lh_ptr_deref(l) != lh_ptr_deref(r))
        {
            return l;
        }
        ++l;
        ++r;
        --n;
    }
    return lh_null;
}

/** Scalar ::lh_memory_std_rcompare: the last differing byte of @p lhs, or ::lh_null.
 *  The suffix is compared by machine word once both ends are aligned. */
static const lh_ptr
lh_memory_std_rcompare_bytes(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr l = lh_ptr_ccast(lh_byte_t, lhs);
    lh_byte_cptr r = lh_ptr_ccast(lh_byte_t, rhs);

    if (n == 0U)
    {
        return lh_null;
    }

    while (n > 0U && ((lh_ptr_to_uaddr(l + n) | lh_ptr_to_uaddr(r + n)) & (w - 1U)) != 0U)
    {
        --n;
        if (l[n] != r[n])
        {
            return l + n;
        }
    }

    while (n >= w * 2U)
    {
        n -= w * 2U;
        if (lh_memory_word_load(l + n + w) != lh_memory_word_load(r + n + w))
        {
            return lh_memory_std_rcompare_word_byte(l + n + w, r + n + w, w);
        }
        if (lh_memory_word_load(l + n) != lh_memory_word_load(r + n))
        {
            return lh_memory_std_rcompare_word_byte(l + n, r + n, w);
        }
    }

    if (n >= w)
    {
        n -= w;
        if (lh_memory_word_load(l + n) != lh_memory_word_load(r + n))
        {
            return lh_memory_std_rcompare_word_byte(l + n, r + n, w);
        }
    }

    while (n > 0U)
    {
        --n;
        if (l[n] != r[n])
        {
            return l + n;
        }
    }
    return lh_null;
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
static lh_void
lh_memory_std_copy_simd_scalar(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    lh_memory_std_copy_bytes(dst, src, n);
}
#endif

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
lh_memory_std_copy_words(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_ptr d = lh_ptr_cast(lh_byte_t, dst);
    lh_byte_cptr s = lh_ptr_ccast(lh_byte_t, src);

    /* Lockstep byte peel until both sit on a word boundary — never touch either
       through a misaligned word pointer (strict-alignment targets fault on that).
       Alignment is a mask, not ::lh_math_is_multiple_of: that path is a modulo. */
    while (n > 0U && ((lh_ptr_to_uaddr(d) | lh_ptr_to_uaddr(s)) & (w - 1U)) != 0U)
    {
        lh_ptr_deref(d++) = lh_ptr_deref(s++);
        --n;
    }

    while (n >= w)
    {
        lh_memory_word_store(d, lh_memory_word_load(s));
        d += w;
        s += w;
        n -= w;
    }

    while (n > 0U)
    {
        lh_ptr_deref(d++) = lh_ptr_deref(s++);
        --n;
    }

    return lh_ptr_cast(lh_void, d);
}

lh_ptr
lh_memory_std_fill_words(lh_ptr dst, lh_usize_t word, lh_usize_t n)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    const lh_byte_t byte = lh_cast_static(lh_byte_t, word);
    lh_byte_ptr d = lh_ptr_cast(lh_byte_t, dst);

    while (n > 0U && !lh_ptr_is_aligned(d, w))
    {
        lh_ptr_deref(d++) = byte;
        --n;
    }

    while (n >= w)
    {
        lh_memory_word_store(d, word);
        d += w;
        n -= w;
    }

    while (n > 0U)
    {
        lh_ptr_deref(d++) = byte;
        --n;
    }

    return lh_ptr_cast(lh_void, d);
}

LH_COMPILER_EXTERN_C_END

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
static lh_void
lh_memory_std_copy_rev_simd_scalar(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    lh_memory_std_copy_rev_bytes(dst, src, n);
}

static lh_void
lh_memory_std_rcopy_simd_scalar(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    lh_memory_std_rcopy_bytes(dst, src, n);
}

static lh_void
lh_memory_std_set_simd_scalar(lh_byte_ptr dst, lh_byte_t val, lh_usize_t n)
{
    lh_memory_std_set_bytes(dst, val, n);
}
#endif

LH_COMPILER_EXTERN_C_BEGIN

/** Scalar ::lh_memory_std_xor: machine words, then a byte tail. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_xor_bytes(lh_byte_ptr dst, lh_byte_cptr lhs, lh_byte_cptr rhs, lh_usize_t n)
{
    const lh_usize_t w = sizeof(lh_usize_t);

    while (n > 0U &&
           ((lh_ptr_to_uaddr(dst) | lh_ptr_to_uaddr(lhs) | lh_ptr_to_uaddr(rhs)) & (w - 1U)) != 0U)
    {
        lh_ptr_deref(dst++) = lh_cast_static(lh_byte_t, lh_ptr_deref(lhs++) ^ lh_ptr_deref(rhs++));
        --n;
    }
    while (n >= w)
    {
        lh_memory_word_store(dst, lh_memory_word_load(lhs) ^ lh_memory_word_load(rhs));
        dst += w;
        lhs += w;
        rhs += w;
        n -= w;
    }
    while (n > 0U)
    {
        lh_ptr_deref(dst++) = lh_cast_static(lh_byte_t, lh_ptr_deref(lhs++) ^ lh_ptr_deref(rhs++));
        --n;
    }
}

LH_COMPILER_EXTERN_C_END

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
static const lh_ptr
lh_memory_std_compare_scalar(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    return lh_memory_std_compare_bytes(lhs, rhs, n);
}

static const lh_ptr
lh_memory_std_rcompare_scalar(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    return lh_memory_std_rcompare_bytes(lhs, rhs, n);
}
#endif

LH_COMPILER_EXTERN_C_BEGIN

/** Whether @p cand matches @p needle between its first and last byte (already checked).
 *  Short middles stay local: routing a one-byte gap through ::lh_memory_std_compare
 *  pays the public entry for nothing. */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_std_find_is_match(lh_byte_cptr cand, lh_byte_cptr needle, lh_usize_t m)
{
    if (m <= 2U)
    {
        return lh_bool_true;
    }
    if (m == 3U)
    {
        return lh_cast_static(lh_bool_t, cand[1] == needle[1]);
    }
    return lh_cast_static(lh_bool_t,
                          lh_null_eq(lh_memory_std_compare_bytes(cand + 1, needle + 1, m - 2U)));
}

/** True when the candidate at @p off has the needle's first byte, last byte, and middle. */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_std_find_at(lh_byte_cptr hay, lh_usize_t off, lh_byte_cptr needle, lh_usize_t m,
                      lh_byte_t first, lh_byte_t last)
{
    return lh_cast_static(lh_bool_t, hay[off] == first && hay[off + m - 1U] == last &&
                                         lh_memory_std_find_is_match(hay + off, needle, m));
}

LH_COMPILER_EXTERN_C_END

/** Scalar ::lh_memory_std_find from offset @p off.
 *  Full words of candidates move by sizeof(::lh_usize_t); the tail steps one byte.
 *  The word filter watches one needle end only: the last byte when @p m > 1 (so a
 *  repeated first byte does not light every lane), otherwise the single byte. */
static const lh_ptr
lh_memory_std_find_scalar(lh_byte_cptr hay, lh_usize_t n, lh_byte_cptr needle, lh_usize_t m,
                          lh_usize_t off)
{
    const lh_byte_t first = needle[0];
    const lh_byte_t last = needle[m - 1U];
    const lh_usize_t max_start = n - m;
    const lh_usize_t w = sizeof(lh_usize_t);

    /* Below 32 bytes the byte scan is shorter than the word prologue. */
    if (n >= 32U && n >= m + w - 1U)
    {
        const lh_usize_t filter = lh_bit_swar_broadcast(m > 1U ? last : first);
        const lh_usize_t filter_off = m > 1U ? (m - 1U) : 0U;
        const lh_usize_t word_limit = n - (m + w - 1U);

        while (off <= word_limit)
        {
            /* Lanes whose byte equals the filter byte. A lit lane is not proof: the
             * subtract can light a neighbor, so each candidate is rechecked. */
            const lh_usize_t mask = lh_bit_swar_zero_mask(
                lh_bit_xor(lh_memory_load_usize(hay + off + filter_off), filter));

            if (mask != 0U)
            {
                lh_usize_t lane;

                for (lane = 0U; lane < w; ++lane)
                {
                    if (lh_math_ne(lh_bit_and(mask, lh_bit_swar_lane_bit(lane)), 0U) &&
                        lh_memory_std_find_at(hay, off + lane, needle, m, first, last))
                    {
                        return hay + off + lane;
                    }
                }
            }
            off += w;
        }
    }

    for (; off <= max_start; ++off)
    {
        if (lh_memory_std_find_at(hay, off, needle, m, first, last))
        {
            return hay + off;
        }
    }
    return lh_null;
}

/** Scalar ::lh_memory_std_rfind: same single-end word filter, highest candidate first. */
static const lh_ptr
lh_memory_std_rfind_scalar(lh_byte_cptr hay, lh_usize_t n, lh_byte_cptr needle, lh_usize_t m)
{
    const lh_byte_t first = needle[0];
    const lh_byte_t last = needle[m - 1U];
    const lh_usize_t max_start = n - m;
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_usize_t off;

    if (n >= 32U && n >= m + w - 1U)
    {
        const lh_usize_t filter = lh_bit_swar_broadcast(m > 1U ? last : first);
        const lh_usize_t filter_off = m > 1U ? (m - 1U) : 0U;
        const lh_usize_t word_limit = n - (m + w - 1U);
        const lh_usize_t block = lh_addr_align_down(word_limit, w);

        if (max_start >= block + w)
        {
            for (off = max_start;; --off)
            {
                if (lh_memory_std_find_at(hay, off, needle, m, first, last))
                {
                    return hay + off;
                }
                if (off == block + w)
                {
                    break;
                }
            }
        }

        off = block;
        for (;;)
        {
            const lh_usize_t mask = lh_bit_swar_zero_mask(
                lh_bit_xor(lh_memory_load_usize(hay + off + filter_off), filter));

            if (mask != 0U)
            {
                lh_usize_t lane = w;

                while (lane-- != 0U)
                {
                    if (lh_math_ne(lh_bit_and(mask, lh_bit_swar_lane_bit(lane)), 0U) &&
                        lh_memory_std_find_at(hay, off + lane, needle, m, first, last))
                    {
                        return hay + off + lane;
                    }
                }
            }
            if (off == 0U)
            {
                return lh_null;
            }
            off -= w;
        }
    }

    off = max_start;
    for (;;)
    {
        if (lh_memory_std_find_at(hay, off, needle, m, first, last))
        {
            return hay + off;
        }
        if (off == 0U)
        {
            break;
        }
        --off;
    }
    return lh_null;
}

#endif /* LH_MEMORY_STD_WORDS_H */
