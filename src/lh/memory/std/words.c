/**
 * @file words.c
 * @brief Portable ::lh_memory_std_* kernels (no CPUID, no SIMD ISA).
 */

/** Copy fewer than 16 bytes with two overlapping loads and stores of the widest fitting word. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_copy_tiny(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n >= 8U)
    {
        const lh_u64_t first = lh_ptr_deref(lh_ptr_rcast(const lh_u64_t, src));
        const lh_u64_t last = lh_ptr_deref(lh_ptr_rcast(const lh_u64_t, src + n - 8U));
        lh_ptr_deref(lh_ptr_rcast(lh_u64_t, dst)) = first;
        lh_ptr_deref(lh_ptr_rcast(lh_u64_t, dst + n - 8U)) = last;
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = lh_ptr_deref(lh_ptr_rcast(const lh_u32_t, src));
        const lh_u32_t last = lh_ptr_deref(lh_ptr_rcast(const lh_u32_t, src + n - 4U));
        lh_ptr_deref(lh_ptr_rcast(lh_u32_t, dst)) = first;
        lh_ptr_deref(lh_ptr_rcast(lh_u32_t, dst + n - 4U)) = last;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first = lh_ptr_deref(lh_ptr_rcast(const lh_u16_t, src));
        const lh_u16_t last = lh_ptr_deref(lh_ptr_rcast(const lh_u16_t, src + n - 2U));
        lh_ptr_deref(lh_ptr_rcast(lh_u16_t, dst)) = first;
        lh_ptr_deref(lh_ptr_rcast(lh_u16_t, dst + n - 2U)) = last;
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
        lh_ptr_deref(lh_ptr_rcast(lh_u64_t, dst)) = lh_ptr_deref(lh_ptr_rcast(const lh_u64_t, src));
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
        const lh_u64_t first = lh_bit_bswap_u64(lh_ptr_deref(lh_ptr_rcast(const lh_u64_t, src)));
        const lh_u64_t last = lh_bit_bswap_u64(lh_ptr_deref(lh_ptr_rcast(const lh_u64_t, src + n - 8U)));
        lh_ptr_deref(lh_ptr_rcast(lh_u64_t, dst + n - 8U)) = first;
        lh_ptr_deref(lh_ptr_rcast(lh_u64_t, dst)) = last;
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = lh_bit_bswap_u32(lh_ptr_deref(lh_ptr_rcast(const lh_u32_t, src)));
        const lh_u32_t last = lh_bit_bswap_u32(lh_ptr_deref(lh_ptr_rcast(const lh_u32_t, src + n - 4U)));
        lh_ptr_deref(lh_ptr_rcast(lh_u32_t, dst + n - 4U)) = first;
        lh_ptr_deref(lh_ptr_rcast(lh_u32_t, dst)) = last;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first_rev = lh_bit_bswap_u16(lh_ptr_deref(lh_ptr_rcast(const lh_u16_t, src)));
        const lh_u16_t last_rev = lh_bit_bswap_u16(lh_ptr_deref(lh_ptr_rcast(const lh_u16_t, src + n - 2U)));
        lh_ptr_deref(lh_ptr_rcast(lh_u16_t, dst + n - 2U)) = first_rev;
        lh_ptr_deref(lh_ptr_rcast(lh_u16_t, dst)) = last_rev;
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
            lh_ptr_deref(lh_ptr_rcast(lh_u64_t, d_end)) =
                lh_bit_bswap_u64(lh_ptr_deref(lh_ptr_rcast(const lh_u64_t, src)));
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
            lh_ptr_deref(lh_ptr_rcast(lh_u32_t, d_end)) =
                lh_bit_bswap_u32(lh_ptr_deref(lh_ptr_rcast(const lh_u32_t, src)));
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
        const lh_u64_t first = lh_ptr_deref(lh_ptr_rcast(const lh_u64_t, src));
        const lh_u64_t last = lh_ptr_deref(lh_ptr_rcast(const lh_u64_t, src + n - 8U));
        lh_ptr_deref(lh_ptr_rcast(lh_u64_t, dst + n - 8U)) = last;
        lh_ptr_deref(lh_ptr_rcast(lh_u64_t, dst)) = first;
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = lh_ptr_deref(lh_ptr_rcast(const lh_u32_t, src));
        const lh_u32_t last = lh_ptr_deref(lh_ptr_rcast(const lh_u32_t, src + n - 4U));
        lh_ptr_deref(lh_ptr_rcast(lh_u32_t, dst + n - 4U)) = last;
        lh_ptr_deref(lh_ptr_rcast(lh_u32_t, dst)) = first;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first = lh_ptr_deref(lh_ptr_rcast(const lh_u16_t, src));
        const lh_u16_t last = lh_ptr_deref(lh_ptr_rcast(const lh_u16_t, src + n - 2U));
        lh_ptr_deref(lh_ptr_rcast(lh_u16_t, dst + n - 2U)) = last;
        lh_ptr_deref(lh_ptr_rcast(lh_u16_t, dst)) = first;
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
            lh_ptr_deref(lh_ptr_rcast(lh_u64_t, d)) = lh_ptr_deref(lh_ptr_rcast(const lh_u64_t, s));
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
            lh_ptr_deref(lh_ptr_rcast(lh_usize_t, dst + n)) = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, src + n));
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
            lh_ptr_deref(lh_ptr_rcast(lh_u64_t, dst)) = v;
            dst += 8;
            n -= 8U;
        } while (n >= 8U);

        if (n != 0U)
        {
            lh_ptr_deref(lh_ptr_rcast(lh_u64_t, dst + n - 8U)) = v;
        }
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t v = lh_cast_static(lh_u32_t, val) * 0x01010101U;
        lh_ptr_deref(lh_ptr_rcast(lh_u32_t, dst)) = v;
        lh_ptr_deref(lh_ptr_rcast(lh_u32_t, dst + n - 4U)) = v;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t v = lh_cast_static(lh_u16_t, lh_cast_static(lh_u16_t, val) * 0x0101U);
        lh_ptr_deref(lh_ptr_rcast(lh_u16_t, dst)) = v;
        lh_ptr_deref(lh_ptr_rcast(lh_u16_t, dst + n - 2U)) = v;
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
            const lh_usize_t v = ((~(lh_usize_t)0) / 0xFFU) * val;

            do
            {
                lh_ptr_deref(lh_ptr_rcast(lh_usize_t, dst)) = v;
                dst += w;
                n -= w;
            } while (n >= w);

            if (n != 0U)
            {
                lh_ptr_deref(lh_ptr_rcast(lh_usize_t, dst + n - w)) = v;
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
        const lh_usize_t a0 = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, l));
        const lh_usize_t b0 = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, r));
        const lh_usize_t a1 = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, l + w));
        const lh_usize_t b1 = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, r + w));

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
        if (lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, l)) !=
            lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, r)))
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

    while (n > 0U &&
           ((lh_ptr_to_uaddr(l + n) | lh_ptr_to_uaddr(r + n)) & (w - 1U)) != 0U)
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
        if (lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, l + n + w)) !=
            lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, r + n + w)))
        {
            return lh_memory_std_rcompare_word_byte(l + n + w, r + n + w, w);
        }
        if (lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, l + n)) !=
            lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, r + n)))
        {
            return lh_memory_std_rcompare_word_byte(l + n, r + n, w);
        }
    }

    if (n >= w)
    {
        n -= w;
        if (lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, l + n)) !=
            lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, r + n)))
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
        lh_ptr_deref(lh_ptr_rcast(lh_usize_t, d)) = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, s));
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

    while (n > 0U && (lh_ptr_to_uaddr(d) & (w - 1U)) != 0U)
    {
        lh_ptr_deref(d++) = byte;
        --n;
    }

    while (n >= w)
    {
        lh_ptr_deref(lh_ptr_rcast(lh_usize_t, d)) = word;
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
        lh_ptr_deref(lh_ptr_rcast(lh_usize_t, dst)) =
            lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, lhs)) ^ lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, rhs));
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

/** High bit of address-ordered lane @p lane inside a machine word. Lane 0 is the lowest address. */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_memory_std_find_lane_bit(lh_usize_t lane)
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    lane = sizeof(lh_usize_t) - 1U - lane;
#endif
    return ((lh_usize_t)0x80U) << (lane << 3);
}

/** Lanes of a machine word whose bytes equal the filter broadcast in @p byte_word.
 *  A lit lane is not proof: the subtract can light a neighbor, so the caller rechecks
 *  the candidate. @p ones is 0x0101... and @p high is 0x8080... */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_memory_std_find_byte_mask(lh_byte_cptr at, lh_usize_t ones, lh_usize_t high,
                             lh_usize_t byte_word)
{
    const lh_usize_t x = (lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, at))) ^ byte_word;

    return (x - ones) & ~x & high;
}

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
        const lh_usize_t ones = (~(lh_usize_t)0) / 0xFFU;
        const lh_usize_t high = ones << 7;
        const lh_usize_t filter = ones * (m > 1U ? last : first);
        const lh_usize_t filter_off = m > 1U ? (m - 1U) : 0U;
        const lh_usize_t word_limit = n - (m + w - 1U);

        while (off <= word_limit)
        {
            const lh_usize_t mask =
                lh_memory_std_find_byte_mask(hay + off + filter_off, ones, high, filter);

            if (mask != 0U)
            {
                lh_usize_t lane;

                for (lane = 0U; lane < w; ++lane)
                {
                    if (((mask & lh_memory_std_find_lane_bit(lane)) != 0U) &&
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
        const lh_usize_t ones = (~(lh_usize_t)0) / 0xFFU;
        const lh_usize_t high = ones << 7;
        const lh_usize_t filter = ones * (m > 1U ? last : first);
        const lh_usize_t filter_off = m > 1U ? (m - 1U) : 0U;
        const lh_usize_t word_limit = n - (m + w - 1U);
        const lh_usize_t block = word_limit & ~(w - 1U);

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
            const lh_usize_t mask =
                lh_memory_std_find_byte_mask(hay + off + filter_off, ones, high, filter);

            if (mask != 0U)
            {
                lh_usize_t lane = w;

                while (lane-- != 0U)
                {
                    if (((mask & lh_memory_std_find_lane_bit(lane)) != 0U) &&
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
