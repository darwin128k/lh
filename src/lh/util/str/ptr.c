#include <lh/util/str/ptr.h>
#include <lh/bit/scan.h>
#include <lh/char/limits.h>
#include <lh/char/map.h>
#include <lh/memory.h>
#include <lh/optional/ref.h>
#include <lh/size/limits.h>
#include <lh/util/char.h>
#include <lh/util/memory.h>
#include <lh/math.h>
#include <lh/util/return.h>
#include <lh/assert.h>

static const lh_char_t m_whitespace_chars[] = {lh_char_map_space, lh_char_map_lf, lh_char_map_cr,
                                               lh_char_map_ht,    lh_char_map_vt, lh_char_map_nul};

static const lh_usize_t m_whitespace_size = lh_str_ptr_get_size(m_whitespace_chars);

/* Bitset for m_whitespace_chars[0..m_whitespace_size): HT LF VT CR SP (NUL is the array sentinel). */
static const lh_byte_t m_whitespace_set[LH_STR_PTR_CHAR_SET_BYTES] = {
    [1] = 0x2Eu, /* HT|LF|VT|CR */
    [4] = 0x01u, /* SP */
};

/** Default whitespace bitset, or a filled local when @p chars is custom. */
LH_ATTRIBUTE_FORCE_INLINE const lh_byte_t *
lh_str_ptr_whitespace_set(const lh_str_ptr chars, lh_usize_t chars_size, lh_byte_t *scratch)
{
    if (chars == m_whitespace_chars && chars_size == m_whitespace_size)
    {
        return m_whitespace_set;
    }
    lh_str_ptr_fill_char_set(scratch, chars, chars_size);
    return scratch;
}

lh_usize_t
lh_str_ptr_put_char(lh_str_ptr str, lh_usize_t pos, lh_char_t ch)
{
    str[pos] = ch;
    return lh_math_add_one(pos);
}

lh_usize_t
lh_str_ptr_put_chars(lh_str_ptr str, lh_usize_t pos, const lh_str_ptr chars, lh_usize_t n)
{
    lh_memory_std_copy(lh_ptr_add_by_offset_unsafe(lh_char_t, str, pos), chars, n);
    return lh_math_add(pos, n);
}

lh_usize_t
lh_str_ptr_put_run(lh_str_ptr str, lh_usize_t pos, lh_char_t ch, lh_usize_t n)
{
    lh_memory_std_set(lh_ptr_add_by_offset_unsafe(lh_char_t, str, pos), lh_cast_static(lh_byte_t, ch), n);
    return lh_math_add(pos, n);
}

lh_void
lh_str_ptr_fill_char_set(lh_byte_t *set, const lh_str_ptr chars, lh_usize_t chars_size)
{
    lh_memory_set(set, LH_STR_PTR_CHAR_SET_BYTES, 0);
    for (lh_usize_t i = 0; i < chars_size; ++i)
    {
        const lh_byte_t b = lh_cast_static(lh_byte_t, chars[i]);
        set[b >> 3] =
            lh_cast_static(lh_byte_t, set[b >> 3] | lh_cast_static(lh_byte_t, 1U << (b & 7U)));
    }
}

/** 0x0101… and address-ordered 0,1,2,… lane ramp for ascending-charset word checks. */
static const lh_usize_t lh_str_ptr_swar_ones = (~(lh_usize_t)0) / 0xFFU;
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    if defined(__SIZEOF_SIZE_T__) && __SIZEOF_SIZE_T__ == 8
static const lh_usize_t lh_str_ptr_swar_ramp = 0x0001020304050607ULL;
#    else
static const lh_usize_t lh_str_ptr_swar_ramp = 0x00010203UL;
#    endif
#else
#    if defined(__SIZEOF_SIZE_T__) && __SIZEOF_SIZE_T__ == 8
static const lh_usize_t lh_str_ptr_swar_ramp = 0x0706050403020100ULL;
#    else
static const lh_usize_t lh_str_ptr_swar_ramp = 0x03020100UL;
#    endif
#endif

/**
 * True when @p chars is an ascending contiguous run (e.g. "abc", "0"-"9", "a"-"z").
 * Avoids building a bitset for the common alphabet / digit class scans.
 * Contiguity is checked with machine words of an arithmetic ramp, not one byte at a time.
 */
static lh_bool_t
lh_str_ptr_chars_ascending_range(const lh_str_ptr chars, lh_usize_t chars_size, lh_byte_t *lo,
                                 lh_byte_t *hi)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    const lh_usize_t ones = lh_str_ptr_swar_ones;
    const lh_usize_t ramp = lh_str_ptr_swar_ramp;
    lh_byte_cptr p;
    lh_byte_t first;
    lh_usize_t i;

    if (chars_size < 2U)
    {
        return lh_bool_false;
    }

    first = lh_cast_static(lh_byte_t, chars[0]);
    if (lh_cast_static(lh_usize_t, first) + chars_size - 1U > 255U ||
        lh_cast_static(lh_byte_t, chars[chars_size - 1U]) !=
            lh_cast_static(lh_byte_t, lh_cast_static(lh_usize_t, first) + chars_size - 1U))
    {
        return lh_bool_false;
    }

    p = lh_ptr_cast(const lh_byte_t, chars);
    i = 0U;
    while (i + w <= chars_size)
    {
        const lh_usize_t expect = (lh_cast_static(lh_usize_t, first) + i) * ones + ramp;

        if (lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p + i)) != expect)
        {
            return lh_bool_false;
        }
        i += w;
    }
    while (i < chars_size)
    {
        if (lh_cast_static(lh_byte_t, p[i]) !=
            lh_cast_static(lh_byte_t, lh_cast_static(lh_usize_t, first) + i))
        {
            return lh_bool_false;
        }
        ++i;
    }

    *lo = first;
    *hi = lh_cast_static(lh_byte_t, lh_cast_static(lh_usize_t, first) + chars_size - 1U);
    return lh_bool_true;
}

/**
 * hasless/hasmore out-of-range mask; @p hi must be < 128.
 * @p high is 0x80…, @p sub_lo is ones*lo (0 when lo==0), @p add_hi is ones*(127-hi).
 */
LH_ATTRIBUTE_FORCE_INLINE lh_usize_t
lh_str_ptr_swar_out_of_range_mask_pre(lh_usize_t v, lh_usize_t high, lh_usize_t sub_lo,
                                      lh_usize_t add_hi)
{
    lh_usize_t out = 0U;

    if (lh_math_ne(sub_lo, 0U))
    {
        out |= (v - sub_lo) & ~v & high;
    }
    out |= ((v + add_hi) | v) & high;
    return out;
}

/** Address-ordered first lane with a 0x80 marker bit in @p out (nonzero). */
LH_ATTRIBUTE_FORCE_INLINE lh_usize_t
lh_str_ptr_swar_first_marked_lane(lh_usize_t out)
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    if defined(__SIZEOF_SIZE_T__) && __SIZEOF_SIZE_T__ == 8
    return lh_bit_shr(lh_math_sub(63U, lh_bit_scan_reverse_u64(out)), 3U);
#    else
    return lh_bit_shr(lh_math_sub(31U, lh_bit_scan_reverse_u32(lh_cast_static(lh_u32_t, out))), 3U);
#    endif
#else
#    if defined(__SIZEOF_SIZE_T__) && __SIZEOF_SIZE_T__ == 8
    return lh_bit_shr(lh_bit_scan_forward_u64(out), 3U);
#    else
    return lh_bit_shr(lh_bit_scan_forward_u32(lh_cast_static(lh_u32_t, out)), 3U);
#    endif
#endif
}

/** Address-ordered last lane with a 0x80 marker bit in @p out (nonzero). */
LH_ATTRIBUTE_FORCE_INLINE lh_usize_t
lh_str_ptr_swar_last_marked_lane(lh_usize_t out)
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    if defined(__SIZEOF_SIZE_T__) && __SIZEOF_SIZE_T__ == 8
    return lh_bit_shr(lh_bit_scan_forward_u64(out), 3U);
#    else
    return lh_bit_shr(lh_bit_scan_forward_u32(lh_cast_static(lh_u32_t, out)), 3U);
#    endif
#else
#    if defined(__SIZEOF_SIZE_T__) && __SIZEOF_SIZE_T__ == 8
    return lh_bit_shr(lh_bit_scan_reverse_u64(out), 3U);
#    else
    return lh_bit_shr(lh_bit_scan_reverse_u32(lh_cast_static(lh_u32_t, out)), 3U);
#    endif
#endif
}

/**
 * First byte of @p n at @p p outside inclusive [@p lo, @p hi], or ::lh_null.
 * Word path uses hasless/hasmore SWAR (valid while @p hi < 128).
 */
static const lh_str_ptr
lh_str_ptr_find_byte_not_in_range(lh_byte_cptr p, lh_usize_t n, lh_byte_t lo, lh_byte_t hi)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr end = p + n;
    const lh_bool_t swar = lh_cast_static(lh_bool_t, hi < 128U);

    while (p < end && (lh_ptr_to_uaddr(p) & (w - 1U)) != 0U)
    {
        const lh_byte_t b = lh_ptr_deref(p);

        if (b < lo || b > hi)
        {
            return lh_ptr_cast(lh_char_t, p);
        }
        ++p;
    }

    if (swar)
    {
        const lh_usize_t ones = lh_str_ptr_swar_ones;
        const lh_usize_t high = ones << 7;
        const lh_usize_t sub_lo = lo != 0U ? ones * lo : 0U;
        const lh_usize_t add_hi = ones * (127U - hi);

        while (p + (w + w) <= end)
        {
            const lh_usize_t v0 = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p));
            const lh_usize_t v1 = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p + w));
            const lh_usize_t m0 = lh_str_ptr_swar_out_of_range_mask_pre(v0, high, sub_lo, add_hi);
            const lh_usize_t m1 = lh_str_ptr_swar_out_of_range_mask_pre(v1, high, sub_lo, add_hi);

            if (lh_math_ne(m0, 0U))
            {
                return lh_ptr_cast(lh_char_t, p + lh_str_ptr_swar_first_marked_lane(m0));
            }
            if (lh_math_ne(m1, 0U))
            {
                return lh_ptr_cast(lh_char_t, p + w + lh_str_ptr_swar_first_marked_lane(m1));
            }
            p += w + w;
        }

        while (p + w <= end)
        {
            const lh_usize_t v = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p));
            const lh_usize_t out = lh_str_ptr_swar_out_of_range_mask_pre(v, high, sub_lo, add_hi);

            if (lh_math_ne(out, 0U))
            {
                return lh_ptr_cast(lh_char_t, p + lh_str_ptr_swar_first_marked_lane(out));
            }
            p += w;
        }
    }

    while (p < end)
    {
        const lh_byte_t b = lh_ptr_deref(p);

        if (b < lo || b > hi)
        {
            return lh_ptr_cast(lh_char_t, p);
        }
        ++p;
    }

    return lh_null;
}

/**
 * First byte of @p n at @p p inside inclusive [@p lo, @p hi], or ::lh_null.
 * When a whole word is in-range the SWAR out-mask is 0 and the first lane wins.
 */
static const lh_str_ptr
lh_str_ptr_find_byte_in_range(lh_byte_cptr p, lh_usize_t n, lh_byte_t lo, lh_byte_t hi)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr end = p + n;
    const lh_bool_t swar = lh_cast_static(lh_bool_t, hi < 128U);

    while (p < end && (lh_ptr_to_uaddr(p) & (w - 1U)) != 0U)
    {
        const lh_byte_t b = lh_ptr_deref(p);

        if (b >= lo && b <= hi)
        {
            return lh_ptr_cast(lh_char_t, p);
        }
        ++p;
    }

    if (swar)
    {
        const lh_usize_t ones = lh_str_ptr_swar_ones;
        const lh_usize_t high = ones << 7;
        const lh_usize_t sub_lo = lo != 0U ? ones * lo : 0U;
        const lh_usize_t add_hi = ones * (127U - hi);

        while (p + w <= end)
        {
            const lh_usize_t v = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p));
            const lh_usize_t out = lh_str_ptr_swar_out_of_range_mask_pre(v, high, sub_lo, add_hi);
            lh_usize_t in;

            if (lh_math_is_zero(out))
            {
                return lh_ptr_cast(lh_char_t, p);
            }
            /* Lanes in range: high bits clear in out. Find first clear 0x80. */
            in = ~out & high;
            if (lh_math_ne(in, 0U))
            {
                return lh_ptr_cast(lh_char_t, p + lh_str_ptr_swar_first_marked_lane(in));
            }
            p += w;
        }
    }

    while (p < end)
    {
        const lh_byte_t b = lh_ptr_deref(p);

        if (b >= lo && b <= hi)
        {
            return lh_ptr_cast(lh_char_t, p);
        }
        ++p;
    }

    return lh_null;
}

/**
 * Last byte of @p n at @p base outside inclusive [@p lo, @p hi], or ::lh_null.
 * @p n must be non-zero.
 */
static const lh_str_ptr
lh_str_ptr_rfind_byte_not_in_range(lh_byte_cptr base, lh_usize_t n, lh_byte_t lo, lh_byte_t hi)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr p = base + n;
    const lh_bool_t swar = lh_cast_static(lh_bool_t, hi < 128U);

    while (p > base && (lh_ptr_to_uaddr(p) & (w - 1U)) != 0U)
    {
        --p;
        {
            const lh_byte_t b = lh_ptr_deref(p);

            if (b < lo || b > hi)
            {
                return lh_ptr_cast(lh_char_t, p);
            }
        }
    }

    if (swar)
    {
        const lh_usize_t ones = lh_str_ptr_swar_ones;
        const lh_usize_t high = ones << 7;
        const lh_usize_t sub_lo = lo != 0U ? ones * lo : 0U;
        const lh_usize_t add_hi = ones * (127U - hi);

        while (p >= base + (w + w))
        {
            const lh_usize_t v1 = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p - w));
            const lh_usize_t v0 = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p - (w + w)));
            const lh_usize_t m1 = lh_str_ptr_swar_out_of_range_mask_pre(v1, high, sub_lo, add_hi);
            const lh_usize_t m0 = lh_str_ptr_swar_out_of_range_mask_pre(v0, high, sub_lo, add_hi);

            p -= w + w;
            if (lh_math_ne(m1, 0U))
            {
                return lh_ptr_cast(lh_char_t, p + w + lh_str_ptr_swar_last_marked_lane(m1));
            }
            if (lh_math_ne(m0, 0U))
            {
                return lh_ptr_cast(lh_char_t, p + lh_str_ptr_swar_last_marked_lane(m0));
            }
        }

        while (p >= base + w)
        {
            lh_usize_t out;

            p -= w;
            out = lh_str_ptr_swar_out_of_range_mask_pre(
                lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p)), high, sub_lo, add_hi);
            if (lh_math_ne(out, 0U))
            {
                return lh_ptr_cast(lh_char_t, p + lh_str_ptr_swar_last_marked_lane(out));
            }
        }
    }

    while (p > base)
    {
        --p;
        {
            const lh_byte_t b = lh_ptr_deref(p);

            if (b < lo || b > hi)
            {
                return lh_ptr_cast(lh_char_t, p);
            }
        }
    }

    return lh_null;
}

/**
 * Last byte of @p n at @p base inside inclusive [@p lo, @p hi], or ::lh_null.
 * @p n must be non-zero.
 */
static const lh_str_ptr
lh_str_ptr_rfind_byte_in_range(lh_byte_cptr base, lh_usize_t n, lh_byte_t lo, lh_byte_t hi)
{
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr p = base + n;
    const lh_bool_t swar = lh_cast_static(lh_bool_t, hi < 128U);

    while (p > base && (lh_ptr_to_uaddr(p) & (w - 1U)) != 0U)
    {
        --p;
        {
            const lh_byte_t b = lh_ptr_deref(p);

            if (b >= lo && b <= hi)
            {
                return lh_ptr_cast(lh_char_t, p);
            }
        }
    }

    if (swar)
    {
        const lh_usize_t ones = lh_str_ptr_swar_ones;
        const lh_usize_t high = ones << 7;
        const lh_usize_t sub_lo = lo != 0U ? ones * lo : 0U;
        const lh_usize_t add_hi = ones * (127U - hi);

        while (p >= base + w)
        {
            lh_usize_t out;
            lh_usize_t in;

            p -= w;
            out = lh_str_ptr_swar_out_of_range_mask_pre(
                lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p)), high, sub_lo, add_hi);
            if (lh_math_is_zero(out))
            {
                return lh_ptr_cast(lh_char_t, p + (w - 1U));
            }
            in = ~out & high;
            if (lh_math_ne(in, 0U))
            {
                return lh_ptr_cast(lh_char_t, p + lh_str_ptr_swar_last_marked_lane(in));
            }
        }
    }

    while (p > base)
    {
        --p;
        {
            const lh_byte_t b = lh_ptr_deref(p);

            if (b >= lo && b <= hi)
            {
                return lh_ptr_cast(lh_char_t, p);
            }
        }
    }

    return lh_null;
}

const lh_str_ptr
lh_str_ptr_find_of_char(const lh_str_ptr str, lh_usize_t size, lh_char_t ch)
{
    return lh_memory_find(str, size, lh_addr_of(ch), LH_CHAR_T_SIZE);
}

const lh_str_ptr
lh_str_ptr_rfind_of_char(const lh_str_ptr str, lh_usize_t size, lh_char_t ch)
{
    return lh_memory_rfind(str, size, lh_addr_of(ch), LH_CHAR_T_SIZE);
}

const lh_str_ptr
lh_str_ptr_find_of_null_terminator(const lh_str_ptr str)
{
    /* Page-safe glibc-style strlen: aligned words never cross a page edge.
     * Avoids ::lh_memory_scan's address-space bound and size_rest on every step. */
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr p = lh_ptr_cast(const lh_byte_t, str);
    const lh_usize_t ones = (~(lh_usize_t)0) / 0xFFU;
    const lh_usize_t high = ones << 7;

    lh_assert_runtime_ref(str);

    while ((lh_ptr_to_uaddr(p) & (w - 1U)) != 0U)
    {
        if (lh_ptr_deref(p) == 0U)
        {
            return lh_ptr_cast(lh_char_t, p);
        }
        ++p;
    }

    for (;;)
    {
        const lh_usize_t v = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p));

        if (((v - ones) & ~v & high) != 0U)
        {
            lh_usize_t lane;

            for (lane = 0U; lane < w; ++lane)
            {
                if (p[lane] == 0U)
                {
                    return lh_ptr_cast(lh_char_t, p + lane);
                }
            }
        }
        p += w;
    }
}

lh_str_ptr
lh_str_ptr_copy(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src, lh_usize_t src_size)
{
    return lh_memory_copy(str, str_size, src, src_size);
}

lh_str_ptr
lh_str_ptr_move(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src, lh_usize_t src_size)
{
    return lh_memory_move(str, str_size, src, src_size);
}

lh_str_ptr
lh_str_ptr_set_pattern(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                       lh_usize_t src_size)
{
    return lh_memory_set_pattern(str, str_size, src, src_size);
}

lh_str_ptr
lh_str_ptr_set(lh_str_ptr str, lh_usize_t size, lh_char_t ch)
{
    return lh_memory_set(str, size, ch);
}

const lh_str_ptr
lh_str_ptr_compare_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                                  lh_usize_t src_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(src);

    const lh_usize_t n = lh_math_min(str_size, src_size);
    for (lh_usize_t i = 0; i < n; ++i)
    {
        if (lh_char_fold_case(str[i]) != lh_char_fold_case(src[i]))
        {
            return str + i;
        }
    }
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_rcompare_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                                   lh_usize_t src_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(src);

    const lh_usize_t n = lh_math_min(str_size, src_size);
    for (lh_usize_t k = 0; k < n; ++k)
    {
        const lh_str_ptr lp = str + str_size - 1 - k;
        const lh_str_ptr rp = src + src_size - 1 - k;

        if (lh_char_fold_case(lh_ptr_deref(lp)) != lh_char_fold_case(lh_ptr_deref(rp)))
        {
            return lp;
        }
    }
    return lh_null;
}

lh_str_ptr
lh_str_ptr_erase(lh_str_ptr str, lh_usize_t index)
{
    lh_assert_runtime_ref(str);
    str[index] = lh_char_map_nul;
    return str;
}

const lh_str_ptr
lh_str_ptr_find_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                         lh_usize_t chars_size)
{
    lh_byte_t set[LH_STR_PTR_CHAR_SET_BYTES];
    lh_byte_t lo;
    lh_byte_t hi;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(chars);
    lh_return_ifn(chars_size, lh_null);

    if (chars_size == 1U)
    {
        return lh_str_ptr_find_of_char(str, str_size, chars[0]);
    }

    /* Two needles: dual compare beats a 32-byte bitset fill (case-pair ignore_case). */
    if (chars_size == 2U)
    {
        const lh_char_t a = chars[0];
        const lh_char_t b = chars[1];
        lh_usize_t i;

        for (i = 0U; i < str_size; ++i)
        {
            const lh_char_t c = str[i];

            if (c == a || c == b)
            {
                return str + i;
            }
        }
        return lh_null;
    }

    if (lh_str_ptr_chars_ascending_range(chars, chars_size, lh_addr_of(lo), lh_addr_of(hi)))
    {
        return lh_str_ptr_find_byte_in_range(lh_ptr_cast(const lh_byte_t, str), str_size, lo, hi);
    }

    lh_str_ptr_fill_char_set(set, chars, chars_size);
    for (lh_usize_t i = 0; i < str_size; ++i)
    {
        if (lh_str_ptr_char_set_has(set, str[i]))
        {
            return str + i;
        }
    }
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_rfind_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                          lh_usize_t chars_size)
{
    lh_byte_t set[LH_STR_PTR_CHAR_SET_BYTES];
    lh_byte_t lo;
    lh_byte_t hi;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(chars);
    lh_return_ifn(chars_size, lh_null);

    /* do-while starts with i = str_size; --i would wrap on unsigned 0 */
    lh_return_ifn(str_size, lh_null);

    if (chars_size == 1U)
    {
        return lh_str_ptr_rfind_of_char(str, str_size, chars[0]);
    }

    if (chars_size == 2U)
    {
        const lh_char_t a = chars[0];
        const lh_char_t b = chars[1];
        lh_usize_t i = str_size;

        do
        {
            const lh_char_t c = str[--i];

            if (c == a || c == b)
            {
                return str + i;
            }
        } while (i > 0U);
        return lh_null;
    }

    if (lh_str_ptr_chars_ascending_range(chars, chars_size, lh_addr_of(lo), lh_addr_of(hi)))
    {
        return lh_str_ptr_rfind_byte_in_range(lh_ptr_cast(const lh_byte_t, str), str_size, lo, hi);
    }

    lh_str_ptr_fill_char_set(set, chars, chars_size);
    {
        lh_usize_t i = str_size;
        do
        {
            --i;
            if (lh_str_ptr_char_set_has(set, str[i]))
            {
                return str + i;
            }
        } while (i > 0);
    }
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_find_not_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                             lh_usize_t chars_size)
{
    lh_byte_t set[LH_STR_PTR_CHAR_SET_BYTES];
    lh_byte_t lo;
    lh_byte_t hi;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(chars);

    if (lh_str_ptr_chars_ascending_range(chars, chars_size, lh_addr_of(lo), lh_addr_of(hi)))
    {
        return lh_str_ptr_find_byte_not_in_range(lh_ptr_cast(const lh_byte_t, str), str_size, lo,
                                                 hi);
    }

    lh_str_ptr_fill_char_set(set, chars, chars_size);
    for (lh_usize_t i = 0; i < str_size; ++i)
    {
        if (!lh_str_ptr_char_set_has(set, str[i]))
        {
            return str + i;
        }
    }
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_rfind_not_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                              lh_usize_t chars_size)
{
    lh_byte_t set[LH_STR_PTR_CHAR_SET_BYTES];
    lh_byte_t lo;
    lh_byte_t hi;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(chars);

    /* do-while starts with i = str_size; --i would wrap on unsigned 0 */
    lh_return_ifn(str_size, lh_null);

    if (lh_str_ptr_chars_ascending_range(chars, chars_size, lh_addr_of(lo), lh_addr_of(hi)))
    {
        return lh_str_ptr_rfind_byte_not_in_range(lh_ptr_cast(const lh_byte_t, str), str_size, lo,
                                                  hi);
    }

    lh_str_ptr_fill_char_set(set, chars, chars_size);
    {
        lh_usize_t i = str_size;
        do
        {
            --i;
            if (!lh_str_ptr_char_set_has(set, str[i]))
            {
                return str + i;
            }
        } while (i > 0);
    }
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_find_of_null_terminator_by_size(const lh_str_ptr str, lh_usize_t size)
{
    /* Same page-safe SWAR as ::lh_str_ptr_find_of_null_terminator, capped at @p size. */
    const lh_usize_t w = sizeof(lh_usize_t);
    lh_byte_cptr p = lh_ptr_cast(const lh_byte_t, str);
    lh_byte_cptr end = p + size;
    const lh_usize_t ones = (~(lh_usize_t)0) / 0xFFU;
    const lh_usize_t high = ones << 7;

    lh_assert_runtime_ref(str);
    lh_return_ifn(size, lh_null);

    while (p < end && (lh_ptr_to_uaddr(p) & (w - 1U)) != 0U)
    {
        if (lh_ptr_deref(p) == 0U)
        {
            return lh_ptr_cast(lh_char_t, p);
        }
        ++p;
    }

    while (p + w <= end)
    {
        const lh_usize_t v = lh_ptr_deref(lh_ptr_rcast(const lh_usize_t, p));

        if (((v - ones) & ~v & high) != 0U)
        {
            lh_usize_t lane;

            for (lane = 0U; lane < w; ++lane)
            {
                if (p[lane] == 0U)
                {
                    return lh_ptr_cast(lh_char_t, p + lane);
                }
            }
        }
        p += w;
    }

    while (p < end)
    {
        if (lh_ptr_deref(p) == 0U)
        {
            return lh_ptr_cast(lh_char_t, p);
        }
        ++p;
    }

    return lh_null;
}

lh_usize_t
lh_str_ptr_len(const lh_str_ptr str)
{
    const lh_str_ptr ptr = lh_str_ptr_find_of_null_terminator(str);
    lh_assert_runtime_ifn(lh_ptr_is_set(ptr), lh_runtime_error_code_no_null_terminator);
    return lh_ptr_udiff(ptr, str);
}

const lh_str_ptr
lh_str_ptr_find_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                               lh_usize_t src_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(src);

    lh_return_ifn(str_size, lh_null);
    lh_return_ifn(src_size, lh_null);
    /* lh_math_sub(str_size, src_size) below would underflow without this guard */
    lh_return_if(lh_math_lt(str_size, src_size), lh_null);

    /* One code unit: ASCII letters → both cases via find_of_chars; else fold scan. */
    if (lh_math_eq(src_size, 1U))
    {
        const lh_char_t ch = src[0];
        const lh_uchar_t u = lh_cast_static(lh_uchar_t, ch);

        if (u < 128U)
        {
            if (u >= 65U && u <= 90U)
            {
                const lh_char_t pair[2] = {ch, lh_cast_static(lh_char_t, u + 32U)};

                return lh_str_ptr_find_of_chars(str, str_size, pair, 2U);
            }
            if (u >= 97U && u <= 122U)
            {
                const lh_char_t pair[2] = {lh_cast_static(lh_char_t, u - 32U), ch};

                return lh_str_ptr_find_of_chars(str, str_size, pair, 2U);
            }
            return lh_str_ptr_find_of_char(str, str_size, ch);
        }

        {
            const lh_char_t folded = lh_char_fold_case(ch);
            lh_usize_t i;

            for (i = 0U; i < str_size; ++i)
            {
                if (lh_math_eq(lh_char_fold_case(str[i]), folded))
                {
                    return str + i;
                }
            }
            return lh_null;
        }
    }

    {
        const lh_usize_t last = lh_math_sub(str_size, src_size);
        lh_usize_t i;

        for (i = 0U; i <= last; ++i)
        {
            if (lh_null_eq(lh_str_ptr_compare_by_ignore_case(str + i, src_size, src, src_size)))
            {
                return str + i;
            }
        }
    }

    return lh_null;
}

const lh_str_ptr
lh_str_ptr_rfind_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                                lh_usize_t src_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(src);

    lh_return_ifn(str_size, lh_null);
    lh_return_ifn(src_size, lh_null);
    /* lh_math_sub(str_size, src_size) below would underflow without this guard */
    lh_return_if(lh_math_lt(str_size, src_size), lh_null);

    if (lh_math_eq(src_size, 1U))
    {
        const lh_char_t ch = src[0];
        const lh_uchar_t u = lh_cast_static(lh_uchar_t, ch);

        if (u < 128U)
        {
            if (u >= 65U && u <= 90U)
            {
                const lh_char_t pair[2] = {ch, lh_cast_static(lh_char_t, u + 32U)};

                return lh_str_ptr_rfind_of_chars(str, str_size, pair, 2U);
            }
            if (u >= 97U && u <= 122U)
            {
                const lh_char_t pair[2] = {lh_cast_static(lh_char_t, u - 32U), ch};

                return lh_str_ptr_rfind_of_chars(str, str_size, pair, 2U);
            }
            return lh_str_ptr_rfind_of_char(str, str_size, ch);
        }

        {
            const lh_char_t folded = lh_char_fold_case(ch);
            lh_usize_t i = str_size;

            do
            {
                --i;
                if (lh_math_eq(lh_char_fold_case(str[i]), folded))
                {
                    return str + i;
                }
            } while (i > 0U);
            return lh_null;
        }
    }

    {
        lh_usize_t i = lh_math_sub(str_size, src_size);

        do
        {
            if (lh_null_eq(lh_str_ptr_compare_by_ignore_case(str + i, src_size, src, src_size)))
            {
                return str + i;
            }
        } while (i-- > 0);
    }

    return lh_null;
}

const lh_str_ptr
lh_str_ptr_compare(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                   lh_usize_t src_size, lh_bool_t ignore_case)
{
    return ignore_case ? lh_str_ptr_compare_by_ignore_case(str, str_size, src, src_size)
                       : lh_memory_compare(str, str_size, src, src_size);
}

const lh_str_ptr
lh_str_ptr_rcompare(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                    lh_usize_t src_size, lh_bool_t ignore_case)
{
    return ignore_case ? lh_str_ptr_rcompare_by_ignore_case(str, str_size, src, src_size)
                       : lh_memory_rcompare(str, str_size, src, src_size);
}

lh_bool_t
lh_str_ptr_contains_char(const lh_str_ptr chars, lh_usize_t chars_size, lh_char_t ch)
{
    return lh_null_ne(lh_str_ptr_find_of_char(chars, chars_size, ch));
}

lh_str_ptr
lh_str_ptr_clear(lh_str_ptr str)
{
    return lh_str_ptr_erase(str, 0);
}

const lh_str_ptr
lh_str_ptr_find(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                lh_usize_t src_size, lh_bool_t ignore_case)
{
    return ignore_case ? lh_str_ptr_find_by_ignore_case(str, str_size, src, src_size)
                       : lh_memory_find(str, str_size, src, src_size);
}

const lh_str_ptr
lh_str_ptr_rfind(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                 lh_usize_t src_size, lh_bool_t ignore_case)
{
    return ignore_case ? lh_str_ptr_rfind_by_ignore_case(str, str_size, src, src_size)
                       : lh_memory_rfind(str, str_size, src, src_size);
}

lh_bool_t
lh_str_ptr_starts_with_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                               lh_usize_t src_size, lh_bool_t ignore_case)
{
    if (lh_math_lt(str_size, src_size))
    {
        return lh_bool_false;
    }
    return lh_null_eq(lh_str_ptr_compare(str, src_size, src, src_size, ignore_case));
}

lh_bool_t
lh_str_ptr_ends_with_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                             lh_usize_t src_size, lh_bool_t ignore_case)
{
    if (lh_math_lt(str_size, src_size))
    {
        return lh_bool_false;
    }
    return lh_null_eq(lh_str_ptr_rcompare(str, str_size, src, src_size, ignore_case));
}

lh_bool_t
lh_str_ptr_equals_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                          lh_usize_t src_size, lh_bool_t ignore_case)
{
    return lh_math_eq(str_size, src_size) &&
           lh_null_eq(lh_str_ptr_compare(str, str_size, src, src_size, ignore_case));
}

lh_str_ptr
lh_str_ptr_copy_except(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                       lh_usize_t src_size, const lh_str_ptr except, lh_usize_t except_size,
                       const lh_char_t *replace)
{
    lh_byte_t set[LH_STR_PTR_CHAR_SET_BYTES];
    lh_usize_t si;
    lh_usize_t di;
    lh_char_t only;
    const lh_bool_t single = lh_cast_static(lh_bool_t, except_size == 1U);

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(src);
    lh_assert_runtime_ref(except);

    if (!single && except_size > 1U)
    {
        lh_str_ptr_fill_char_set(set, except, except_size);
    }
    only = single ? except[0] : lh_char_map_nul;

    di = 0;
    for (si = 0; si < src_size && di < str_size; ++si)
    {
        const lh_char_t c = src[si];
        const lh_bool_t hit =
            single ? lh_cast_static(lh_bool_t, c == only)
                   : (except_size > 0U && lh_str_ptr_char_set_has(set, c));

        if (hit)
        {
            lh_optional_ref(replace)
            {
                str[di++] = lh_ptr_deref(replace);
            }
            continue;
        }
        str[di++] = c;
    }
    return str + di;
}

lh_str_ptr
lh_str_ptr_rtrim_custom(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr whitespace_chars,
                        lh_usize_t whitespace_size)
{
    lh_byte_t scratch[LH_STR_PTR_CHAR_SET_BYTES];
    const lh_byte_t *set;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(whitespace_chars);

    if (!str_size || !whitespace_size)
    {
        return str;
    }

    set = lh_str_ptr_whitespace_set(whitespace_chars, whitespace_size, scratch);

    {
        lh_usize_t end = str_size;
        while (lh_math_is_positive(end) && lh_str_ptr_char_set_has(set, str[end - 1]))
        {
            --end;
        }

        str[end] = lh_char_map_nul;
    }
    return str;
}

lh_str_ptr
lh_str_ptr_ltrim_custom(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr whitespace_chars,
                        lh_usize_t whitespace_size)
{
    lh_byte_t scratch[LH_STR_PTR_CHAR_SET_BYTES];
    const lh_byte_t *set;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(whitespace_chars);

    if (!str_size || !whitespace_size)
    {
        return str;
    }

    set = lh_str_ptr_whitespace_set(whitespace_chars, whitespace_size, scratch);

    {
        lh_usize_t offset = 0;
        while (lh_math_lt(offset, str_size) && lh_str_ptr_char_set_has(set, str[offset]))
        {
            ++offset;
        }

        lh_return_ifn(offset, str);
        if (lh_math_eq(offset, str_size))
        {
            return lh_str_ptr_clear(str);
        }

        {
            const lh_usize_t trimmed_size = str_size - offset;
            lh_str_ptr_move(str, trimmed_size + 1, str + offset, trimmed_size + 1);
        }
    }
    return str;
}

lh_usize_t
lh_str_ptr_index_of_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                            lh_usize_t src_size, lh_bool_t ignore_case)
{
    /* empty needle matches at position 0 by convention */
    lh_return_ifn(src_size, LH_USIZE_T_MIN);
    if (lh_math_lt(str_size, src_size))
    {
        return LH_STR_PTR_INVALID;
    }

    const lh_str_ptr ptr = lh_str_ptr_find(str, str_size, src, src_size, ignore_case);
    return lh_null_ne(ptr) ? lh_ptr_udiff(ptr, str) : LH_STR_PTR_INVALID;
}

lh_bool_t
lh_str_ptr_contains_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                            lh_usize_t src_size, lh_bool_t ignore_case)
{
    return lh_null_ne(lh_str_ptr_find(str, str_size, src, src_size, ignore_case));
}

lh_bool_t
lh_str_ptr_starts_with(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    /* Prefix walk only — no haystack/needle strlen. */
    {
        lh_usize_t i = 0;
        for (;;)
        {
            const lh_char_t bc = src[i];
            if (lh_math_eq(bc, lh_char_map_nul))
            {
                return lh_bool_true;
            }
            if (ignore_case)
            {
                if (lh_math_ne(lh_char_fold_case(str[i]), lh_char_fold_case(bc)))
                {
                    return lh_bool_false;
                }
            }
            else if (lh_math_ne(str[i], bc))
            {
                return lh_bool_false;
            }
            i = lh_math_add_one(i);
        }
    }
}

lh_bool_t
lh_str_ptr_ends_with(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    /* Needle length (usually short) + one NUL scan of the haystack — never two
       full strlens (the old ignore_case path paid len(str)+len(src)). */
    lh_usize_t n = 0;

    while (lh_math_ne(src[n], lh_char_map_nul))
    {
        n = lh_math_add_one(n);
    }
    lh_return_ifn(n, lh_bool_true);

    {
        const lh_str_ptr end = lh_str_ptr_find_of_null_terminator(str);
        const lh_usize_t str_size = lh_ptr_udiff(end, str);

        if (lh_math_lt(str_size, n))
        {
            return lh_bool_false;
        }
        return lh_null_eq(lh_str_ptr_compare(end - n, n, src, n, ignore_case));
    }
}

lh_bool_t
lh_str_ptr_equals(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    if (!ignore_case)
    {
        return lh_memory_bytes_eq_cstr(lh_ptr_cast(const lh_byte_t, str),
                                       lh_ptr_cast(const lh_byte_t, src));
    }

    /* Paired fold walk — no double strlen. */
    {
        lh_usize_t i = 0;
        for (;;)
        {
            const lh_char_t ac = str[i];
            const lh_char_t bc = src[i];

            if (lh_math_ne(lh_char_fold_case(ac), lh_char_fold_case(bc)))
            {
                return lh_bool_false;
            }
            if (lh_math_eq(ac, lh_char_map_nul))
            {
                return lh_math_eq(bc, lh_char_map_nul);
            }
            i = lh_math_add_one(i);
        }
    }
}

lh_str_ptr
lh_str_ptr_rtrim_set(lh_str_ptr str, const lh_str_ptr whitespace_chars, lh_usize_t whitespace_size)
{
    return lh_str_ptr_rtrim_custom(str, lh_str_ptr_len(str), whitespace_chars, whitespace_size);
}

lh_str_ptr
lh_str_ptr_trim_custom(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr whitespace_chars,
                       lh_usize_t whitespace_size)
{
    lh_byte_t scratch[LH_STR_PTR_CHAR_SET_BYTES];
    const lh_byte_t *set;
    lh_usize_t start;
    lh_usize_t end;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(whitespace_chars);

    if (!str_size || !whitespace_size)
    {
        return str;
    }

    set = lh_str_ptr_whitespace_set(whitespace_chars, whitespace_size, scratch);

    end = str_size;
    while (lh_math_is_positive(end) && lh_str_ptr_char_set_has(set, str[end - 1]))
    {
        --end;
    }

    start = 0;
    while (lh_math_lt(start, end) && lh_str_ptr_char_set_has(set, str[start]))
    {
        ++start;
    }

    if (lh_math_eq(start, 0))
    {
        str[end] = lh_char_map_nul;
        return str;
    }
    if (lh_math_eq(start, end))
    {
        return lh_str_ptr_clear(str);
    }

    {
        const lh_usize_t n = end - start;
        lh_str_ptr_move(str, n + 1, str + start, n);
        str[n] = lh_char_map_nul;
    }
    return str;
}

lh_str_ptr
lh_str_ptr_ltrim_set(lh_str_ptr str, const lh_str_ptr whitespace_chars, lh_usize_t whitespace_size)
{
    return lh_str_ptr_ltrim_custom(str, lh_str_ptr_len(str), whitespace_chars, whitespace_size);
}

lh_usize_t
lh_str_ptr_index_of(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    const lh_usize_t src_size = lh_str_ptr_len(src);

    /* Empty needle matches at 0 (same as ::lh_str_ptr_index_of_by_size). */
    lh_return_ifn(src_size, LH_USIZE_T_MIN);

    if (!ignore_case && lh_math_eq(src_size, 1))
    {
        /* Stop at the first match or NUL — no full haystack strlen. */
        const lh_char_t ch = src[0];
        lh_usize_t i = 0;
        for (;;)
        {
            const lh_char_t c = str[i];
            if (lh_math_eq(c, lh_char_map_nul))
            {
                return LH_STR_PTR_INVALID;
            }
            if (lh_math_eq(c, ch))
            {
                return i;
            }
            i = lh_math_add_one(i);
        }
    }

    return lh_str_ptr_index_of_by_size(str, lh_str_ptr_len(str), src, src_size, ignore_case);
}

lh_str_ptr
lh_str_ptr_rtrim_auto(lh_str_ptr str, const lh_str_ptr whitespace_chars)
{
    return lh_str_ptr_rtrim_set(str, whitespace_chars, lh_str_ptr_len(whitespace_chars));
}

lh_str_ptr
lh_str_ptr_trim_set(lh_str_ptr str, const lh_str_ptr whitespace_chars, lh_usize_t whitespace_size)
{
    return lh_str_ptr_trim_custom(str, lh_str_ptr_len(str), whitespace_chars, whitespace_size);
}

lh_str_ptr
lh_str_ptr_ltrim_auto(lh_str_ptr str, const lh_str_ptr whitespace_chars)
{
    return lh_str_ptr_ltrim_set(str, whitespace_chars, lh_str_ptr_len(whitespace_chars));
}

lh_str_ptr
lh_str_ptr_ltrim(lh_str_ptr str)
{
    return lh_str_ptr_ltrim_set(str, m_whitespace_chars, m_whitespace_size);
}

lh_str_ptr
lh_str_ptr_rtrim(lh_str_ptr str)
{
    return lh_str_ptr_rtrim_set(str, m_whitespace_chars, m_whitespace_size);
}

lh_bool_t
lh_str_ptr_contains(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    return lh_math_ne(lh_str_ptr_index_of(str, src, ignore_case), LH_STR_PTR_INVALID);
}

lh_str_ptr
lh_str_ptr_trim_auto(lh_str_ptr str, const lh_str_ptr whitespace_chars)
{
    return lh_str_ptr_trim_set(str, whitespace_chars, lh_str_ptr_len(whitespace_chars));
}

lh_str_ptr
lh_str_ptr_trim(lh_str_ptr str)
{
    return lh_str_ptr_trim_set(str, m_whitespace_chars, m_whitespace_size);
}
