#include <lh/memory/raw.h>
#include <lh/memory.h>
#include <lh/attribute/static.h>
#include <lh/assert.h>

LH_ATTRIBUTE_STATIC
lh_uaddr_t
lh_memory_raw_get_size(const lh_ptr begin, const lh_ptr end)
{
    lh_assert_runtime_ref(end);
    return lh_ptr_udiff(end, begin);
}

lh_ptr
lh_memory_raw_copy(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_copy(dst, lh_memory_raw_get_size(dst, dst_end), src,
                          lh_memory_raw_get_size(src, src_end));
}

lh_ptr
lh_memory_raw_copy_rev(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_copy_rev(dst, lh_memory_raw_get_size(dst, dst_end), src,
                              lh_memory_raw_get_size(src, src_end));
}

lh_ptr
lh_memory_raw_rcopy(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_rcopy(dst, lh_memory_raw_get_size(dst, dst_end), src,
                           lh_memory_raw_get_size(src, src_end));
}

lh_ptr
lh_memory_raw_move(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_move(dst, lh_memory_raw_get_size(dst, dst_end), src,
                          lh_memory_raw_get_size(src, src_end));
}

lh_ptr
lh_memory_raw_set(lh_ptr dst, const lh_ptr dst_end, lh_uchar_t value)
{
    return lh_memory_set(dst, lh_memory_raw_get_size(dst, dst_end), value);
}

const lh_ptr
lh_memory_raw_compare(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                      const lh_ptr rhs_end)
{
    return lh_memory_compare(lhs, lh_memory_raw_get_size(lhs, lhs_end), rhs,
                             lh_memory_raw_get_size(rhs, rhs_end));
}

const lh_ptr
lh_memory_raw_rcompare(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                       const lh_ptr rhs_end)
{
    return lh_memory_rcompare(lhs, lh_memory_raw_get_size(lhs, lhs_end), rhs,
                              lh_memory_raw_get_size(rhs, rhs_end));
}

const lh_ptr
lh_memory_raw_find(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs, const lh_ptr rhs_end)
{
    return lh_memory_find(lhs, lh_memory_raw_get_size(lhs, lhs_end), rhs,
                          lh_memory_raw_get_size(rhs, rhs_end));
}

const lh_ptr
lh_memory_raw_rfind(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs, const lh_ptr rhs_end)
{
    return lh_memory_rfind(lhs, lh_memory_raw_get_size(lhs, lhs_end), rhs,
                           lh_memory_raw_get_size(rhs, rhs_end));
}

lh_ptr
lh_memory_raw_set_pattern(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    return lh_memory_set_pattern(dst, lh_memory_raw_get_size(dst, dst_end), src,
                                 lh_memory_raw_get_size(src, src_end));
}
