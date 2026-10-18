#include <lh/str/format/bytes.h>
#include <lh/assert.h>
#include <lh/bit/ops.h>
#include <lh/byte/ptr.h>
#include <lh/char/map.h>
#include <lh/char/xdigit.h>
#include <lh/math.h>
#include <lh/size/limits.h>

lh_usize_t
lh_str_ptr_format_bytes_hex_dump_line_size(lh_usize_t count)
{
    return lh_math_add(lh_math_add(LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS, 2U),
                       lh_math_add_one(lh_math_mul(count, 3U)));
}

lh_void
lh_str_ptr_format_bytes_hex_byte(lh_byte_t byte, lh_bool_t uppercase, lh_str_ptr str)
{
    str[0] = lh_char_from_xdigit(lh_cast_static(lh_byte_t, lh_bit_shr(byte, 4U)), uppercase);
    str[1] = lh_char_from_xdigit(lh_cast_static(lh_byte_t, lh_bit_and(byte, 0x0FU)), uppercase);
}

lh_void
lh_str_ptr_format_bytes_hex_dump_offset(lh_u32_t offset, lh_bool_t uppercase, lh_str_ptr str)
{
    for (lh_usize_t d = LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS; lh_math_gt(d, 0U);
         d = lh_math_sub_one(d))
    {
        str[lh_math_sub_one(d)] =
            lh_char_from_xdigit(lh_cast_static(lh_byte_t, lh_bit_and(offset, 0x0FU)), uppercase);
        offset = lh_bit_shr(offset, 4U);
    }
}

lh_usize_t
lh_str_ptr_format_bytes_hex(const lh_ptr data, lh_usize_t data_size, lh_bool_t uppercase,
                            lh_str_ptr str, lh_usize_t str_size)
{
    lh_byte_cptr bytes;

    lh_assert_runtime_ref(str);
    if (lh_math_is_zero(data_size))
    {
        return 0U;
    }
    lh_assert_runtime_ref(data);
    if (lh_math_gt(data_size, lh_math_div(LH_USIZE_T_MAX, 2U)) ||
        lh_math_gt(lh_math_mul(data_size, 2U), str_size))
    {
        return 0U;
    }

    bytes = lh_ptr_rcast(const lh_byte_t, data);
    for (lh_usize_t i = 0U; lh_math_lt(i, data_size); i = lh_math_add_one(i))
    {
        lh_str_ptr_format_bytes_hex_byte(bytes[i], uppercase,
                                         lh_ptr_add_by_offset_unsafe(lh_char_t, str, lh_math_mul(i, 2U)));
    }
    return lh_math_mul(data_size, 2U);
}

lh_usize_t
lh_str_ptr_format_bytes_hex_dump_line(const lh_ptr data, lh_usize_t count, lh_u32_t offset,
                                      lh_bool_t uppercase, lh_str_ptr str)
{
    lh_byte_cptr bytes = lh_ptr_rcast(const lh_byte_t, data);
    lh_usize_t pos = LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS;

    lh_str_ptr_format_bytes_hex_dump_offset(offset, uppercase, str);
    str[pos] = lh_char_map_colon;
    str[lh_math_add_one(pos)] = lh_char_map_space;
    pos = lh_math_add(pos, 2U);
    for (lh_usize_t j = 0U; lh_math_lt(j, count); j = lh_math_add_one(j))
    {
        lh_str_ptr_format_bytes_hex_byte(bytes[j], uppercase,
                                         lh_ptr_add_by_offset_unsafe(lh_char_t, str, pos));
        str[lh_math_add(pos, 2U)] = lh_char_map_space;
        pos = lh_math_add(pos, 3U);
    }
    str[pos] = lh_char_map_lf;
    return lh_math_add_one(pos);
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
lh_str_ptr_format_bytes_hex_dump(const lh_ptr data, lh_usize_t data_size, lh_bool_t uppercase,
                                 lh_str_ptr str, lh_usize_t str_size)
{
    lh_usize_t needed;
    lh_usize_t pos = 0U;

    lh_assert_runtime_ref(str);
    if (lh_math_is_zero(data_size))
    {
        return 0U;
    }
    lh_assert_runtime_ref(data);
    needed = lh_str_ptr_format_bytes_hex_dump_size(data_size);
    if (lh_math_is_zero(needed) || lh_math_gt(needed, str_size))
    {
        return 0U;
    }

    for (lh_usize_t offset = 0U; lh_math_lt(offset, data_size);
         offset = lh_math_add(offset, LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH))
    {
        pos = lh_math_add(
            pos, lh_str_ptr_format_bytes_hex_dump_line(
                     lh_ptr_add_by_offset_unsafe(const lh_void, data, offset),
                     lh_math_min(lh_math_sub(data_size, offset), LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH),
                     lh_cast_static(lh_u32_t, offset), uppercase,
                     lh_ptr_add_by_offset_unsafe(lh_char_t, str, pos)));
    }
    return pos;
}
