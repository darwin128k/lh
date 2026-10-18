#include <lh/str/format/put.h>
#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/math.h>

lh_usize_t
lh_str_ptr_format_put(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr text, lh_usize_t n)
{
    lh_assert_runtime_ref(str);
    if (lh_math_gt(n, str_size))
    {
        return 0U;
    }
    lh_memory_std_copy(str, text, n);
    return n;
}
