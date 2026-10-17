/**
 * @file avx2.c
 * @brief AVX2 tiers for ::lh_memory_std_*.
 */

#if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("avx2")
lh_void
lh_memory_std_copy128_storeu(lh_byte_ptr dst, lh_byte_cptr src)
{
    const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
    const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
    const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
    const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
    _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
    _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
    _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
    _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
}

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("avx2")
lh_void
lh_memory_std_copy256_store(lh_byte_ptr dst, lh_byte_cptr src)
{
    const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
    const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
    const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
    const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
    const __m256i v4 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 128));
    const __m256i v5 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 160));
    const __m256i v6 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 192));
    const __m256i v7 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 224));
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 128), v4);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 160), v5);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 192), v6);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 224), v7);
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static lh_void
lh_memory_std_copy_avx2(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    /* 16-31: overlapping XMM. Compiled under target("avx2") so GCC/Clang emit
     * VEX-encoded vmovdqu, not a legacy-SSE mix. */
    if (n < 32U)
    {
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

    if (n < 64U)
    {
        const __m256i a = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src));
        const __m256i b = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + n - 32));
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), a);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), b);
        return;
    }

    if (n < 128U)
    {
        const __m256i a = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src));
        const __m256i b = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
        const __m256i c = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + n - 64));
        const __m256i d = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + n - 32));
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), a);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 32), b);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 64), c);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), d);
        return;
    }

    {
        lh_byte_ptr dst_end = dst + n;
        lh_byte_cptr src_end = src + n;

        if (n >= 512U)
        {
            lh_byte_ptr aligned_dst =
                lh_ptr_align_up(lh_byte_t, dst, lh_cast_static(lh_uaddr_t, 32));
            lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

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

            while (n >= 256U)
            {
                if (n >= LH_MEMORY_STD_PREFETCH_TRIGGER)
                {
                    _mm_prefetch(lh_ptr_ccast(char, src + LH_MEMORY_STD_PREFETCH_DISTANCE),
                                 _MM_HINT_T0);
                }

                lh_memory_std_copy256_store(dst, src);
                dst += 256;
                src += 256;
                n -= 256U;
            }

            if (n >= 128U)
            {
                const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
                const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
                const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
                const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
                dst += 128;
                src += 128;
                n -= 128U;
            }
        }
        else
        {
            while (n >= 128U)
            {
                lh_memory_std_copy128_storeu(dst, src);
                dst += 128;
                src += 128;
                n -= 128U;
            }
        }

        if (n != 0U)
        {
            lh_memory_std_copy128_storeu(dst_end - 128, src_end - 128);
        }
    }
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static lh_void
lh_memory_std_copy_avx2_stream(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    lh_byte_ptr dst_end = dst + n;
    lh_byte_cptr src_end = src + n;
    lh_byte_ptr aligned_dst = lh_ptr_align_up(lh_byte_t, dst, lh_cast_static(lh_uaddr_t, 32));
    lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

    if (head > n)
    {
        head = n;
    }

    lh_memory_std_copy_bytes(dst, src, head);
    dst += head;
    src += head;
    n -= head;

    while (n >= 256U)
    {
        const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
        const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
        const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
        const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
        const __m256i v4 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 128));
        const __m256i v5 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 160));
        const __m256i v6 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 192));
        const __m256i v7 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 224));
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 128), v4);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 160), v5);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 192), v6);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 224), v7);
        dst += 256;
        src += 256;
        n -= 256U;
    }

    while (n >= 128U)
    {
        const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
        const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
        const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
        const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
        dst += 128;
        src += 128;
        n -= 128U;
    }

    _mm_sfence();

    if (n != 0U)
    {
        lh_memory_std_copy128_storeu(dst_end - 128, src_end - 128);
    }
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static lh_void
lh_memory_std_copy_rev_avx2(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    const __m256i lane_rev = _mm256_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                              15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);

    if (n < 64U)
    {
        if (n >= 32U)
        {
            __m256i a =
                _mm256_shuffle_epi8(_mm256_loadu_si256(lh_ptr_rcast(const __m256i, src)), lane_rev);
            __m256i b = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + n - 32)), lane_rev);
            a = _mm256_permute2x128_si256(a, a, 0x01);
            b = _mm256_permute2x128_si256(b, b, 0x01);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), a);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), b);
            return;
        }

        lh_memory_std_copy_rev_bytes(dst, src, n);
        return;
    }

    {
        lh_byte_ptr d_end = dst + n;

        while (n >= 256U)
        {
            __m256i v0 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0)), lane_rev);
            __m256i v1 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32)), lane_rev);
            __m256i v2 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64)), lane_rev);
            __m256i v3 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96)), lane_rev);
            __m256i v4 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 128)), lane_rev);
            __m256i v5 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 160)), lane_rev);
            __m256i v6 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 192)), lane_rev);
            __m256i v7 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 224)), lane_rev);
            v0 = _mm256_permute2x128_si256(v0, v0, 0x01);
            v1 = _mm256_permute2x128_si256(v1, v1, 0x01);
            v2 = _mm256_permute2x128_si256(v2, v2, 0x01);
            v3 = _mm256_permute2x128_si256(v3, v3, 0x01);
            v4 = _mm256_permute2x128_si256(v4, v4, 0x01);
            v5 = _mm256_permute2x128_si256(v5, v5, 0x01);
            v6 = _mm256_permute2x128_si256(v6, v6, 0x01);
            v7 = _mm256_permute2x128_si256(v7, v7, 0x01);
            d_end -= 256;
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 224), v0);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 192), v1);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 160), v2);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 128), v3);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 96), v4);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 64), v5);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 32), v6);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 0), v7);
            src += 256;
            n -= 256U;
        }

        while (n >= 128U)
        {
            __m256i v0 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0)), lane_rev);
            __m256i v1 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32)), lane_rev);
            __m256i v2 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64)), lane_rev);
            __m256i v3 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96)), lane_rev);
            v0 = _mm256_permute2x128_si256(v0, v0, 0x01);
            v1 = _mm256_permute2x128_si256(v1, v1, 0x01);
            v2 = _mm256_permute2x128_si256(v2, v2, 0x01);
            v3 = _mm256_permute2x128_si256(v3, v3, 0x01);
            d_end -= 128;
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 96), v0);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 64), v1);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 32), v2);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 0), v3);
            src += 128;
            n -= 128U;
        }

        lh_memory_std_copy_rev_bytes(dst, src, n);
    }
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static lh_void
lh_memory_std_rcopy_avx2(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
    if (n < 64U)
    {
        if (n >= 32U)
        {
            const __m256i a = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src));
            const __m256i b = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + n - 32));
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), b);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), a);
            return;
        }

        lh_memory_std_rcopy_bytes(dst, src, n);
        return;
    }

    {
        lh_byte_ptr d = dst + n;
        lh_byte_cptr s = src + n;

        while (n >= 256U)
        {
            d -= 256;
            s -= 256;
            const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 0));
            const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 32));
            const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 64));
            const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 96));
            const __m256i v4 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 128));
            const __m256i v5 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 160));
            const __m256i v6 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 192));
            const __m256i v7 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 224));
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 0), v0);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 32), v1);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 64), v2);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 96), v3);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 128), v4);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 160), v5);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 192), v6);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 224), v7);
            n -= 256U;
        }

        while (n >= 128U)
        {
            d -= 128;
            s -= 128;
            const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 0));
            const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 32));
            const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 64));
            const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 96));
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 0), v0);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 32), v1);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 64), v2);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 96), v3);
            n -= 128U;
        }

        lh_memory_std_rcopy_bytes(dst, src, n);
    }
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static lh_void
lh_memory_std_set_avx2(lh_byte_ptr dst, lh_byte_t val, lh_usize_t n)
{
    const __m256i v = _mm256_set1_epi8(lh_cast_static(char, val));

    if (n < 32U)
    {
        lh_memory_std_set_bytes(dst, val, n);
        return;
    }

    /* First and last 32 bytes cover [32, 64]; four overlapping 32-byte stores
     * cover [64, 128). Two stores leave a hole in the middle for n > 64. */
    if (n < 64U)
    {
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), v);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), v);
        return;
    }

    if (n < 128U)
    {
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), v);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 32), v);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 64), v);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), v);
        return;
    }

    {
        lh_byte_ptr dst_end = dst + n;

        if (n >= 512U)
        {
            lh_byte_ptr aligned_dst =
                lh_ptr_align_up(lh_byte_t, dst, lh_cast_static(lh_uaddr_t, 32));
            lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

            /* The misaligned head (< 32 bytes) goes out as one unaligned vector
             * store, not lh_memory_std_set_bytes: n >= 512, so it stays inside the
             * buffer, and the aligned loop rewrites the overlap with the same value.
             * Under MSVC x86 the scalar head compiled to an __allmul call (set_bytes'
             * 64-bit broadcast) plus rep movsd, a fixed startup cost paid on nearly
             * every call there, since 32-bit malloc only aligns to 8. */
            if (head != 0U)
            {
                _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), v);
                dst += head;
                n -= head;
            }

            while (n >= 256U)
            {
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 0), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 32), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 64), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 96), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 128), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 160), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 192), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 224), v);
                dst += 256;
                n -= 256U;
            }

            while (n >= 128U)
            {
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 0), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 32), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 64), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 96), v);
                dst += 128;
                n -= 128U;
            }
        }
        else
        {
            while (n >= 128U)
            {
                _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 0), v);
                _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 32), v);
                _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 64), v);
                _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 96), v);
                dst += 128;
                n -= 128U;
            }
        }

        if (n != 0U)
        {
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst_end - 128), v);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst_end - 96), v);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst_end - 64), v);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst_end - 32), v);
        }
    }
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static lh_void
lh_memory_std_set_avx2_stream(lh_byte_ptr dst, lh_byte_t val, lh_usize_t n)
{
    const __m256i v = _mm256_set1_epi8(lh_cast_static(char, val));

    {
        lh_byte_ptr aligned_dst = lh_ptr_align_up(lh_byte_t, dst, lh_cast_static(lh_uaddr_t, 32));
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

    while (n >= 128U)
    {
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 0), v);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 32), v);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 64), v);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 96), v);
        dst += 128;
        n -= 128U;
    }

    while (n >= 32U)
    {
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst), v);
        dst += 32;
        n -= 32U;
    }

    _mm_sfence();

    lh_memory_std_set_bytes(dst, val, n);
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static lh_void
lh_memory_std_xor_avx2(lh_byte_ptr dst, lh_byte_cptr lhs, lh_byte_cptr rhs, lh_usize_t n)
{
    lh_usize_t i = 0U;

    for (; lh_math_le(lh_math_add(i, 64U), n); i = lh_math_add(i, 64U))
    {
        const __m256i l0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lhs + i));
        const __m256i l1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lhs + i + 32U));
        const __m256i r0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rhs + i));
        const __m256i r1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rhs + i + 32U));
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + i), _mm256_xor_si256(l0, r0));
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + i + 32U), _mm256_xor_si256(l1, r1));
    }
    if (lh_math_le(lh_math_add(i, 32U), n))
    {
        const __m256i l0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lhs + i));
        const __m256i r0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rhs + i));
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + i), _mm256_xor_si256(l0, r0));
        i = lh_math_add(i, 32U);
    }
    lh_memory_std_xor_bytes(dst + i, lhs + i, rhs + i, lh_math_sub(n, i));
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static const lh_ptr
lh_memory_std_compare_avx2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_byte_cptr l = lh_ptr_ccast(lh_byte_t, lhs);
    lh_byte_cptr r = lh_ptr_ccast(lh_byte_t, rhs);

    while (n >= 128U)
    {
        const __m256i va0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 0));
        const __m256i vb0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 0));
        const __m256i va1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 32));
        const __m256i vb1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 32));
        const __m256i va2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 64));
        const __m256i vb2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 64));
        const __m256i va3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 96));
        const __m256i vb3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 96));
        const lh_u32_t eq0 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va0, vb0)));
        const lh_u32_t eq1 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va1, vb1)));
        const lh_u32_t eq2 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va2, vb2)));
        const lh_u32_t eq3 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va3, vb3)));

        if (eq0 != 0xFFFFFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_not(eq0));
        }

        if (eq1 != 0xFFFFFFFFU)
        {
            return l + 32 + lh_bit_scan_forward_u32(lh_bit_not(eq1));
        }

        if (eq2 != 0xFFFFFFFFU)
        {
            return l + 64 + lh_bit_scan_forward_u32(lh_bit_not(eq2));
        }

        if (eq3 != 0xFFFFFFFFU)
        {
            return l + 96 + lh_bit_scan_forward_u32(lh_bit_not(eq3));
        }

        l += 128;
        r += 128;
        n -= 128U;
    }

    while (n >= 64U)
    {
        const __m256i va0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 0));
        const __m256i vb0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 0));
        const __m256i va1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 32));
        const __m256i vb1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 32));
        const lh_u32_t eq0 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va0, vb0)));
        const lh_u32_t eq1 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va1, vb1)));

        if (eq0 != 0xFFFFFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_not(eq0));
        }

        if (eq1 != 0xFFFFFFFFU)
        {
            return l + 32 + lh_bit_scan_forward_u32(lh_bit_not(eq1));
        }

        l += 64;
        r += 64;
        n -= 64U;
    }

    while (n >= 32U)
    {
        const __m256i va = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l));
        const __m256i vb = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_not(eq_mask));
        }

        l += 32;
        r += 32;
        n -= 32U;
    }

    if (n != 0U)
    {
        lh_byte_cptr const l0 = lh_ptr_ccast(lh_byte_t, lhs);
        if (lh_cast_static(lh_usize_t, ((l + n) - l0)) >= 32U)
        {
            lh_byte_cptr lt = l + n - 32U;
            lh_byte_cptr rt = r + n - 32U;
            const __m256i va = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lt));
            const __m256i vb = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rt));
            const lh_u32_t eq_mask =
                lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)));

            if (eq_mask != 0xFFFFFFFFU)
            {
                return lt + lh_bit_scan_forward_u32(lh_bit_not(eq_mask));
            }

            return lh_null;
        }
    }

    return lh_memory_std_compare_scalar(l, r, n);
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static const lh_ptr
lh_memory_std_rcompare_avx2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_usize_t orig = n;
    lh_byte_cptr l = lh_ptr_ccast(lh_byte_t, lhs) + (n - 1U);
    lh_byte_cptr r = lh_ptr_ccast(lh_byte_t, rhs) + (n - 1U);

    while (n >= 64U)
    {
        lh_byte_cptr lb1 = l - 31;
        lh_byte_cptr rb1 = r - 31;
        lh_byte_cptr lb0 = l - 63;
        lh_byte_cptr rb0 = r - 63;

        const __m256i va1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lb1));
        const __m256i vb1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rb1));
        const __m256i va0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lb0));
        const __m256i vb0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rb0));
        const lh_u32_t eq1 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va1, vb1)));
        const lh_u32_t eq0 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va0, vb0)));

        if (eq1 != 0xFFFFFFFFU)
        {
            return lb1 + lh_bit_scan_reverse_u32(lh_bit_not(eq1));
        }

        if (eq0 != 0xFFFFFFFFU)
        {
            return lb0 + lh_bit_scan_reverse_u32(lh_bit_not(eq0));
        }

        l -= 64;
        r -= 64;
        n -= 64U;
    }

    while (n >= 32U)
    {
        lh_byte_cptr lb = l - 31;
        lh_byte_cptr rb = r - 31;

        const __m256i va = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lb));
        const __m256i vb = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rb));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFFFFFU)
        {
            return lb + lh_bit_scan_reverse_u32(lh_bit_not(eq_mask));
        }

        l -= 32;
        r -= 32;
        n -= 32U;
    }

    if (n != 0U && orig >= 32U)
    {
        lh_byte_cptr lt = lh_ptr_ccast(lh_byte_t, lhs);
        lh_byte_cptr rt = lh_ptr_ccast(lh_byte_t, rhs);
        const __m256i va = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lt));
        const __m256i vb = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rt));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFFFFFU)
        {
            return lt + lh_bit_scan_reverse_u32(lh_bit_not(eq_mask));
        }

        return lh_null;
    }

    return lh_memory_std_rcompare_scalar(l - (n - 1U), r - (n - 1U), n);
}

#endif /* AVX2 */
