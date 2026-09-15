/**
 * @file next.h
 * @brief Walk a printf/scanf format mask, one token at a time.
 *
 * The piece shared by ::lh_str_ptr_format_text and ::lh_str_ptr_parse_text:
 * both interpret the same mask (`%%`, `%c`, `%s`, `%d`/`%i`, `%u`, `%x`/`%X`,
 * flags `0`/`-`, decimal width, `.` precision). Conversion of values is left
 * to the caller.
 *
 * ::LH_ATTRIBUTE_FORCE_INLINE — called once per token from the text
 * format/parse engines.
 */

#ifndef LH_STR_SCANF_NEXT_H
#define LH_STR_SCANF_NEXT_H

#include <lh/assert.h>
#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/char/digit.h>
#include <lh/compiler/extern/c.h>
#include <lh/str/scanf/spec.h>
#include <lh/util/addr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Extract the next mask token starting at `*pos`.
 *
 * `*pos` is advanced past the token. Loop until this returns ::lh_bool_false
 * to walk every token in @p fmt.
 *
 * @param fmt  NUL-terminated format mask.
 * @param pos  In/out cursor. Start at `0`.
 * @param out  Receives the token. On ::lh_str_scanf_spec_kind_invalid the
 *             mask is malformed; on ::lh_str_scanf_spec_kind_literal,
 *             `literal` / `literal_size` view a slice of @p fmt.
 *
 * @return ::lh_bool_true if a token was produced (including invalid),
 *         ::lh_bool_false if `*pos` is already at the NUL (nothing left).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_scanf_next(lh_str_cptr fmt, lh_usize_t *pos, lh_str_scanf_spec_t *out)
{
    lh_usize_t fmt_pos;
    lh_char_t spec;

    lh_assert_runtime_ref(fmt);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(out);

    fmt_pos = *pos;

    out->kind = lh_str_scanf_spec_kind_invalid;
    out->zero_pad = lh_bool_false;
    out->left_justify = lh_bool_false;
    out->uppercase = lh_bool_false;
    out->width = 0;
    out->have_precision = lh_bool_false;
    out->precision = 0;
    out->literal = fmt;
    out->literal_size = 0;

    if (fmt[fmt_pos] == '\0')
    {
        return lh_bool_false;
    }

    if (fmt[fmt_pos] != '%')
    {
        lh_usize_t run_start = fmt_pos;
        while (fmt[fmt_pos] != '\0' && fmt[fmt_pos] != '%')
        {
            fmt_pos++;
        }
        out->kind = lh_str_scanf_spec_kind_literal;
        out->literal = fmt + run_start;
        out->literal_size = fmt_pos - run_start;
        *pos = fmt_pos;
        return lh_bool_true;
    }

    fmt_pos++; /* consume '%' */

    for (;;)
    {
        if (fmt[fmt_pos] == '0')
        {
            out->zero_pad = lh_bool_true;
            fmt_pos++;
        }
        else if (fmt[fmt_pos] == '-')
        {
            out->left_justify = lh_bool_true;
            fmt_pos++;
        }
        else
        {
            break;
        }
    }

    while (lh_char_is_digit(fmt[fmt_pos]))
    {
        if (!lh_char_digit_accumulate(lh_addr_of(out->width), lh_char_to_digit(fmt[fmt_pos])))
        {
            *pos = fmt_pos;
            return lh_bool_true; /* kind stays invalid */
        }
        fmt_pos++;
    }

    if (fmt[fmt_pos] == '.')
    {
        fmt_pos++;
        out->have_precision = lh_bool_true;
        while (lh_char_is_digit(fmt[fmt_pos]))
        {
            if (!lh_char_digit_accumulate(lh_addr_of(out->precision),
                                          lh_char_to_digit(fmt[fmt_pos])))
            {
                *pos = fmt_pos;
                return lh_bool_true;
            }
            fmt_pos++;
        }
    }

    spec = fmt[fmt_pos];
    if (spec == '\0')
    {
        *pos = fmt_pos;
        return lh_bool_true; /* '%' at the end, no conversion */
    }
    fmt_pos++;

    switch (spec)
    {
    case '%':
        out->kind = lh_str_scanf_spec_kind_percent;
        break;
    case 'c':
        out->kind = lh_str_scanf_spec_kind_char;
        break;
    case 's':
        out->kind = lh_str_scanf_spec_kind_str;
        break;
    case 'u':
        out->kind = lh_str_scanf_spec_kind_uint;
        break;
    case 'd':
    case 'i':
        out->kind = lh_str_scanf_spec_kind_sint;
        break;
    case 'x':
        out->kind = lh_str_scanf_spec_kind_hex;
        break;
    case 'X':
        out->kind = lh_str_scanf_spec_kind_hex;
        out->uppercase = lh_bool_true;
        break;
    default:
        break; /* kind stays invalid */
    }

    *pos = fmt_pos;
    return lh_bool_true;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_SCANF_NEXT_H */
