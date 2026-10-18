#include <lh/str/format/text.h>
#include <lh/assert.h>
#include <lh/memory/std.h>
#include <lh/str/format/hex.h>
#include <lh/str/format/sint.h>
#include <lh/str/format/uint.h>
#include <lh/str/scanf/next.h>
#include <lh/util/str/ptr.h>

/* lh_sint_t/lh_uint_t are 32-bit; "-2147483648" (11 chars) is the longest
 * conversion this engine ever builds into a local buffer. %s bypasses this
 * buffer entirely and is copied straight from the caller's argument. */
#define LH_STR_FORMAT_TEXT_VALUE_BUF_MAX 16U

lh_usize_t
lh_str_ptr_format_text_walk(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args)
{
    const lh_bool_t counting = lh_null_eq(str);
    lh_usize_t out_pos = 0;
    lh_usize_t fmt_pos = 0;
    lh_str_scanf_spec_t spec;


    while (lh_str_ptr_scanf_next(fmt, lh_addr_of(fmt_pos), lh_addr_of(spec)))
    {
        lh_char_t value_buf[LH_STR_FORMAT_TEXT_VALUE_BUF_MAX];
        lh_str_cptr content = value_buf;
        lh_usize_t content_len = 0;

        if (lh_str_scanf_spec_get_kind(lh_addr_of(spec)) == lh_str_scanf_spec_kind_invalid)
        {
            return counting ? LH_STR_PTR_INVALID : 0;
        }

        if (lh_str_scanf_spec_get_kind(lh_addr_of(spec)) == lh_str_scanf_spec_kind_literal)
        {
            if (counting)
            {
                out_pos += lh_str_scanf_spec_get_literal_size(lh_addr_of(spec));
                continue;
            }
            if (out_pos + lh_str_scanf_spec_get_literal_size(lh_addr_of(spec)) > str_size)
            {
                return 0;
            }
            lh_memory_std_copy(str + out_pos, lh_str_scanf_spec_get_literal(lh_addr_of(spec)), lh_str_scanf_spec_get_literal_size(lh_addr_of(spec)));
            out_pos += lh_str_scanf_spec_get_literal_size(lh_addr_of(spec));
            continue;
        }

        switch (lh_str_scanf_spec_get_kind(lh_addr_of(spec)))
        {
        case lh_str_scanf_spec_kind_percent:
            value_buf[0] = '%';
            content_len = 1;
            break;
        case lh_str_scanf_spec_kind_char:
            value_buf[0] = lh_cast_static(lh_char_t, va_arg(args, lh_sint_t));
            content_len = 1;
            break;
        case lh_str_scanf_spec_kind_uint:
            content_len = lh_str_ptr_format_uint(va_arg(args, lh_uint_t), value_buf,
                                                 LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
            break;
        case lh_str_scanf_spec_kind_sint:
            content_len = lh_str_ptr_format_sint(va_arg(args, lh_sint_t), value_buf,
                                                 LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
            break;
        case lh_str_scanf_spec_kind_hex:
            content_len = lh_str_ptr_format_hex(va_arg(args, lh_uint_t), lh_str_scanf_spec_get_uppercase(lh_addr_of(spec)), value_buf,
                                                LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
            break;
        case lh_str_scanf_spec_kind_str:
            content = va_arg(args, lh_str_cptr);
            content_len = lh_str_ptr_len(content);
            if (lh_str_scanf_spec_get_have_precision(lh_addr_of(spec)) && lh_str_scanf_spec_get_precision(lh_addr_of(spec)) < content_len)
            {
                content_len = lh_str_scanf_spec_get_precision(lh_addr_of(spec));
            }
            break;
        default:
            return counting ? LH_STR_PTR_INVALID : 0;
        }

        if (content_len == 0 && lh_str_scanf_spec_get_kind(lh_addr_of(spec)) != lh_str_scanf_spec_kind_str)
        {
            return counting ? LH_STR_PTR_INVALID : 0;
        }

        {
            lh_bool_t has_sign = content_len > 0 && content[0] == '-';
            lh_usize_t pad_len = lh_str_scanf_spec_get_width(lh_addr_of(spec)) > content_len ? lh_str_scanf_spec_get_width(lh_addr_of(spec)) - content_len : 0U;

            if (counting)
            {
                out_pos += pad_len + content_len;
                continue;
            }

            if (lh_str_scanf_spec_get_left_justify(lh_addr_of(spec)))
            {
                if (out_pos + content_len + pad_len > str_size)
                {
                    return 0;
                }
                lh_memory_std_copy(str + out_pos, content, content_len);
                out_pos += content_len;
                lh_memory_std_set(str + out_pos, lh_cast_static(lh_byte_t, ' '), pad_len);
                out_pos += pad_len;
            }
            else if (lh_str_scanf_spec_get_zero_pad(lh_addr_of(spec)) && has_sign)
            {
                if (out_pos + 1U + pad_len + (content_len - 1U) > str_size)
                {
                    return 0;
                }
                str[out_pos++] = content[0];
                lh_memory_std_set(str + out_pos, lh_cast_static(lh_byte_t, '0'), pad_len);
                out_pos += pad_len;
                lh_memory_std_copy(str + out_pos, content + 1, content_len - 1U);
                out_pos += content_len - 1U;
            }
            else
            {
                lh_char_t pad_char = lh_str_scanf_spec_get_zero_pad(lh_addr_of(spec)) ? '0' : ' ';
                if (out_pos + pad_len + content_len > str_size)
                {
                    return 0;
                }
                lh_memory_std_set(str + out_pos, lh_cast_static(lh_byte_t, pad_char), pad_len);
                out_pos += pad_len;
                lh_memory_std_copy(str + out_pos, content, content_len);
                out_pos += content_len;
            }
        }
    }

    return out_pos;
}

lh_usize_t
lh_str_ptr_format_text_v(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args)
{
    lh_assert_runtime_ref(str);
    return lh_str_ptr_format_text_walk(str, str_size, fmt, args);
}

lh_usize_t
lh_str_ptr_format_text_size_v(lh_str_cptr fmt, va_list args)
{
    return lh_str_ptr_format_text_walk(lh_null, 0, fmt, args);
}

lh_usize_t
lh_str_ptr_format_text(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, ...)
{
    lh_usize_t result;
    va_list args;

    va_start(args, fmt);
    result = lh_str_ptr_format_text_v(str, str_size, fmt, args);
    va_end(args);

    return result;
}

lh_usize_t
lh_str_ptr_format_text_size(lh_str_cptr fmt, ...)
{
    lh_usize_t result;
    va_list args;

    va_start(args, fmt);
    result = lh_str_ptr_format_text_size_v(fmt, args);
    va_end(args);

    return result;
}
