#include <lh/str/parse/text.h>
#include <lh/assert.h>
#include <lh/char/digit.h>
#include <lh/char/xdigit.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/numeric/limits.h>
#include <lh/numeric/types.h>
#include <lh/str/parse/hex.h>
#include <lh/str/parse/uint.h>
#include <lh/util/addr.h>

static lh_usize_t
lh_str_ptr_parse_text_take_run(lh_str_cptr str, lh_usize_t str_size, lh_usize_t str_pos,
                               lh_uint_t width, lh_bool_t hex)
{
    lh_usize_t end = str_pos;

    while (end < str_size)
    {
        if (width > 0 && (end - str_pos) >= width)
        {
            break;
        }
        if (hex)
        {
            if (!lh_char_is_xdigit(str[end]))
            {
                break;
            }
        }
        else if (!lh_char_is_digit(str[end]))
        {
            break;
        }
        end++;
    }

    return end;
}

static lh_bool_t
lh_str_ptr_parse_text_uint_field(lh_str_cptr str, lh_usize_t str_size, lh_usize_t *str_pos,
                                 lh_uint_t width, lh_bool_t hex, lh_uint_t *out)
{
    lh_usize_t start = *str_pos;
    lh_usize_t end = lh_str_ptr_parse_text_take_run(str, str_size, start, width, hex);

    if (end == start)
    {
        return lh_bool_false;
    }

    if (hex)
    {
        if (!lh_str_ptr_parse_hex(str + start, end - start, LH_UINT_T_MAX, out))
        {
            return lh_bool_false;
        }
    }
    else if (!lh_str_ptr_parse_uint(str + start, end - start, LH_UINT_T_MAX, out))
    {
        return lh_bool_false;
    }

    *str_pos = end;
    return lh_bool_true;
}

static lh_bool_t
lh_str_ptr_parse_text_sint_field(lh_str_cptr str, lh_usize_t str_size, lh_usize_t *str_pos,
                                 lh_uint_t width, lh_sint_t *out)
{
    lh_bool_t negative = lh_bool_false;
    lh_uint_t magnitude;
    lh_uint_t digit_width = width;

    if (*str_pos >= str_size)
    {
        return lh_bool_false;
    }

    if (str[*str_pos] == '-')
    {
        if (width == 1U)
        {
            return lh_bool_false;
        }
        negative = lh_bool_true;
        (*str_pos)++;
        if (digit_width > 0)
        {
            digit_width -= 1U;
        }
    }

    if (!lh_str_ptr_parse_text_uint_field(str, str_size, str_pos, digit_width, lh_bool_false,
                                          lh_addr_of(magnitude)))
    {
        return lh_bool_false;
    }

    if (negative)
    {
        const lh_uint_t min_mag = (lh_uint_t)LH_SINT_T_MAX + 1U;
        if (magnitude > min_mag)
        {
            return lh_bool_false;
        }
        if (magnitude == min_mag)
        {
            *out = LH_SINT_T_MIN;
        }
        else
        {
            *out = -(lh_sint_t)magnitude;
        }
    }
    else
    {
        if (magnitude > (lh_uint_t)LH_SINT_T_MAX)
        {
            return lh_bool_false;
        }
        *out = (lh_sint_t)magnitude;
    }

    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_parse_text_v(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args)
{
    lh_usize_t str_pos = 0;
    lh_usize_t fmt_pos = 0;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(fmt);

    while (fmt[fmt_pos] != '\0')
    {
        lh_uint_t width = 0;
        lh_char_t spec;

        if (fmt[fmt_pos] != '%')
        {
            lh_usize_t run_start = fmt_pos;
            lh_usize_t run;
            while (fmt[fmt_pos] != '\0' && fmt[fmt_pos] != '%')
            {
                fmt_pos++;
            }
            run = fmt_pos - run_start;
            if (str_pos + run > str_size)
            {
                return lh_bool_false;
            }
            if (lh_memory_compare(str + str_pos, run, fmt + run_start, run) != lh_null)
            {
                return lh_bool_false;
            }
            str_pos += run;
            continue;
        }
        fmt_pos++;

        for (;;)
        {
            if (fmt[fmt_pos] == '0' || fmt[fmt_pos] == '-')
            {
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
                return lh_bool_false;
            }
            fmt_pos++;
        }

        if (fmt[fmt_pos] == '.')
        {
            fmt_pos++;
            while (lh_char_is_digit(fmt[fmt_pos]))
            {
                fmt_pos++;
            }
        }

        spec = fmt[fmt_pos];
        if (spec == '\0')
        {
            return lh_bool_false;
        }
        fmt_pos++;

        switch (spec)
        {
        case '%':
            if (str_pos >= str_size || str[str_pos] != '%')
            {
                return lh_bool_false;
            }
            str_pos++;
            break;
        case 'c':
            if (str_pos >= str_size)
            {
                return lh_bool_false;
            }
            *va_arg(args, lh_char_t *) = str[str_pos++];
            break;
        case 'u':
            if (!lh_str_ptr_parse_text_uint_field(str, str_size, lh_addr_of(str_pos), width,
                                                  lh_bool_false, va_arg(args, lh_uint_t *)))
            {
                return lh_bool_false;
            }
            break;
        case 'd':
        case 'i':
            if (!lh_str_ptr_parse_text_sint_field(str, str_size, lh_addr_of(str_pos), width,
                                                  va_arg(args, lh_sint_t *)))
            {
                return lh_bool_false;
            }
            break;
        case 'x':
        case 'X':
            if (!lh_str_ptr_parse_text_uint_field(str, str_size, lh_addr_of(str_pos), width,
                                                  lh_bool_true, va_arg(args, lh_uint_t *)))
            {
                return lh_bool_false;
            }
            break;
        default:
            return lh_bool_false;
        }
    }

    return str_pos == str_size ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_str_ptr_parse_text(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr fmt, ...)
{
    lh_bool_t result;
    va_list args;

    va_start(args, fmt);
    result = lh_str_ptr_parse_text_v(str, str_size, fmt, args);
    va_end(args);

    return result;
}
