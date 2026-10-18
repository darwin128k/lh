#include <lh/size/ops.h>
#include <lh/size/limits.h>
#include <lh/assert.h>
#include <lh/math.h>

lh_usize_t
lh_size_add(lh_usize_t a, lh_usize_t b)
{
    lh_runtime_check_if(lh_math_gt(a, lh_math_sub(LH_USIZE_T_MAX, b)),
                        lh_runtime_error_code_overflow);
    return lh_math_add(a, b);
}
