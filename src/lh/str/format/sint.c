#include <lh/str/format/sint.h>
#include <lh/assert.h>
#include <lh/char/map.h>
#include <lh/str/format/uint.h>
#include <lh/math.h>

lh_usize_t
lh_str_ptr_format_sint(lh_sint_t value, lh_str_ptr str, lh_usize_t str_size)
{
    lh_usize_t written;

    lh_assert_runtime_ref(str);

    if (!lh_math_is_negative(value))
    {
        return lh_str_ptr_format_uint(lh_cast_static(lh_uint_t, value), str, str_size);
    }
    if (lh_math_is_zero(str_size))
    {
        return 0U;
    }
    str[0] = lh_char_map_minus;
    /* Unsigned wraparound: correct even for LH_SINT_T_MIN, where -value would overflow. */
    written = lh_str_ptr_format_uint(lh_math_sub(0U, lh_cast_static(lh_uint_t, value)),
                                     lh_ptr_add_by_offset_unsafe(lh_char_t, str, 1U),
                                     lh_math_sub_one(str_size));
    return lh_math_is_zero(written) ? 0U : lh_math_add_one(written);
}
