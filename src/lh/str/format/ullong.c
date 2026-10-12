#include <lh/str/format/ullong.h>
#include <lh/assert.h>
#include <lh/byte.h>
#include <lh/char.h>
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
        parts[count++] = (lh_uint_t)(x % LH_STR_FORMAT_ULLONG_CHUNK);
        x /= LH_STR_FORMAT_ULLONG_CHUNK;
    } while (x > 0U && count < LH_STR_FORMAT_ULLONG_PARTS_MAX);

    written = lh_str_ptr_format_uint(parts[count - 1U], str, str_size);
    if (written == 0U)
    {
        return 0;
    }

    i = count;
    while (i > 1U)
    {
        lh_char_t digits[LH_STR_FORMAT_ULLONG_CHUNK_DIGITS];
        lh_usize_t n;
        lh_usize_t pad;

        --i;
        n = lh_str_ptr_format_uint(parts[i - 1U], digits, LH_STR_FORMAT_ULLONG_CHUNK_DIGITS);
        if (n == 0U)
        {
            return 0;
        }
        pad = LH_STR_FORMAT_ULLONG_CHUNK_DIGITS - n;
        if (written + LH_STR_FORMAT_ULLONG_CHUNK_DIGITS > str_size)
        {
            return 0;
        }
        lh_memory_std_set(str + written, (lh_byte_t)'0', pad);
        lh_memory_std_copy(str + written + pad, digits, n);
        written += LH_STR_FORMAT_ULLONG_CHUNK_DIGITS;
    }
    return written;
}
