#include <lh/str/format/text.h>
#include <lh/assert.h>
#include <lh/bool.h>
#include <lh/char/digit.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/str/format/hex.h>
#include <lh/str/format/sint.h>
#include <lh/str/format/uint.h>
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

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(fmt);

    while (fmt[fmt_pos] != '\0')
    {
        lh_bool_t zero_pad = lh_bool_false;
        lh_bool_t left_justify = lh_bool_false;
        lh_uint_t width = 0;
        lh_bool_t have_precision = lh_bool_false;
        lh_uint_t precision = 0;
        lh_char_t spec;
        lh_char_t value_buf[LH_STR_FORMAT_TEXT_VALUE_BUF_MAX];
        lh_str_cptr content = value_buf;
        lh_usize_t content_len = 0;

        if (fmt[fmt_pos] != '%')
        {
            if (out_pos >= str_size)
            {
                return 0;
            }
            str[out_pos++] = fmt[fmt_pos++];
            continue;
        }
        fmt_pos++; /* consume '%' */

        for (;;)
        {
            if (fmt[fmt_pos] == '0')
            {
                zero_pad = lh_bool_true;
                fmt_pos++;
            }
            else if (fmt[fmt_pos] == '-')
            {
                left_justify = lh_bool_true;
                fmt_pos++;
            }
            else
            {
                break;
            }
        }

        while (lh_char_is_digit(fmt[fmt_pos]))
        {
            if (!lh_char_digit_accumulate(lh_addr_of(width), lh_char_to_digit(fmt[fmt_pos])))
            {
                return 0; /* width overflow */
            }
            fmt_pos++;
        }

        if (fmt[fmt_pos] == '.')
        {
            fmt_pos++;
            have_precision = lh_bool_true;
            while (lh_char_is_digit(fmt[fmt_pos]))
            {
                if (!lh_char_digit_accumulate(lh_addr_of(precision),
                                              lh_char_to_digit(fmt[fmt_pos])))
                {
                    return 0; /* precision overflow */
                }
                fmt_pos++;
            }
        }

        spec = fmt[fmt_pos];
        if (spec == '\0')
        {
            return 0; /* '%' at the very end of fmt, no conversion character */
        }
        fmt_pos++;

        switch (spec)
        {
        case '%':
            value_buf[0] = '%';
            content_len = 1;
            break;
        case 'c':
            value_buf[0] = (lh_char_t)va_arg(args, lh_sint_t);
            content_len = 1;
            break;
        case 'u':
            content_len = lh_str_ptr_format_uint(va_arg(args, lh_uint_t), value_buf,
                                                 LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
            break;
        case 'd':
        case 'i':
            content_len = lh_str_ptr_format_sint(va_arg(args, lh_sint_t), value_buf,
                                                 LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
            break;
        case 'x':
            content_len = lh_str_ptr_format_hex(va_arg(args, lh_uint_t), lh_bool_false, value_buf,
                                                LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
            break;
        case 'X':
            content_len = lh_str_ptr_format_hex(va_arg(args, lh_uint_t), lh_bool_true, value_buf,
                                                LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
            break;
        case 's':
            content = va_arg(args, lh_str_cptr);
            content_len = lh_str_ptr_len(content);
            if (have_precision && precision < content_len)
            {
                content_len = precision;
            }
            break;
        default:
            return 0; /* unsupported conversion */
        }

        if (content_len == 0 && spec != 's')
        {
            return 0; /* the per-type formatter ran out of buffer space */
        }

        {
            lh_bool_t has_sign = content_len > 0 && content[0] == '-';
            lh_usize_t pad_len = width > content_len ? width - content_len : 0U;

            if (left_justify)
            {
                if (out_pos + content_len + pad_len > str_size)
                {
                    return 0;
                }
                lh_str_ptr_copy(str + out_pos, content_len, content, content_len);
                out_pos += content_len;
                lh_str_ptr_set(str + out_pos, pad_len, ' ');
                out_pos += pad_len;
            }
            else if (zero_pad && has_sign)
            {
                if (out_pos + 1U + pad_len + (content_len - 1U) > str_size)
                {
                    return 0;
                }
                str[out_pos++] = content[0];
                lh_str_ptr_set(str + out_pos, pad_len, '0');
                out_pos += pad_len;
                lh_str_ptr_copy(str + out_pos, content_len - 1U, content + 1, content_len - 1U);
                out_pos += content_len - 1U;
            }
            else
            {
                lh_char_t pad_char = zero_pad ? '0' : ' ';
                if (out_pos + pad_len + content_len > str_size)
                {
                    return 0;
                }
                lh_str_ptr_set(str + out_pos, pad_len, pad_char);
                out_pos += pad_len;
                lh_str_ptr_copy(str + out_pos, content_len, content, content_len);
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
