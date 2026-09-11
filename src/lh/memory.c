#include <lh/memory.h>
#include <lh/memory/std.h>
#include <lh/util/algorithm.h>
#include <lh/util/return.h>
#include <lh/assert.h>

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
lh_memory_set(lh_ptr dst, lh_usize_t size, lh_uchar_t val)
{
    return lh_memory_std_set(dst, val, size);
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
    const lh_ptr l = lh_ptr_add(void, lhs, lh_math_sub(lhs_size, n));
    const lh_ptr r = lh_ptr_add(void, rhs, lh_math_sub(rhs_size, n));

    return lh_memory_std_rcompare(l, r, n);
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

    const lh_uchar_t *base = lh_ptr_cast(const lh_uchar_t, lhs);
    const lh_uchar_t *end = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, lhs_size);

    if (rhs_size == 1)
    {
        /* Single-element needle (lh_str_ptr_find_of_char, lh_str_view_find_char, ... all
         * bottom out here with rhs_size 1) — read it once and compare directly instead of
         * paying a lh_memory_compare call (itself calling into lh_memory_std_compare) at
         * every one of up to lhs_size candidate positions. */
        const lh_uchar_t needle = *lh_ptr_cast(const lh_uchar_t, rhs);

        if (step == 1)
        {
            /* Contiguous scan: check LH_ALGORITHM_COMPARE_BLOCK bytes at a time with no
             * branch inside the block (same technique as lh_algorithm_compare) so the
             * compiler can auto-vectorize the common no-match-yet case; only the block
             * that actually contains a hit pays for a per-byte branch. */
            const lh_uchar_t *cand = base;
            while (cand + LH_ALGORITHM_COMPARE_BLOCK <= end)
            {
                lh_bool_t block_hit = lh_bool_false;
                lh_usize_t block_i;
                for (block_i = 0; block_i < LH_ALGORITHM_COMPARE_BLOCK; ++block_i)
                {
                    block_hit = (lh_bool_t)(block_hit | (cand[block_i] == needle));
                }
                if (block_hit)
                {
                    break;
                }
                cand += LH_ALGORITHM_COMPARE_BLOCK;
            }
            for (; cand + 1 <= end; ++cand)
            {
                if (*cand == needle)
                {
                    return cand;
                }
            }
            return lh_null;
        }

        for (const lh_uchar_t *cand = base; cand + 1 <= end; cand += step)
        {
            if (*cand == needle)
            {
                return cand;
            }
        }
        return lh_null;
    }

    for (const lh_uchar_t *cand = base; cand + rhs_size <= end; cand += step)
    {
        if (!lh_memory_compare(cand, (end - cand), rhs, rhs_size))
        {
            return cand;
        }
    }
    return lh_null;
}

const lh_ptr
lh_memory_find(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size)
{
    return lh_memory_find_step(lhs, lhs_size, rhs, rhs_size, LH_UCHAR_T_SIZE);
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

    const lh_uchar_t *base = lh_ptr_cast(const lh_uchar_t, lhs);
    if (lhs_size < rhs_size)
    {
        return lh_null;
    }

    const lh_usize_t max_start = lhs_size - rhs_size;
    lh_usize_t off = (max_start / step) * step;

    if (rhs_size == 1)
    {
        /* See the matching fast path in lh_memory_find_step. */
        const lh_uchar_t needle = *lh_ptr_cast(const lh_uchar_t, rhs);
        for (;;)
        {
            const lh_uchar_t *cand = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, off);
            if (*cand == needle)
            {
                return cand;
            }
            if (off < step)
            {
                break;
            }
            off -= step;
        }
        return lh_null;
    }

    for (;;)
    {
        const lh_uchar_t *cand = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, off);
        if (!lh_memory_compare(cand, lhs_size - off, rhs, rhs_size))
        {
            return cand;
        }
        if (off < step)
        {
            break;
        }
        off -= step;
    }
    return lh_null;
}

const lh_ptr
lh_memory_rfind(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size)
{
    return lh_memory_rfind_step(lhs, lhs_size, rhs, rhs_size, 1);
}

lh_ptr
lh_memory_set_pattern(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_return_ifn(dst_size, lh_null);
    lh_return_ifn(src_size, lh_null);

    lh_uchar_t *d = lh_ptr_cast(lh_uchar_t, dst);
    const lh_uchar_t *s = lh_ptr_cast(const lh_uchar_t, src);

    lh_uchar_t *cur = d;
    for (lh_usize_t i = 0; i < dst_size; i += src_size)
    {
        lh_usize_t remaining = dst_size - i;
        lh_usize_t copy_size = (remaining < src_size) ? remaining : src_size;

        for (lh_usize_t j = 0; j < copy_size; j++)
        {
            d[i + j] = s[j];
        }

        cur = d + i + copy_size;
    }

    return cur;
}
