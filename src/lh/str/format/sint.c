#include <lh/str/format/sint.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/str/format/uint.h>
#include <lh/util/math.h>

lh_usize_t
lh_str_ptr_format_sint(lh_sint_t value, lh_str_ptr str, lh_usize_t str_size)
{
    lh_bool_t is_negative = lh_math_is_negative(value) ? lh_bool_true : lh_bool_false;
    lh_uint_t magnitude;

    lh_assert_runtime_ref(str);

    if (!is_negative)
    {
        return lh_str_ptr_format_uint(lh_cast_static(lh_uint_t, value), str, str_size);
    }

    /* Unsigned wraparound: correct even for LH_SINT_T_MIN, where -value would overflow. */
    magnitude = 0U - lh_cast_static(lh_uint_t, value);

    if (str_size == 0U)
    {
        return 0;
    }
    str[0] = '-';

    {
        lh_usize_t written = lh_str_ptr_format_uint(magnitude, str + 1, str_size - 1U);
        if (written == 0U)
        {
            return 0;
        }
        return written + 1U;
    }
}
