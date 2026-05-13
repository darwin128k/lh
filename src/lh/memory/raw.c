#include <lh/memory.h>
#include <lh/memory/raw.h>
#include <lh/runtime/assert.h>

lh_ptr
lh_memory_raw_copy(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    lh_runtime_assert_ref(dst_end);
    lh_runtime_assert_ref(src_end);

    lh_uaddr_t dst_size = lh_ptr_udiff(dst_end, dst);
    lh_uaddr_t src_size = lh_ptr_udiff(src_end, src);
    return lh_memory_copy(dst, dst_size, src, src_size);
}

lh_ptr
lh_memory_raw_copy_rev(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    lh_runtime_assert_ref(dst_end);
    lh_runtime_assert_ref(src_end);

    lh_uaddr_t dst_size = lh_ptr_udiff(dst_end, dst);
    lh_uaddr_t src_size = lh_ptr_udiff(src_end, src);
    return lh_memory_copy_rev(dst, dst_size, src, src_size);
}

lh_ptr
lh_memory_raw_rcopy(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    lh_runtime_assert_ref(dst_end);
    lh_runtime_assert_ref(src_end);

    lh_uaddr_t dst_size = lh_ptr_udiff(dst_end, dst);
    lh_uaddr_t src_size = lh_ptr_udiff(src_end, src);
    return lh_memory_rcopy(dst, dst_size, src, src_size);
}

lh_ptr
lh_memory_raw_move(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    lh_runtime_assert_ref(dst_end);
    lh_runtime_assert_ref(src_end);

    lh_uaddr_t dst_size = lh_ptr_udiff(dst_end, dst);
    lh_uaddr_t src_size = lh_ptr_udiff(src_end, src);
    return lh_memory_move(dst, dst_size, src, src_size);
}

lh_ptr
lh_memory_raw_set(lh_ptr dst, const lh_ptr dst_end, lh_uchar_t value)
{
    lh_runtime_assert_ref(dst_end);

    const lh_uaddr_t n = lh_ptr_udiff(dst_end, dst);
    return lh_memory_set(dst, n, value);
}

const lh_ptr
lh_memory_raw_compare(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                      const lh_ptr rhs_end)
{
    lh_runtime_assert_ref(lhs_end);
    lh_runtime_assert_ref(rhs_end);

    lh_uaddr_t lhs_size = lh_ptr_udiff(lhs_end, lhs);
    lh_uaddr_t rhs_size = lh_ptr_udiff(rhs_end, rhs);
    return lh_memory_compare(lhs, lhs_size, rhs, rhs_size);
}

const lh_ptr
lh_memory_raw_rcompare(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                       const lh_ptr rhs_end)
{
    lh_runtime_assert_ref(lhs_end);
    lh_runtime_assert_ref(rhs_end);

    lh_uaddr_t lhs_size = lh_ptr_udiff(lhs_end, lhs);
    lh_uaddr_t rhs_size = lh_ptr_udiff(rhs_end, rhs);
    return lh_memory_rcompare(lhs, lhs_size, rhs, rhs_size);
}

const lh_ptr
lh_memory_raw_find(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs, const lh_ptr rhs_end)
{
    lh_runtime_assert_ref(lhs_end);
    lh_runtime_assert_ref(rhs_end);

    lh_uaddr_t lhs_size = lh_ptr_udiff(lhs_end, lhs);
    lh_uaddr_t rhs_size = lh_ptr_udiff(rhs_end, rhs);
    return lh_memory_find(lhs, lhs_size, rhs, rhs_size);
}

const lh_ptr
lh_memory_raw_rfind(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs, const lh_ptr rhs_end)
{
    lh_runtime_assert_ref(lhs_end);
    lh_runtime_assert_ref(rhs_end);

    lh_uaddr_t lhs_size = lh_ptr_udiff(lhs_end, lhs);
    lh_uaddr_t rhs_size = lh_ptr_udiff(rhs_end, rhs);
    return lh_memory_rfind(lhs, lhs_size, rhs, rhs_size);
}

lh_ptr
lh_memory_raw_set_pattern(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end)
{
    lh_runtime_assert_ref(dst_end);
    lh_runtime_assert_ref(src_end);

    lh_uaddr_t dst_size = lh_ptr_udiff(dst_end, dst);
    lh_uaddr_t src_size = lh_ptr_udiff(src_end, src);

    return lh_memory_set_pattern(dst, dst_size, src, src_size);
}
