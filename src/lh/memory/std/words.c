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
        const lh_u64_t first = *lh_ptr_rcast(const lh_u64_t, src);
        const lh_u64_t last = *lh_ptr_rcast(const lh_u64_t, src + n - 8U);
        *lh_ptr_rcast(lh_u64_t, dst) = first;
        *lh_ptr_rcast(lh_u64_t, dst + n - 8U) = last;
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = *lh_ptr_rcast(const lh_u32_t, src);
        const lh_u32_t last = *lh_ptr_rcast(const lh_u32_t, src + n - 4U);
        *lh_ptr_rcast(lh_u32_t, dst) = first;
        *lh_ptr_rcast(lh_u32_t, dst + n - 4U) = last;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first = *lh_ptr_rcast(const lh_u16_t, src);
        const lh_u16_t last = *lh_ptr_rcast(const lh_u16_t, src + n - 2U);
        *lh_ptr_rcast(lh_u16_t, dst) = first;
        *lh_ptr_rcast(lh_u16_t, dst + n - 2U) = last;
        return;
    }

    if (n != 0U)
    {
        *dst = *src;
    }
}

/** Scalar forward copy: 8-byte words, then ::lh_memory_std_copy_tiny for the tail. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_copy_bytes(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
#if LH_MEMORY_STD_HAVE_COPY_TINY
    if (n < 16U)
    {
        lh_memory_std_copy_tiny(dst, src, n);
        return;
    }

    do
    {
        *lh_ptr_rcast(lh_u64_t, dst) = *lh_ptr_rcast(const lh_u64_t, src);
        dst += 8;
        src += 8;
        n -= 8U;
    } while (n >= 8U);

    lh_memory_std_copy_tiny(dst, src, n);
#else
    while (n--)
    {
        *dst++ = *src++;
    }
#endif
}

/** ::lh_memory_std_copy_rev for fewer than 16 bytes: @p src byte-reversed into @p dst. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_copy_rev_tiny(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n >= 8U)
    {
        const lh_u64_t first = lh_bit_bswap_u64(*lh_ptr_rcast(const lh_u64_t, src));
        const lh_u64_t last = lh_bit_bswap_u64(*lh_ptr_rcast(const lh_u64_t, src + n - 8U));
        *lh_ptr_rcast(lh_u64_t, dst + n - 8U) = first;
        *lh_ptr_rcast(lh_u64_t, dst) = last;
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = lh_bit_bswap_u32(*lh_ptr_rcast(const lh_u32_t, src));
        const lh_u32_t last = lh_bit_bswap_u32(*lh_ptr_rcast(const lh_u32_t, src + n - 4U));
        *lh_ptr_rcast(lh_u32_t, dst + n - 4U) = first;
        *lh_ptr_rcast(lh_u32_t, dst) = last;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first_rev = lh_bit_bswap_u16(*lh_ptr_rcast(const lh_u16_t, src));
        const lh_u16_t last_rev = lh_bit_bswap_u16(*lh_ptr_rcast(const lh_u16_t, src + n - 2U));
        *lh_ptr_rcast(lh_u16_t, dst + n - 2U) = first_rev;
        *lh_ptr_rcast(lh_u16_t, dst) = last_rev;
        return;
    }

    if (n != 0U)
    {
        *dst = *src;
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
            *lh_ptr_rcast(lh_u64_t, d_end) = lh_bit_bswap_u64(*lh_ptr_rcast(const lh_u64_t, src));
            src += 8;
            n -= 8U;
        } while (n >= 8U);

        lh_memory_std_copy_rev_tiny(dst, src, n);
        return;
    }

    lh_memory_std_copy_rev_tiny(dst, src, n);
#else
    dst += n;
    while (n--)
    {
        *--dst = *src++;
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
        const lh_u64_t first = *lh_ptr_rcast(const lh_u64_t, src);
        const lh_u64_t last = *lh_ptr_rcast(const lh_u64_t, src + n - 8U);
        *lh_ptr_rcast(lh_u64_t, dst + n - 8U) = last;
        *lh_ptr_rcast(lh_u64_t, dst) = first;
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = *lh_ptr_rcast(const lh_u32_t, src);
        const lh_u32_t last = *lh_ptr_rcast(const lh_u32_t, src + n - 4U);
        *lh_ptr_rcast(lh_u32_t, dst + n - 4U) = last;
        *lh_ptr_rcast(lh_u32_t, dst) = first;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first = *lh_ptr_rcast(const lh_u16_t, src);
        const lh_u16_t last = *lh_ptr_rcast(const lh_u16_t, src + n - 2U);
        *lh_ptr_rcast(lh_u16_t, dst + n - 2U) = last;
        *lh_ptr_rcast(lh_u16_t, dst) = first;
        return;
    }

    if (n != 0U)
    {
        *dst = *src;
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
            *lh_ptr_rcast(lh_u64_t, d) = *lh_ptr_rcast(const lh_u64_t, s);
            n -= 8U;
        } while (n >= 8U);

        lh_memory_std_rcopy_tiny(dst, src, n);
        return;
    }

    lh_memory_std_rcopy_tiny(dst, src, n);
#else
    dst += n;
    src += n;
    while (n--)
    {
        *--dst = *--src;
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
            *lh_ptr_rcast(lh_u64_t, dst) = v;
            dst += 8;
            n -= 8U;
        } while (n >= 8U);

        if (n != 0U)
        {
            *lh_ptr_rcast(lh_u64_t, dst + n - 8U) = v;
        }
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t v = lh_cast_static(lh_u32_t, val) * 0x01010101U;
        *lh_ptr_rcast(lh_u32_t, dst) = v;
        *lh_ptr_rcast(lh_u32_t, dst + n - 4U) = v;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t v = lh_cast_static(lh_u16_t, lh_cast_static(lh_u16_t, val) * 0x0101U);
        *lh_ptr_rcast(lh_u16_t, dst) = v;
        *lh_ptr_rcast(lh_u16_t, dst + n - 2U) = v;
        return;
    }

    if (n != 0U)
    {
        *dst = val;
    }
#else
    while (n--)
    {
        *dst++ = val;
    }
#endif
}

/** Scalar ::lh_memory_std_compare: the first differing byte of @p lhs, or ::lh_null. */
static const lh_ptr
lh_memory_std_compare_bytes(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_byte_cptr l = lh_ptr_ccast(lh_byte_t, lhs);
    lh_byte_cptr r = lh_ptr_ccast(lh_byte_t, rhs);

    while (n >= LH_MEMORY_STD_SCAN_BLOCK)
    {
        if (lh_memory_bytes_any_ne(l, r, LH_MEMORY_STD_SCAN_BLOCK))
        {
            break;
        }
        l += LH_MEMORY_STD_SCAN_BLOCK;
        r += LH_MEMORY_STD_SCAN_BLOCK;
        n -= LH_MEMORY_STD_SCAN_BLOCK;
    }

    while (n--)
    {
        if (*l != *r)
        {
            return l;
        }
        ++l;
        ++r;
    }
    return lh_null;
}

/** Scalar ::lh_memory_std_rcompare: the last differing byte of @p lhs, or ::lh_null. */
static const lh_ptr
lh_memory_std_rcompare_bytes(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_byte_cptr l = lh_ptr_ccast(lh_byte_t, lhs) + (n - 1U);
    lh_byte_cptr r = lh_ptr_ccast(lh_byte_t, rhs) + (n - 1U);

    while (n--)
    {
        if (*l != *r)
        {
            return l;
        }
        --l;
        --r;
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

    /* The two pointers advance together, so stepping one byte at a time until
       each sits on a word boundary aligns the source and the destination in the
       same pass — neither is ever touched through a misaligned pointer, which is
       the access a strict-alignment target faults on. */
    while (n > 0U && ((lh_ptr_to_uaddr(d) % w) != 0U || (lh_ptr_to_uaddr(s) % w) != 0U))
    {
        *d++ = *s++;
        --n;
    }

    while (n >= w)
    {
        *lh_ptr_rcast(lh_usize_t, d) = *lh_ptr_rcast(const lh_usize_t, s);
        d += w;
        s += w;
        n -= w;
    }

    while (n > 0U)
    {
        *d++ = *s++;
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

    while (n > 0U && (lh_ptr_to_uaddr(d) % w) != 0U)
    {
        *d++ = byte;
        --n;
    }

    while (n >= w)
    {
        *lh_ptr_rcast(lh_usize_t, d) = word;
        d += w;
        n -= w;
    }

    while (n > 0U)
    {
        *d++ = byte;
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

/** Scalar ::lh_memory_std_xor: @p dst = @p lhs ^ @p rhs, byte by byte. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_std_xor_bytes(lh_byte_ptr dst, lh_byte_cptr lhs, lh_byte_cptr rhs, lh_usize_t n)
{
    lh_usize_t i;

    for (i = 0U; lh_math_lt(i, n); i = lh_math_add_one(i))
    {
        dst[i] = lh_cast_static(lh_byte_t, lh_math_bit_xor(lhs[i], rhs[i]));
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

/** Whether @p cand matches @p needle between its first and last byte (already checked). */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_std_find_is_match(lh_byte_cptr cand, lh_byte_cptr needle, lh_usize_t m)
{
    return lh_cast_static(lh_bool_t,
                          m <= 2U || lh_null_eq(lh_memory_std_compare(cand + 1, needle + 1, m - 2U)));
}

/** Scalar ::lh_memory_std_find from offset @p off: first and last byte as a filter,
 * then the middle. */
static const lh_ptr
lh_memory_std_find_scalar(lh_byte_cptr hay, lh_usize_t n, lh_byte_cptr needle, lh_usize_t m,
                          lh_usize_t off)
{
    const lh_byte_t first = needle[0];
    const lh_byte_t last = needle[m - 1U];
    const lh_usize_t max_start = n - m;

    for (; off <= max_start; ++off)
    {
        if (hay[off] == first && hay[off + m - 1U] == last &&
            lh_memory_std_find_is_match(hay + off, needle, m))
        {
            return hay + off;
        }
    }
    return lh_null;
}
