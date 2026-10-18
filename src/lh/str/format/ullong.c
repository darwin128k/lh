#include <lh/str/format/ullong.h>
#include <lh/char/map.h>
#include <lh/math.h>
#include <lh/assert.h>
#include <lh/memory/std.h>
#include <lh/str/format/uint.h>

/* 10^9 fits in ::lh_uint_t and is nine decimal digits — one call of
   ::lh_str_ptr_format_uint per chunk, not a second digit loop. */
#define LH_STR_FORMAT_ULLONG_CHUNK 1000000000ULL
#define LH_STR_FORMAT_ULLONG_CHUNK_DIGITS 9U
#define LH_STR_FORMAT_ULLONG_PARTS_MAX 3U

lh_usize_t
lh_str_ptr_format_ullong(lh_ullong_t value, lh_str_ptr str, lh_usize_t str_size)
{
    lh_uint_t parts[LH_STR_FORMAT_ULLONG_PARTS_MAX];
    lh_usize_t count = 0;
    lh_usize_t written = 0;
    lh_ullong_t x = value;
    lh_usize_t i;

    lh_assert_runtime_ref(str);

    do
    {
        parts[count] = lh_cast_static(lh_uint_t, lh_math_mod(x, LH_STR_FORMAT_ULLONG_CHUNK));
        count = lh_math_add_one(count);
        x = lh_math_div(x, LH_STR_FORMAT_ULLONG_CHUNK);
    } while (lh_math_gt(x, 0U) && lh_math_lt(count, LH_STR_FORMAT_ULLONG_PARTS_MAX));

    written = lh_str_ptr_format_uint(parts[lh_math_sub_one(count)], str, str_size);
    if (lh_math_is_zero(written))
    {
        return 0;
    }

    i = count;
    while (lh_math_gt(i, 1U))
    {
        lh_char_t digits[LH_STR_FORMAT_ULLONG_CHUNK_DIGITS];
        lh_usize_t n;
        lh_usize_t pad;

        i = lh_math_sub_one(i);
        n = lh_str_ptr_format_uint(parts[lh_math_sub_one(i)], digits, LH_STR_FORMAT_ULLONG_CHUNK_DIGITS);
        if (lh_math_is_zero(n))
        {
            return 0;
        }
        pad = lh_math_sub(LH_STR_FORMAT_ULLONG_CHUNK_DIGITS, n);
        if (lh_math_gt(lh_math_add(written, LH_STR_FORMAT_ULLONG_CHUNK_DIGITS), str_size))
        {
            return 0;
        }
        lh_memory_std_set(lh_ptr_add_by_offset_unsafe(lh_char_t, str, written),
                          lh_cast_static(lh_byte_t, lh_char_map_digit_0), pad);
        lh_memory_std_copy(lh_ptr_add_by_offset_unsafe(lh_char_t, str, lh_math_add(written, pad)), digits,
                           n);
        written = lh_math_add(written, LH_STR_FORMAT_ULLONG_CHUNK_DIGITS);
    }
    return written;
}
