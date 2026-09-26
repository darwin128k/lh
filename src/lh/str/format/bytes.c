#include <lh/str/format/bytes.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/char/xdigit.h>
#include <lh/size.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

/* One dump line for @p count bytes: "hhhhhhhh: " + "xx " * count + "\n". */
LH_ATTRIBUTE_STATIC
lh_usize_t
lh_str_ptr_format_bytes_hex_dump_line_size(lh_usize_t count)
{
    return lh_math_add(lh_math_add(LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS, 2U),
                       lh_math_add_one(lh_math_mul(count, 3U)));
}

lh_usize_t
lh_str_ptr_format_bytes_hex_dump_size(lh_usize_t data_size)
{
    const lh_usize_t full_lines = lh_math_div(data_size, LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH);
    const lh_usize_t last = lh_math_mod(data_size, LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH);
    const lh_usize_t full_size =
        lh_str_ptr_format_bytes_hex_dump_line_size(LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH);

    if (lh_math_gt(full_lines, lh_math_div(lh_math_sub(LH_USIZE_T_MAX, full_size), full_size)))
    {
        return 0U;
    }
    return lh_math_add(lh_math_mul(full_lines, full_size),
                       lh_math_is_zero(last) ? 0U
                                             : lh_str_ptr_format_bytes_hex_dump_line_size(last));
}

lh_usize_t
lh_str_ptr_format_bytes_hex(const lh_ptr data, lh_usize_t data_size, lh_bool_t uppercase,
                            lh_str_ptr str, lh_usize_t str_size)
{
    const lh_uchar_t *bytes;
    lh_usize_t i;
    lh_usize_t needed;

    lh_assert_runtime_ref(str);
    if (data_size == 0)
    {
        return 0;
    }
    lh_assert_runtime_ref(data);

    if (data_size > (LH_USIZE_T_MAX / 2U))
    {
        return 0;
    }
    needed = data_size * 2U;
    if (needed > str_size)
    {
        return 0;
    }

    bytes = lh_ptr_rcast(const lh_uchar_t, data);
    for (i = 0; i < data_size; ++i)
    {
        str[i * 2U] = lh_char_from_xdigit(lh_cast_static(lh_uchar_t, (bytes[i] >> 4)), uppercase);
        str[i * 2U + 1U] = lh_char_from_xdigit(lh_cast_static(lh_uchar_t, (bytes[i] & 0x0FU)), uppercase);
    }
    return needed;
}

lh_usize_t
lh_str_ptr_format_bytes_hex_dump(const lh_ptr data, lh_usize_t data_size, lh_bool_t uppercase,
                                 lh_str_ptr str, lh_usize_t str_size)
{
    const lh_uchar_t *bytes;
    lh_usize_t needed;
    lh_usize_t pos;
    lh_usize_t offset;

    lh_assert_runtime_ref(str);
    if (data_size == 0)
    {
        return 0;
    }
    lh_assert_runtime_ref(data);

    needed = lh_str_ptr_format_bytes_hex_dump_size(data_size);
    if (needed == 0 || needed > str_size)
    {
        return 0;
    }

    bytes = lh_ptr_rcast(const lh_uchar_t, data);
    pos = 0;
    for (offset = 0; offset < data_size; offset += LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH)
    {
        lh_usize_t d;
        lh_usize_t count;
        lh_usize_t j;
        lh_u32_t rest = lh_cast_static(lh_u32_t, offset);

        for (d = LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS; d > 0U; --d)
        {
            str[pos + d - 1U] = lh_char_from_xdigit(lh_cast_static(lh_uchar_t, (rest & 0x0FU)), uppercase);
            rest >>= 4;
        }
        pos += LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS;
        str[pos++] = ':';
        str[pos++] = ' ';

        count = data_size - offset;
        if (count > LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH)
        {
            count = LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH;
        }
        for (j = 0; j < count; ++j)
        {
            lh_uchar_t b = bytes[offset + j];
            str[pos++] = lh_char_from_xdigit(lh_cast_static(lh_uchar_t, (b >> 4)), uppercase);
            str[pos++] = lh_char_from_xdigit(lh_cast_static(lh_uchar_t, (b & 0x0FU)), uppercase);
            str[pos++] = ' ';
        }
        str[pos++] = '\n';
    }

    return pos;
}
