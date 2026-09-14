#include <lh/str/format/bytes.h>
#include <lh/assert.h>
#include <lh/char/xdigit.h>
#include <lh/size.h>
#include <lh/util/ptr.h>

static lh_usize_t
lh_str_ptr_format_bytes_hex_dump_needed(lh_usize_t data_size)
{
    lh_usize_t lines;
    lh_usize_t last;
    lh_usize_t per_full;

    if (data_size == 0)
    {
        return 0;
    }

    lines =
        (data_size + LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH - 1U) / LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH;
    last = data_size % LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH;
    if (last == 0)
    {
        last = LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH;
    }
    /* "hhhhhhhh: " + "xx " * count + "\n" */
    per_full = LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS + 2U +
               LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH * 3U + 1U;
    return (lines - 1U) * per_full +
           (LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS + 2U + last * 3U + 1U);
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
        str[i * 2U] = lh_char_from_xdigit((lh_uchar_t)(bytes[i] >> 4), uppercase);
        str[i * 2U + 1U] = lh_char_from_xdigit((lh_uchar_t)(bytes[i] & 0x0FU), uppercase);
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

    needed = lh_str_ptr_format_bytes_hex_dump_needed(data_size);
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
        lh_u32_t rest = (lh_u32_t)offset;

        for (d = LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS; d > 0U; --d)
        {
            str[pos + d - 1U] = lh_char_from_xdigit((lh_uchar_t)(rest & 0x0FU), uppercase);
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
            str[pos++] = lh_char_from_xdigit((lh_uchar_t)(b >> 4), uppercase);
            str[pos++] = lh_char_from_xdigit((lh_uchar_t)(b & 0x0FU), uppercase);
            str[pos++] = ' ';
        }
        str[pos++] = '\n';
    }

    return pos;
}
