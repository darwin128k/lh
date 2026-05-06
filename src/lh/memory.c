#include "lh/numeric/limits.h"
#include <lh/memory.h>
#include <lh/memory/std.h>
#include <lh/runtime/check/ref.h>
#include <lh/runtime/return/if.h>

lh_ptr
lh_memory_copy(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size) {
    const lh_usize_t n = lh_math_min(dst_size, src_size);
    return lh_memory_std_copy(dst, src, n);
}

lh_ptr
lh_memory_copy_rev(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size) {
    const lh_usize_t n = lh_math_min(dst_size, src_size);
    return lh_memory_std_copy_rev(dst, src, n);
}

lh_ptr
lh_memory_rcopy(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size) {
    const lh_usize_t n = lh_math_min(dst_size, src_size);
    return lh_memory_std_rcopy(dst, src, n);
}

lh_ptr
lh_memory_move(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size) {
    const lh_usize_t n = lh_math_min(dst_size, src_size);
    return lh_memory_std_move(dst, src, n);
}

lh_ptr
lh_memory_set(lh_ptr dst, lh_usize_t size, lh_uchar_t val) {
    return lh_memory_std_set(dst, val, size);
}

const lh_ptr
lh_memory_compare(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size) {
    const lh_usize_t n = lh_math_min(lhs_size, rhs_size);
    return lh_memory_std_compare(lhs, rhs, n);
}

const lh_ptr
lh_memory_rcompare(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size) {
    const lh_usize_t n = lh_math_min(lhs_size, rhs_size);
    const lh_ptr l = lh_ptr_add(void, lhs, lh_math_sub(lhs_size, n));
    const lh_ptr r = lh_ptr_add(void, rhs, lh_math_sub(rhs_size, n));

    return lh_memory_std_rcompare(l, r, n);
}

const lh_ptr
lh_memory_find_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size,
                    lh_usize_t step) {
    lh_runtime_check_ref(lhs);
    lh_runtime_check_ref(rhs);

    lh_runtime_return_ifn(lhs_size, lh_null);
    lh_runtime_return_ifn(rhs_size, lh_null);
    lh_runtime_return_ifn(step, lh_null);

    const lh_uchar_t *base = lh_ptr_cast(const lh_uchar_t, lhs);
    const lh_uchar_t *end = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, lhs_size);

    for (const lh_uchar_t *cand = base; cand + rhs_size <= end; cand += step) {
        if (!lh_memory_compare(cand, (end - cand), rhs, rhs_size)) {
            return cand;
        }
    }
    return lh_null;
}

const lh_ptr
lh_memory_find(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size) {
    return lh_memory_find_step(lhs, lhs_size, rhs, rhs_size, LH_UCHAR_T_SIZE);
}

const lh_ptr
lh_memory_rfind_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size,
                     lh_usize_t step) {
    lh_runtime_check_ref(lhs);
    lh_runtime_check_ref(rhs);

    lh_runtime_return_ifn(lhs_size, lh_null);
    lh_runtime_return_ifn(rhs_size, lh_null);
    lh_runtime_return_ifn(step, lh_null);

    const lh_uchar_t *base = lh_ptr_cast(const lh_uchar_t, lhs);
    if (lhs_size < rhs_size) {
        return lh_null;
    }

    const lh_usize_t max_start = lhs_size - rhs_size;
    lh_usize_t off = (max_start / step) * step;

    for (;;) {
        const lh_uchar_t *cand = lh_ptr_add_by_offset_unsafe(const lh_uchar_t, base, off);
        if (!lh_memory_compare(cand, lhs_size - off, rhs, rhs_size)) {
            return cand;
        }
        if (off < step) {
            break;
        }
        off -= step;
    }
    return lh_null;
}

const lh_ptr
lh_memory_rfind(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size) {
    return lh_memory_rfind_step(lhs, lhs_size, rhs, rhs_size, 1);
}

lh_ptr
lh_memory_set_pattern(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size) {
    lh_runtime_check_ref(dst);
    lh_runtime_check_ref(src);

    lh_runtime_return_ifn(dst_size, lh_null);
    lh_runtime_return_ifn(src_size, lh_null);

    lh_uchar_t *d = lh_ptr_cast(lh_uchar_t, dst);
    const lh_uchar_t *s = lh_ptr_cast(const lh_uchar_t, src);

    lh_uchar_t *cur = d;
    for (lh_usize_t i = 0; i < dst_size; i += src_size) {
        lh_usize_t remaining = dst_size - i;
        lh_usize_t copy_size = (remaining < src_size) ? remaining : src_size;

        for (lh_usize_t j = 0; j < copy_size; j++) {
            d[i + j] = s[j];
        }

        cur = d + i + copy_size;
    }

    return cur;
}
