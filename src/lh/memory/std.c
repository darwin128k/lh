#include <lh/memory/std.h>
#include <lh/runtime/check/ref.h>
#include <lh/util/algorithm.h>

lh_ptr
lh_memory_std_copy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_runtime_check_ref(dst);
    lh_runtime_check_ref(src);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);
    lh_algorithm_copy(lh_uchar_t, dst, src, n);

    return end;
}

lh_ptr
lh_memory_std_copy_rev(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_runtime_check_ref(dst);
    lh_runtime_check_ref(src);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);
    lh_algorithm_copy_rev(lh_uchar_t, dst, src, n);

    return end;
}

lh_ptr
lh_memory_std_rcopy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_runtime_check_ref(dst);
    lh_runtime_check_ref(src);

    lh_algorithm_rcopy(lh_uchar_t, dst, src, n);
    return dst;
}

lh_ptr
lh_memory_std_move(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    const lh_ptr src_end = lh_ptr_add_by_offset(lh_void, src, n);
    if (lh_ptr_is_backward_copy(dst, src, src_end))
    {
        lh_memory_std_rcopy(dst, src, n);
        return lh_ptr_add_by_offset(lh_void, dst, n);
    }
    return lh_memory_std_copy(dst, src, n);
}

lh_ptr
lh_memory_std_set(lh_ptr dst, lh_uchar_t val, lh_usize_t n)
{
    lh_runtime_check_ref(dst);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);
    lh_algorithm_set(lh_uchar_t, dst, val, n);
    return end;
}

const lh_ptr
lh_memory_std_compare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_runtime_check_ref(lhs);
    lh_runtime_check_ref(rhs);

    lh_algorithm_compare(lh_uchar_t, lhs, rhs, n);
    return lh_null;
}

const lh_ptr
lh_memory_std_rcompare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_runtime_check_ref(lhs);
    lh_runtime_check_ref(rhs);

    lh_algorithm_rcompare(lh_uchar_t, lhs, rhs, n);
    return lh_null;
}