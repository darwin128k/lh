#include <lh/memory.h>
#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/math.h>

lh_usize_t
lh_memory_raw_get_size(const lh_ptr begin, const lh_ptr end)
{
    lh_assert_runtime_ref(end);
    return lh_ptr_udiff(end, begin);
}

lh_ptr
lh_memory_raw_copy(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_std_copy(dst, src,
                              lh_math_min(lh_memory_raw_get_size(dst, dst_end),
                                          lh_memory_raw_get_size(src, src_end)));
}

lh_ptr
lh_memory_raw_copy_rev(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_std_copy_rev(dst, src,
                                  lh_math_min(lh_memory_raw_get_size(dst, dst_end),
                                              lh_memory_raw_get_size(src, src_end)));
}

lh_ptr
lh_memory_raw_rcopy(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_std_rcopy(dst, src,
                               lh_math_min(lh_memory_raw_get_size(dst, dst_end),
                                           lh_memory_raw_get_size(src, src_end)));
}

lh_ptr
lh_memory_raw_move(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_std_move(dst, src,
                              lh_math_min(lh_memory_raw_get_size(dst, dst_end),
                                          lh_memory_raw_get_size(src, src_end)));
}

lh_ptr
lh_memory_raw_set(lh_ptr dst, const lh_ptr dst_end, lh_byte_t value)
{
    return lh_memory_std_set(dst, value, lh_memory_raw_get_size(dst, dst_end));
}

const lh_ptr
lh_memory_raw_compare(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                      const lh_ptr rhs_end)
{
    return lh_memory_std_compare(lhs, rhs,
                                 lh_math_min(lh_memory_raw_get_size(lhs, lhs_end),
                                             lh_memory_raw_get_size(rhs, rhs_end)));
}

const lh_ptr
lh_memory_raw_rcompare(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                       const lh_ptr rhs_end)
{
    {
        const lh_usize_t lhs_size = lh_memory_raw_get_size(lhs, lhs_end);
        const lh_usize_t rhs_size = lh_memory_raw_get_size(rhs, rhs_end);
        const lh_usize_t n = lh_math_min(lhs_size, rhs_size);

        return lh_memory_std_rcompare(
            lh_ptr_add_by_offset_unsafe(const lh_void, lhs, lh_math_sub(lhs_size, n)),
            lh_ptr_add_by_offset_unsafe(const lh_void, rhs, lh_math_sub(rhs_size, n)), n);
    }
}

const lh_ptr
lh_memory_raw_find(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs, const lh_ptr rhs_end)
{
    return lh_memory_std_find(lhs, lh_memory_raw_get_size(lhs, lhs_end), rhs,
                              lh_memory_raw_get_size(rhs, rhs_end));
}

const lh_ptr
lh_memory_raw_rfind(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs, const lh_ptr rhs_end)
{
    return lh_memory_std_rfind(lhs, lh_memory_raw_get_size(lhs, lhs_end), rhs,
                               lh_memory_raw_get_size(rhs, rhs_end));
}

lh_ptr
lh_memory_raw_set_pattern(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_set_pattern(dst, lh_memory_raw_get_size(dst, dst_end), src,
                                 lh_memory_raw_get_size(src, src_end));
}
