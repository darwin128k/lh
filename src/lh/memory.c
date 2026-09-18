#include <lh/memory.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/bool.h>
#include <lh/config.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

#define LH_MEMORY_SCAN_BLOCK (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK))

LH_ATTRIBUTE_STATIC
lh_usize_t
lh_memory_size_rest(lh_usize_t size, lh_usize_t off)
{
    return lh_math_le(off, size) ? lh_math_sub(size, off) : 0;
}

LH_ATTRIBUTE_STATIC
lh_usize_t
lh_memory_scan_bound(const lh_ptr ptr)
{
    return lh_addr_diff(LH_USIZE_T_MAX, lh_ptr_to_uaddr(ptr));
}

LH_ATTRIBUTE_STATIC
lh_bool_t
lh_memory_bytes_eq(const lh_uchar_t *lhs, const lh_uchar_t *rhs, lh_usize_t n)
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
    lh_usize_t off = 0;

    if (rhs_size == 1)
    {
        /* Single-byte needle (lh_str_ptr_find_of_char, lh_str_view_find_char, ... all
         * bottom out here with rhs_size 1) — read it once and compare directly instead of
         * paying a lh_memory_compare call (itself calling into lh_memory_std_compare) at
         * every one of up to lhs_size candidate positions. */
        const lh_uchar_t needle = *lh_ptr_cast(const lh_uchar_t, rhs);

        if (step == 1)
        {
            /* Offset walk, not an end pointer: lhs_size may be the remaining address
             * space from lh_memory_scan, and base+lhs_size would wrap. */
            while (lh_math_ge(lh_memory_size_rest(lhs_size, off), LH_MEMORY_SCAN_BLOCK))
            {
                const lh_uchar_t *cand = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, off);
                lh_bool_t block_hit = lh_bool_false;
                lh_usize_t block_i;
                for (block_i = 0; block_i < LH_MEMORY_SCAN_BLOCK; ++block_i)
                {
                    block_hit = lh_cast_static(lh_bool_t, (block_hit | (cand[block_i] == needle)));
                }
                if (block_hit)
                {
                    break;
                }
                off = lh_math_add(off, LH_MEMORY_SCAN_BLOCK);
            }
            for (; lh_math_ge(lh_memory_size_rest(lhs_size, off), 1); off = lh_math_add(off, 1))
            {
                const lh_uchar_t *cand = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, off);
                if (*cand == needle)
                {
                    return cand;
                }
            }
            return lh_null;
        }

        for (; lh_math_ge(lh_memory_size_rest(lhs_size, off), 1); off = lh_math_add(off, step))
        {
            const lh_uchar_t *cand = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, off);
            if (*cand == needle)
            {
                return cand;
            }
        }
        return lh_null;
    }

    if (rhs_size == step)
    {
        /* Fixed-size unit grid (lh_wstr_ptr_find_char, wide NUL scan): compare the
         * unit in-place. When the block size is a multiple of step, reuse the
         * branchless block walk so wchar search hits the same fast path as bytes. */
        const lh_uchar_t *needle = lh_ptr_cast(const lh_uchar_t, rhs);

        if ((LH_MEMORY_SCAN_BLOCK % step) == 0)
        {
            while (lh_math_ge(lh_memory_size_rest(lhs_size, off), LH_MEMORY_SCAN_BLOCK))
            {
                const lh_uchar_t *cand = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, off);
                lh_bool_t block_hit = lh_bool_false;
                lh_usize_t block_i;
                for (block_i = 0; block_i < LH_MEMORY_SCAN_BLOCK; block_i += step)
                {
                    block_hit = lh_cast_static(
                        lh_bool_t, (block_hit | lh_memory_bytes_eq(cand + block_i, needle, rhs_size)));
                }
                if (block_hit)
                {
                    break;
                }
                off = lh_math_add(off, LH_MEMORY_SCAN_BLOCK);
            }
        }

        for (; lh_math_ge(lh_memory_size_rest(lhs_size, off), rhs_size); off = lh_math_add(off, step))
        {
            const lh_uchar_t *cand = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, off);
            if (lh_memory_bytes_eq(cand, needle, rhs_size))
            {
                return cand;
            }
        }
        return lh_null;
    }

    for (; lh_math_ge(lh_memory_size_rest(lhs_size, off), rhs_size); off = lh_math_add(off, step))
    {
        const lh_uchar_t *cand = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, off);
        if (!lh_memory_compare(cand, lh_memory_size_rest(lhs_size, off), rhs, rhs_size))
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
lh_memory_scan_step(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t rhs_size, lh_usize_t step)
{
    return lh_memory_find_step(lhs, lh_memory_scan_bound(lhs), rhs, rhs_size, step);
}

const lh_ptr
lh_memory_scan(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t rhs_size)
{
    return lh_memory_scan_step(lhs, rhs, rhs_size, LH_UCHAR_T_SIZE);
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

        lh_memory_std_copy(d + i, s, copy_size);

        cur = d + i + copy_size;
    }

    return cur;
}
