/**
 * @file sse2.c
 * @brief SSE2/SSSE3 tiers for ::lh_memory_std_*.
 */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("sse2")
lh_void
lh_memory_std_copy64_storeu(lh_byte_ptr dst, lh_byte_cptr src)
{
    const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
    const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
    const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
    const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
    _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
    _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
    _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
    _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
}

/* 64 bytes into a 16-aligned @p dst: the aligned tail block of copy_sse2. */
LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("sse2")
lh_void
lh_memory_std_copy64_store_sse2(lh_byte_ptr dst, lh_byte_cptr src)
{
    const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
    const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
    const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
    const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
}

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("sse2")
lh_void
lh_memory_std_copy128_store_sse2(lh_byte_ptr dst, lh_byte_cptr src)
{
    /* All eight loads ahead of the stores, not two halves of four: measured on an
       Ivy Bridge, splitting it (to spare the Win64 spill of xmm6/7) gained nothing
       and lost a few percent from 1 KB up. */
    const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
    const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
    const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
    const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
    const __m128i v4 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 64));
    const __m128i v5 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 80));
    const __m128i v6 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 96));
    const __m128i v7 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 112));
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 64), v4);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 80), v5);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 96), v6);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 112), v7);
}

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("sse2")
lh_void
lh_memory_std_copy_sse2_small(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    /* CRT-style overlapping vector stores: two (or four) unaligned 16-byte moves
     * cover any length in [16, 63] with no loop, no alignment prologue, no scalar
     * tail. */
    if (n < 64U)
    {
        if (n >= 32U)
        {
            const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
            const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
            const __m128i c = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 32));
            const __m128i d = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), b);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 32), c);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), d);
            return;
        }

        if (n >= 16U)
        {
            const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
            const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), b);
            return;
        }

        lh_memory_std_copy_bytes(dst, src, n);
        return;
    }

    /* Unaligned 64-byte loop + one overlapping last block. Aligned 128-byte
     * movdqa still wins for bigger in-cache copies on SSE2-only targets, but
     * only after the setup cost is amortized — that path is copy_sse2's, n >= 512. */
    {
        lh_byte_ptr dst_end = dst + n;
        lh_byte_cptr src_end = src + n;

        while (n >= 64U)
        {
            lh_memory_std_copy64_storeu(dst, src);
            dst += 64;
            src += 64;
            n -= 64U;
        }

        if (n != 0U)
        {
            lh_memory_std_copy64_storeu(dst_end - 64, src_end - 64);
        }
    }
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static lh_void
lh_memory_std_copy_sse2(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n < 512U)
    {
        lh_memory_std_copy_sse2_small(dst, src, n);
        return;
    }

    {
        lh_byte_ptr aligned_dst = lh_ptr_align_up(lh_byte_t, dst, lh_cast_static(lh_uaddr_t, 16));
        lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));
        lh_byte_ptr dst_end = dst + n;
        lh_byte_cptr src_end = src + n;

        if (head > n)
        {
            head = n;
        }

        if (head != 0U)
        {
            lh_memory_std_copy_bytes(dst, src, head);
            dst += head;
            src += head;
            n -= head;
        }

        while (n >= 128U)
        {
            if (n >= LH_MEMORY_STD_PREFETCH_TRIGGER)
            {
                _mm_prefetch(lh_ptr_ccast(char, src + LH_MEMORY_STD_PREFETCH_DISTANCE),
                             _MM_HINT_T0);
            }

            lh_memory_std_copy128_store_sse2(dst, src);
            dst += 128;
            src += 128;
            n -= 128U;
        }

        if (n >= 64U)
        {
            lh_memory_std_copy64_store_sse2(dst, src);
            dst += 64;
            src += 64;
            n -= 64U;
        }

        if (n != 0U)
        {
            lh_memory_std_copy64_storeu(dst_end - 64, src_end - 64);
        }
    }
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static lh_void
lh_memory_std_copy_sse2_stream(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    lh_byte_ptr dst_end = dst + n;
    lh_byte_cptr src_end = src + n;
    lh_byte_ptr aligned_dst = lh_ptr_align_up(lh_byte_t, dst, lh_cast_static(lh_uaddr_t, 16));
    lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

    if (head > n)
    {
        head = n;
    }

    lh_memory_std_copy_bytes(dst, src, head);
    dst += head;
    src += head;
    n -= head;

    while (n >= 128U)
    {
        const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
        const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
        const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
        const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
        const __m128i v4 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 64));
        const __m128i v5 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 80));
        const __m128i v6 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 96));
        const __m128i v7 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 112));
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 64), v4);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 80), v5);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 96), v6);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 112), v7);
        dst += 128;
        src += 128;
        n -= 128U;
    }

    while (n >= 64U)
    {
        const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
        const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
        const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
        const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
        dst += 64;
        src += 64;
        n -= 64U;
    }

    _mm_sfence();

    if (n != 0U)
    {
        lh_memory_std_copy64_storeu(dst_end - 64, src_end - 64);
    }
}

/* Reverse 16 bytes inside an SSE register using only SSE2 (no SSSE3 pshufb):
 * shuffle dwords end-to-end, swap 16-bit lanes inside each half, then swap the
 * two bytes of every u16. Verified against the scalar reverse-copy path. */
LH_MEMORY_STD_SIMD_TARGET("sse2") static __m128i lh_memory_std_reverse_epi8_sse2(__m128i v)
{
    v = _mm_shuffle_epi32(v, _MM_SHUFFLE(0, 1, 2, 3));
    v = _mm_shufflelo_epi16(v, _MM_SHUFFLE(2, 3, 0, 1));
    v = _mm_shufflehi_epi16(v, _MM_SHUFFLE(2, 3, 0, 1));
    return _mm_or_si128(_mm_slli_epi16(v, 8), _mm_srli_epi16(v, 8));
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static lh_void
lh_memory_std_copy_rev_sse2(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    lh_byte_ptr d_end = dst + n;

    if (n < 32U)
    {
        if (n >= 16U)
        {
            const __m128i a =
                lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src)));
            const __m128i b = lh_memory_std_reverse_epi8_sse2(
                _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16)));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), a);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), b);
            return;
        }

        lh_memory_std_copy_rev_bytes(dst, src, n);
        return;
    }

    while (n >= 64U)
    {
        const __m128i v0 =
            lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0)));
        const __m128i v1 =
            lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16)));
        const __m128i v2 =
            lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32)));
        const __m128i v3 =
            lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48)));
        d_end -= 64;
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 48), v0);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 32), v1);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 16), v2);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 0), v3);
        src += 64;
        n -= 64U;
    }

    lh_memory_std_copy_rev_bytes(dst, src, n);
}

/* Reverse 16 bytes via a single pshufb — measured 1.6x-2.6x faster than the 4-
 * instruction SSE2-only shuffle sequence above across 64B-64KB on this project's
 * own GCC/MinGW toolchain (the mid-size band, ~1KB-4KB, saw the largest win: 2.6x
 * and 2.1x respectively). SSSE3 (2006+ on Intel, Bulldozer+ on AMD) is not part of
 * any baseline ISA, unlike SSE2 on x86-64, so this tier is still runtime-checked. */
LH_MEMORY_STD_SIMD_TARGET("ssse3") static __m128i lh_memory_std_reverse_epi8_ssse3(__m128i v)
{
    const __m128i mask = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    return _mm_shuffle_epi8(v, mask);
}

LH_MEMORY_STD_SIMD_TARGET("ssse3")
static lh_void
lh_memory_std_copy_rev_ssse3(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    lh_byte_ptr d_end = dst + n;

    if (n < 32U)
    {
        if (n >= 16U)
        {
            const __m128i a =
                lh_memory_std_reverse_epi8_ssse3(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src)));
            const __m128i b = lh_memory_std_reverse_epi8_ssse3(
                _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16)));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), a);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), b);
            return;
        }

        lh_memory_std_copy_rev_bytes(dst, src, n);
        return;
    }

    while (n >= 64U)
    {
        const __m128i v0 =
            lh_memory_std_reverse_epi8_ssse3(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0)));
        const __m128i v1 = lh_memory_std_reverse_epi8_ssse3(
            _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16)));
        const __m128i v2 = lh_memory_std_reverse_epi8_ssse3(
            _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32)));
        const __m128i v3 = lh_memory_std_reverse_epi8_ssse3(
            _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48)));
        d_end -= 64;
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 48), v0);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 32), v1);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 16), v2);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 0), v3);
        src += 64;
        n -= 64U;
    }

    lh_memory_std_copy_rev_bytes(dst, src, n);
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static lh_void
lh_memory_std_rcopy_sse2(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n < 64U)
    {
        if (n >= 32U)
        {
            const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
            const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
            const __m128i c = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 32));
            const __m128i d = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), d);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 32), c);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), b);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
            return;
        }

        if (n >= 16U)
        {
            const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
            const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), b);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
            return;
        }

        lh_memory_std_rcopy_bytes(dst, src, n);
        return;
    }

    {
        lh_byte_ptr d = dst + n;
        lh_byte_cptr s = src + n;

        while (n >= 128U)
        {
            d -= 128;
            s -= 128;
            const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 0));
            const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 16));
            const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 32));
            const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 48));
            const __m128i v4 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 64));
            const __m128i v5 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 80));
            const __m128i v6 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 96));
            const __m128i v7 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 112));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 0), v0);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 16), v1);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 32), v2);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 48), v3);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 64), v4);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 80), v5);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 96), v6);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 112), v7);
            n -= 128U;
        }

        while (n >= 64U)
        {
            d -= 64;
            s -= 64;
            const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 0));
            const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 16));
            const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 32));
            const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 48));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 0), v0);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 16), v1);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 32), v2);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 48), v3);
            n -= 64U;
        }

        lh_memory_std_rcopy_bytes(dst, src, n);
    }
}

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("sse2")
lh_void
lh_memory_std_move_small_sse2(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n <= 32U)
    {
        const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
        const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16U));
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16U), b);
        return;
    }

    if (n <= 64U)
    {
        const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
        const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
        const __m128i c = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 32U));
        const __m128i d = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16U));
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), b);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 32U), c);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16U), d);
        return;
    }

    {
        const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
        const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
        const __m128i c = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
        const __m128i d = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
        const __m128i e = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 64U));
        const __m128i f = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 48U));
        const __m128i g = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 32U));
        const __m128i h = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16U));
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), b);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 32), c);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 48), d);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 64U), e);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 48U), f);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 32U), g);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16U), h);
    }
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static lh_void
lh_memory_std_move_forward_sse2(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    const __m128i tail = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16U));
    lh_usize_t i = 0U;

    /* Four blocks per step, all loaded before any is stored: the stores still
     * land below every source byte a later step reads. */
    while (n - i > 64U)
    {
        const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + i));
        const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + i + 16U));
        const __m128i c = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + i + 32U));
        const __m128i d = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + i + 48U));
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + i), a);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + i + 16U), b);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + i + 32U), c);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + i + 48U), d);
        i += 64U;
    }
    while (n - i > 16U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + i), _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + i)));
        i += 16U;
    }
    _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16U), tail);
}

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("sse2")
lh_void
lh_memory_std_set_sse2(lh_byte_ptr dst, lh_byte_t val, lh_usize_t n)
{
    const __m128i v = _mm_set1_epi8(lh_cast_static(char, val));

    if (n < 64U)
    {
        if (n >= 32U)
        {
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 32), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), v);
            return;
        }

        if (n >= 16U)
        {
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), v);
            return;
        }

        lh_memory_std_set_bytes(dst, val, n);
        return;
    }

    {
        lh_byte_ptr dst_end = dst + n;

        if (n >= 512U)
        {
            lh_byte_ptr aligned_dst = lh_ptr_align_up(lh_byte_t, dst, lh_cast_static(lh_uaddr_t, 16));
            lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

            /* The misaligned head (< 16 bytes) goes out as one unaligned vector
             * store, not lh_memory_std_set_bytes: n >= 512, so it stays inside the
             * buffer, and the aligned loop rewrites the overlap with the same value.
             * Under MSVC x86 the scalar head compiled to an __allmul call (set_bytes'
             * 64-bit broadcast) plus rep movsd, a fixed startup cost paid on nearly
             * every call there, since 32-bit malloc only aligns to 8. */
            if (head != 0U)
            {
                _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), v);
                dst += head;
                n -= head;
            }

            while (n >= 64U)
            {
                _mm_store_si128(lh_ptr_rcast(__m128i, dst + 0), v);
                _mm_store_si128(lh_ptr_rcast(__m128i, dst + 16), v);
                _mm_store_si128(lh_ptr_rcast(__m128i, dst + 32), v);
                _mm_store_si128(lh_ptr_rcast(__m128i, dst + 48), v);
                dst += 64;
                n -= 64U;
            }
        }
        else
        {
            while (n >= 64U)
            {
                _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 0), v);
                _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), v);
                _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 32), v);
                _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 48), v);
                dst += 64;
                n -= 64U;
            }
        }

        if (n != 0U)
        {
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst_end - 64), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst_end - 48), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst_end - 32), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst_end - 16), v);
        }
    }
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static lh_void
lh_memory_std_set_sse2_stream(lh_byte_ptr dst, lh_byte_t val, lh_usize_t n)
{
    const __m128i v = _mm_set1_epi8(lh_cast_static(char, val));

    {
        lh_byte_ptr aligned_dst = lh_ptr_align_up(lh_byte_t, dst, lh_cast_static(lh_uaddr_t, 16));
        lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

        if (head > n)
        {
            head = n;
        }

        if (head != 0U)
        {
            lh_memory_std_set_bytes(dst, val, head);
            dst += head;
            n -= head;
        }
    }

    while (n >= 64U)
    {
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 0), v);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 16), v);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 32), v);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 48), v);
        dst += 64;
        n -= 64U;
    }

    while (n >= 16U)
    {
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst), v);
        dst += 16;
        n -= 16U;
    }

    _mm_sfence();

    lh_memory_std_set_bytes(dst, val, n);
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static const lh_ptr
lh_memory_std_compare_sse2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_byte_cptr l = lh_ptr_ccast(lh_byte_t, lhs);
    lh_byte_cptr r = lh_ptr_ccast(lh_byte_t, rhs);

    /* 2-wide unroll on the equal-path hot loop — same ILP idea as the copy tiers;
     * early-exit still fires on the first mismatched register. */
    while (n >= 32U)
    {
        const __m128i va0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, l + 0));
        const __m128i vb0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, r + 0));
        const __m128i va1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, l + 16));
        const __m128i vb1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, r + 16));
        const lh_u32_t eq0 = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va0, vb0)));
        const lh_u32_t eq1 = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va1, vb1)));

        if (eq0 != 0xFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_and(lh_bit_not(eq0), 0xFFFFU));
        }

        if (eq1 != 0xFFFFU)
        {
            return l + 16 + lh_bit_scan_forward_u32(lh_bit_and(lh_bit_not(eq1), 0xFFFFU));
        }

        l += 32;
        r += 32;
        n -= 32U;
    }

    while (n >= 16U)
    {
        const __m128i va = _mm_loadu_si128(lh_ptr_rcast(const __m128i, l));
        const __m128i vb = _mm_loadu_si128(lh_ptr_rcast(const __m128i, r));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
        }

        l += 16;
        r += 16;
        n -= 16U;
    }

    if (n != 0U)
    {
        lh_byte_cptr const l0 = lh_ptr_ccast(lh_byte_t, lhs);
        if (lh_cast_static(lh_usize_t, ((l + n) - l0)) >= 16U)
        {
            lh_byte_cptr lt = l + n - 16U;
            lh_byte_cptr rt = r + n - 16U;
            const __m128i va = _mm_loadu_si128(lh_ptr_rcast(const __m128i, lt));
            const __m128i vb = _mm_loadu_si128(lh_ptr_rcast(const __m128i, rt));
            const lh_u32_t eq_mask =
                lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

            if (eq_mask != 0xFFFFU)
            {
                return lt + lh_bit_scan_forward_u32(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
            }

            return lh_null;
        }
    }

    return lh_memory_std_compare_scalar(l, r, n);
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static const lh_ptr
lh_memory_std_rcompare_sse2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_byte_cptr l = lh_ptr_ccast(lh_byte_t, lhs) + (n - 1U);
    lh_byte_cptr r = lh_ptr_ccast(lh_byte_t, rhs) + (n - 1U);

    while (n >= 32U)
    {
        lh_byte_cptr lb1 = l - 15;
        lh_byte_cptr rb1 = r - 15;
        lh_byte_cptr lb0 = l - 31;
        lh_byte_cptr rb0 = r - 31;

        const __m128i va1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, lb1));
        const __m128i vb1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, rb1));
        const __m128i va0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, lb0));
        const __m128i vb0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, rb0));
        const lh_u32_t eq1 = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va1, vb1)));
        const lh_u32_t eq0 = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va0, vb0)));

        /* Higher-address block first — that's the first mismatch a reverse scan must report. */
        if (eq1 != 0xFFFFU)
        {
            return lb1 + lh_bit_scan_reverse_u32(lh_bit_and(lh_bit_not(eq1), 0xFFFFU));
        }

        if (eq0 != 0xFFFFU)
        {
            return lb0 + lh_bit_scan_reverse_u32(lh_bit_and(lh_bit_not(eq0), 0xFFFFU));
        }

        l -= 32;
        r -= 32;
        n -= 32U;
    }

    while (n >= 16U)
    {
        lh_byte_cptr lb = l - 15;
        lh_byte_cptr rb = r - 15;

        const __m128i va = _mm_loadu_si128(lh_ptr_rcast(const __m128i, lb));
        const __m128i vb = _mm_loadu_si128(lh_ptr_rcast(const __m128i, rb));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFU)
        {
            return lb + lh_bit_scan_reverse_u32(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
        }

        l -= 16;
        r -= 16;
        n -= 16U;
    }

    /* l/r are the tail's *last*-byte pointer (this is a reverse scan), but
     * lh_memory_std_rcompare_bytes expects the tail's *base* pointer (it derives
     * its own last-byte pointer as base + (n-1) internally) — undo that offset once
     * here so the two don't compound. Matches that function's own tolerance
     * for an out-of-range-but-never-dereferenced base pointer when n is 0. */
    return lh_memory_std_rcompare_scalar(l - (n - 1U), r - (n - 1U), n);
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static const lh_ptr
lh_memory_std_find_sse2(lh_byte_cptr hay, lh_usize_t n, lh_byte_cptr needle, lh_usize_t m)
{
    const __m128i first = _mm_set1_epi8(lh_cast_static(char, needle[0]));
    const __m128i last = _mm_set1_epi8(lh_cast_static(char, needle[m - 1U]));
    lh_usize_t off = 0U;

    /* The block at off tests candidates off..off+15; its last-byte load covers
     * off+m-1 .. off+m+14, inside the haystack while n - off >= m + 15. */
    while (n - off >= m + 15U)
    {
        const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, hay + off));
        const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, hay + off + m - 1U));
        lh_u32_t mask = lh_cast_static(
            lh_u32_t, _mm_movemask_epi8(_mm_and_si128(_mm_cmpeq_epi8(a, first), _mm_cmpeq_epi8(b, last))));

        while (mask != 0U)
        {
            const lh_usize_t i = off + lh_bit_scan_forward_u32(mask);
            if (lh_memory_std_find_is_match(hay + i, needle, m))
            {
                return hay + i;
            }
            mask &= mask - 1U;
        }
        off += 16U;
    }
    return lh_memory_std_find_scalar(hay, n, needle, m, off);
}

#endif /* SSE2 */
