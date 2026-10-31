#include <lh/memory.h>
#include <lh/assert.h>
#include <lh/byte/ptr.h>
#include <lh/config.h>
#include <lh/math.h>
#include <lh/util/memory.h>
#include <lh/util/return.h>
#include <lh/memory/word.h>

lh_ptr
lh_memory_copy(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size)
{
    const lh_usize_t n = lh_math_min(dst_size, src_size);
    return lh_memory_std_copy(dst, src, n);
}

lh_ptr
lh_memory_copy_rev(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size)
{
    const lh_usize_t n = lh_math_min(dst_size, src_size);
    return lh_memory_std_copy_rev(dst, src, n);
}

lh_ptr
lh_memory_rcopy(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size)
{
    const lh_usize_t n = lh_math_min(dst_size, src_size);
    return lh_memory_std_rcopy(dst, src, n);
}

lh_ptr
lh_memory_move(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size)
{
    const lh_usize_t n = lh_math_min(dst_size, src_size);
    return lh_memory_std_move(dst, src, n);
}

lh_ptr
lh_memory_set(lh_ptr dst, lh_usize_t size, lh_byte_t val)
{
    return lh_memory_std_set(dst, val, size);
}

lh_ptr
lh_memory_set_u16(lh_u16_t *dst, lh_usize_t count, lh_u16_t val)
{
    lh_usize_t i = 0U;

    /* Eight per turn: cheap loop overhead unoptimized, still vectorized at -O2/-O3. */
    for (; lh_math_le(lh_math_add(i, 8U), count); i = lh_math_add(i, 8U))
    {
        dst[i] = val, dst[i + 1U] = val, dst[i + 2U] = val, dst[i + 3U] = val;
        dst[i + 4U] = val, dst[i + 5U] = val, dst[i + 6U] = val, dst[i + 7U] = val;
    }
    for (; lh_math_lt(i, count); i = lh_math_add_one(i))
    {
        dst[i] = val;
    }
    return lh_addr_of(dst[count]);
}

lh_ptr
lh_memory_set_u32(lh_u32_t *dst, lh_usize_t count, lh_u32_t val)
{
    lh_usize_t i = 0U;

    /* Eight per turn: cheap loop overhead unoptimized, still vectorized at -O2/-O3.
       Measured on pixmap rows, both this and a machine-word variant land on the same
       ~25 GB/s fill bandwidth. */
    for (; lh_math_le(lh_math_add(i, 8U), count); i = lh_math_add(i, 8U))
    {
        dst[i] = val, dst[i + 1U] = val, dst[i + 2U] = val, dst[i + 3U] = val;
        dst[i + 4U] = val, dst[i + 5U] = val, dst[i + 6U] = val, dst[i + 7U] = val;
    }
    for (; lh_math_lt(i, count); i = lh_math_add_one(i))
    {
        dst[i] = val;
    }
    return lh_addr_of(dst[count]);
}

lh_ptr
lh_memory_xor(lh_ptr dst, lh_usize_t dst_size, const lh_ptr lhs, lh_usize_t lhs_size,
              const lh_ptr rhs, lh_usize_t rhs_size)
{
    const lh_usize_t n = lh_math_min(dst_size, lh_math_min(lhs_size, rhs_size));
    return lh_memory_std_xor(dst, lhs, rhs, n);
}

const lh_ptr
lh_memory_compare(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size)
{
    const lh_usize_t n = lh_math_min(lhs_size, rhs_size);
    return lh_memory_std_compare(lhs, rhs, n);
}

const lh_ptr
lh_memory_rcompare(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size)
{
    const lh_usize_t n = lh_math_min(lhs_size, rhs_size);
    const lh_ptr l = lh_ptr_add_v(void, lhs, lh_math_sub(lhs_size, n));
    const lh_ptr r = lh_ptr_add_v(void, rhs, lh_math_sub(rhs_size, n));

    return lh_memory_std_rcompare(l, r, n);
}

lh_ptr
lh_memory_set_pattern(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_return_ifn(dst_size, lh_null);
    lh_return_ifn(src_size, lh_null);

    lh_byte_ptr d = lh_ptr_cast(lh_byte_t, dst);
    lh_byte_cptr s = lh_ptr_cast(const lh_byte_t, src);

    if (src_size == 1)
    {
        return lh_memory_std_set(d, lh_ptr_deref(s), dst_size);
    }

    /* Pattern tiles that divide a machine word: peel to alignment at phase 0,
     * then store a pre-repeated word (2- and 4-byte fills on Cortex-M). */
    {
        const lh_usize_t w = sizeof(lh_usize_t);

        if (src_size <= w && (w % src_size) == 0U)
        {
            lh_byte_ptr out = d;
            lh_usize_t n = dst_size;
            lh_usize_t phase = 0;
            lh_byte_t rep_bytes[sizeof(lh_usize_t)];
            lh_usize_t rep;

            for (lh_usize_t i = 0U; i < w; ++i)
            {
                rep_bytes[i] = s[i % src_size];
            }
            lh_memory_std_copy(lh_addr_of(rep), rep_bytes, w);

            while (n > 0U &&
                   ((lh_ptr_to_uaddr(out) & (w - 1U)) != 0U || phase != 0U))
            {
                lh_ptr_deref(out++) = s[phase];
                phase = lh_math_add_one(phase);
                if (phase == src_size)
                {
                    phase = 0;
                }
                --n;
            }

            while (n >= w)
            {
                lh_memory_word_store(out, rep);
                out += w;
                n -= w;
            }

            while (n > 0U)
            {
                lh_ptr_deref(out++) = s[phase];
                phase = lh_math_add_one(phase);
                if (phase == src_size)
                {
                    phase = 0;
                }
                --n;
            }

            return d + dst_size;
        }
    }

    /* Doubling, as PHP's str_repeat does: write the pattern once, then copy the
     * filled prefix onto what follows, twice as much each time — about log2 of
     * dst_size / src_size copies instead of one per repeat (a wide-char fill of
     * 4KB was 2048 calls). The filled length stays a multiple of src_size until
     * the last, partial chunk, so every copy keeps the period; source and
     * destination never overlap. */
    {
        lh_usize_t filled = lh_math_min(src_size, dst_size);
        lh_memory_std_copy(d, s, filled);
        while (filled < dst_size)
        {
            const lh_usize_t chunk = lh_math_min(filled, dst_size - filled);
            lh_memory_std_copy(d + filled, d, chunk);
            filled += chunk;
        }
    }

    return d + dst_size;
}

lh_bool_t
lh_memory_is_unbounded(const lh_ptr lhs, lh_usize_t lhs_size)
{
    return lhs_size == lh_memory_scan_bound(lhs);
}

lh_bool_t
lh_memory_is_equal(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t size)
{
    return lh_null_eq(lh_memory_std_compare(lhs, rhs, size));
}

const lh_ptr
lh_memory_find_byte_by_step(const lh_ptr lhs, lh_usize_t lhs_size, lh_byte_t byte, lh_usize_t step)
{
    lh_byte_cptr base = lh_ptr_cast(const lh_byte_t, lhs);
    /* Offsets, not an end pointer: lhs_size may be the address-space bound. */
    for (lh_usize_t off = 0; lh_math_ge(lh_memory_size_rest(lhs_size, off), 1);
         off = lh_math_add(off, step))
    {
        lh_byte_cptr cand = lh_ptr_add_by_offset(const lh_byte_t, base, off);
        if (lh_ptr_deref(cand) == byte)
        {
            return cand;
        }
    }
    return lh_null;
}

const lh_ptr
lh_memory_find_unit_by_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr unit,
                            lh_usize_t unit_size)
{
    lh_byte_cptr base = lh_ptr_cast(const lh_byte_t, lhs);
    for (lh_usize_t off = 0; lh_math_ge(lh_memory_size_rest(lhs_size, off), unit_size);
         off = lh_math_add(off, unit_size))
    {
        lh_byte_cptr cand = lh_ptr_add_by_offset(const lh_byte_t, base, off);
        if (lh_memory_is_equal(cand, unit, unit_size))
        {
            return cand;
        }
    }
    return lh_null;
}

const lh_ptr
lh_memory_find_unit(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr unit, lh_usize_t unit_size)
{
    lh_byte_cptr base = lh_ptr_cast(const lh_byte_t, lhs);
    lh_usize_t off = 0;
    /* The byte search of the kernel; a hit off the unit grid resumes at the next unit. */
    while (lh_math_ge(lh_memory_size_rest(lhs_size, off), unit_size))
    {
        lh_byte_cptr hit = lh_memory_std_find(lh_ptr_add_by_offset(const lh_byte_t, base, off),
                                              lh_memory_size_rest(lhs_size, off), unit, unit_size);
        lh_return_if(lh_null_eq(hit), lh_null);
        const lh_usize_t at = lh_addr_diff(lh_ptr_to_uaddr(hit), lh_ptr_to_uaddr(base));
        lh_return_if(lh_math_is_zero(at % unit_size), hit);
        off = lh_math_add(at - at % unit_size, unit_size);
    }
    return lh_null;
}

const lh_ptr
lh_memory_find_needle_by_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs,
                              lh_usize_t rhs_size, lh_usize_t step)
{
    lh_byte_cptr base = lh_ptr_cast(const lh_byte_t, lhs);
    const lh_byte_t first = *lh_ptr_cast(const lh_byte_t, rhs);
    /* The first byte is tested in place; the full compare runs only where it matches. */
    for (lh_usize_t off = 0; lh_math_ge(lh_memory_size_rest(lhs_size, off), rhs_size);
         off = lh_math_add(off, step))
    {
        lh_byte_cptr cand = lh_ptr_add_by_offset(const lh_byte_t, base, off);
        if (lh_ptr_deref(cand) == first &&
            !lh_memory_compare(cand, lh_memory_size_rest(lhs_size, off), rhs, rhs_size))
        {
            return cand;
        }
    }
    return lh_null;
}

const lh_ptr
lh_memory_find_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size,
                    lh_usize_t step)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);
    lh_return_ifn(lhs_size, lh_null);
    lh_return_ifn(rhs_size, lh_null);
    lh_return_ifn(step, lh_null);
    if (!lh_memory_is_unbounded(lhs, lhs_size))
    {
        /* A real length: the kernel's search, on a byte grid or a unit grid. */
        lh_return_if(step == 1U, lh_memory_std_find(lhs, lhs_size, rhs, rhs_size));
        lh_return_if(rhs_size == step, lh_memory_find_unit(lhs, lhs_size, rhs, rhs_size));
    }
    /* A sentinel scan reads nothing past the match. */
    lh_return_if(rhs_size == 1U,
                 lh_memory_find_byte_by_step(lhs, lhs_size, *lh_ptr_cast(const lh_byte_t, rhs),
                                             step));
    lh_return_if(rhs_size == step, lh_memory_find_unit_by_step(lhs, lhs_size, rhs, rhs_size));
    return lh_memory_find_needle_by_step(lhs, lhs_size, rhs, rhs_size, step);
}

lh_usize_t
lh_memory_get_last_step_offset(lh_usize_t lhs_size, lh_usize_t rhs_size, lh_usize_t step)
{
    const lh_usize_t max_start = lh_math_sub(lhs_size, rhs_size);
    /* Power-of-two step (byte, wchar): mask. Otherwise the usual truncate-to-grid. */
    return lh_math_is_power_of_two(step) ? (max_start & ~(step - 1U)) : (max_start / step) * step;
}

const lh_ptr
lh_memory_rfind_needle_by_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs,
                               lh_usize_t rhs_size, lh_usize_t step)
{
    lh_return_if(lhs_size < rhs_size, lh_null);
    lh_byte_cptr base = lh_ptr_cast(const lh_byte_t, lhs);
    const lh_byte_t first = *lh_ptr_cast(const lh_byte_t, rhs);
    lh_usize_t off = lh_memory_get_last_step_offset(lhs_size, rhs_size, step);
    for (;;)
    {
        lh_byte_cptr cand = lh_ptr_add_by_offset(const lh_byte_t, base, off);
        if (lh_ptr_deref(cand) == first && lh_memory_is_equal(cand, rhs, rhs_size))
        {
            return cand;
        }
        lh_return_if(off < step, lh_null);
        off -= step;
    }
}

const lh_ptr
lh_memory_rfind_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size,
                     lh_usize_t step)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);
    lh_return_ifn(lhs_size, lh_null);
    lh_return_ifn(rhs_size, lh_null);
    lh_return_ifn(step, lh_null);
    /* A real length on a byte grid: the kernel's reverse search. */
    lh_return_if(step == 1U && !lh_memory_is_unbounded(lhs, lhs_size),
                 lh_memory_std_rfind(lhs, lhs_size, rhs, rhs_size));
    return lh_memory_rfind_needle_by_step(lhs, lhs_size, rhs, rhs_size, step);
}

const lh_ptr
lh_memory_rfind(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size)
{
    return lh_memory_rfind_step(lhs, lhs_size, rhs, rhs_size, 1);
}

const lh_ptr
lh_memory_find(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size)
{
    return lh_memory_find_step(lhs, lhs_size, rhs, rhs_size, LH_UCHAR_T_SIZE);
}

const lh_ptr
lh_memory_scan_step(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t rhs_size, lh_usize_t step)
{
    return lh_memory_find_step(lhs, lh_memory_scan_bound(lhs), rhs, rhs_size, step);
}

const lh_ptr
lh_memory_scan(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t rhs_size)
{
    return lh_memory_scan_step(lhs, rhs, rhs_size, LH_UCHAR_T_SIZE);
}
