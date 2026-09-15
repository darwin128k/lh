#include <lh/str/format/text.h>
#include <lh/assert.h>
#include <lh/bool.h>
#include <lh/memory/std.h>
#include <lh/numeric/types.h>
#include <lh/str/format/hex.h>
#include <lh/str/format/sint.h>
#include <lh/str/format/uint.h>
#include <lh/str/scanf/next.h>
#include <lh/util/addr.h>
#include <lh/util/str/ptr.h>

/* lh_sint_t/lh_uint_t are 32-bit; "-2147483648" (11 chars) is the longest
 * conversion this engine ever builds into a local buffer. %s bypasses this
 * buffer entirely and is copied straight from the caller's argument. */
#define LH_STR_FORMAT_TEXT_VALUE_BUF_MAX 16U

lh_usize_t
lh_str_ptr_format_text_v(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args)
{
    lh_usize_t out_pos = 0;
    lh_usize_t fmt_pos = 0;
    lh_str_scanf_spec_t spec;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(fmt);

    while (lh_str_ptr_scanf_next(fmt, lh_addr_of(fmt_pos), lh_addr_of(spec)))
    {
        lh_char_t value_buf[LH_STR_FORMAT_TEXT_VALUE_BUF_MAX];
        lh_str_cptr content = value_buf;
        lh_usize_t content_len = 0;

        if (spec.kind == lh_str_scanf_spec_kind_invalid)
        {
            return 0;
        }

        if (spec.kind == lh_str_scanf_spec_kind_literal)
        {
            if (out_pos + spec.literal_size > str_size)
            {
                return 0;
            }
            lh_memory_std_copy(str + out_pos, spec.literal, spec.literal_size);
            out_pos += spec.literal_size;
            continue;
        }

        switch (spec.kind)
        {
        case lh_str_scanf_spec_kind_percent:
            value_buf[0] = '%';
            content_len = 1;
            break;
        case lh_str_scanf_spec_kind_char:
            value_buf[0] = (lh_char_t)va_arg(args, lh_sint_t);
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
            content_len = lh_str_ptr_format_hex(va_arg(args, lh_uint_t), spec.uppercase, value_buf,
                                                LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
            break;
        case lh_str_scanf_spec_kind_str:
            content = va_arg(args, lh_str_cptr);
            content_len = lh_str_ptr_len(content);
            if (spec.have_precision && spec.precision < content_len)
            {
                content_len = spec.precision;
            }
            break;
        default:
            return 0;
        }

        if (content_len == 0 && spec.kind != lh_str_scanf_spec_kind_str)
        {
            return 0; /* the per-type formatter ran out of buffer space */
        }

        {
            lh_bool_t has_sign = content_len > 0 && content[0] == '-';
            lh_usize_t pad_len = spec.width > content_len ? spec.width - content_len : 0U;

            if (spec.left_justify)
            {
                if (out_pos + content_len + pad_len > str_size)
                {
                    return 0;
                }
                lh_memory_std_copy(str + out_pos, content, content_len);
                out_pos += content_len;
                lh_memory_std_set(str + out_pos, (lh_uchar_t)' ', pad_len);
                out_pos += pad_len;
            }
            else if (spec.zero_pad && has_sign)
            {
                if (out_pos + 1U + pad_len + (content_len - 1U) > str_size)
                {
                    return 0;
                }
                str[out_pos++] = content[0];
                lh_memory_std_set(str + out_pos, (lh_uchar_t)'0', pad_len);
                out_pos += pad_len;
                lh_memory_std_copy(str + out_pos, content + 1, content_len - 1U);
                out_pos += content_len - 1U;
            }
            else
            {
                lh_char_t pad_char = spec.zero_pad ? '0' : ' ';
                if (out_pos + pad_len + content_len > str_size)
                {
                    return 0;
                }
                lh_memory_std_set(str + out_pos, (lh_uchar_t)pad_char, pad_len);
                out_pos += pad_len;
                lh_memory_std_copy(str + out_pos, content, content_len);
                out_pos += content_len;
            }
        }
    }

    return out_pos;
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
