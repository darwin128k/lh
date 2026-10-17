/**
 * @file cpuid.c
 * @brief Runtime x86 feature gates for memory/std.
 */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

#    if LH_LIBRARY_OPTION_MEMORY_STD_USE_ERMS
static lh_byte_t m_erms_copy_kind;
#    endif
static lh_usize_t m_stream_cache_size;

LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_std_has_sse2(void)
{
#    if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64
    return lh_bool_true;
#    else
    static lh_byte_t s_has_sse2; /* 0 = unknown, 1 = yes, 2 = no */

    if (s_has_sse2 == 0U)
    {
        s_has_sse2 = lh_cpu_simd_has_sse2() ? 1U : 2U;
    }
    return lh_cast_static(lh_bool_t, s_has_sse2 == 1U);
#    endif
}

LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_std_use_erms_copy(void)
{
    if (m_erms_copy_kind == 0U)
    {
        m_erms_copy_kind = (lh_cpu_vendor_is_intel() && lh_cpu_simd_has_erms()) ? 1U : 2U;
    }
    return lh_cast_static(lh_bool_t, m_erms_copy_kind == 1U);
}

static lh_usize_t
lh_memory_std_stream_cache_size_resolve(void)
{
    const lh_usize_t l3 = lh_cpu_cache_get_l3_size();

    m_stream_cache_size = l3 > LH_MEMORY_STD_STREAM_FLOOR ? l3 : LH_MEMORY_STD_STREAM_FLOOR;
    return m_stream_cache_size;
}

LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_memory_std_is_stream_size(lh_usize_t n, lh_usize_t traffic)
{
    lh_usize_t cache_size;

    if (n < LH_MEMORY_STD_STREAM_FLOOR)
    {
        return lh_bool_false;
    }
    cache_size = m_stream_cache_size;
    if (cache_size == 0U)
    {
        cache_size = lh_memory_std_stream_cache_size_resolve();
    }
    /* n >= cache_size / traffic, without n * traffic overflowing. */
    return lh_cast_static(lh_bool_t, n >= cache_size / traffic);
}

#endif /* SSE2 || AVX2 */
