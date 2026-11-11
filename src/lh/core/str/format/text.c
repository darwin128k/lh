#include <lh/core/str/format/text.h>
#include <lh/core/assert.h>
#include <lh/core/memory/std.h>
#include <lh/core/str/format/hex.h>
#include <lh/core/str/format/sint.h>
#include <lh/core/str/format/sllong.h>
#include <lh/core/str/format/ullong.h>
#include <lh/core/str/format/real.h>
#include <lh/core/str/scanf/next.h>
#include <lh/core/util/str/ptr.h>
#include <lh/core/char/map.h>
#include <lh/core/math.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_str_ptr_format_text_put_text(lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                                lh_str_cptr text, lh_usize_t n)
{
    const lh_usize_t at = lh_ptr_deref(pos);

    if (lh_ptr_is_set(str))
    {
        lh_return_if(lh_math_gt(n, lh_math_sub(str_size, at)), lh_bool_false);
        lh_memory_std_copy(lh_ptr_add_by_offset(lh_char_t, str, at), text, n);
    }
    lh_ptr_deref(pos) = lh_math_add(at, n);
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_format_text_put_run(lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos, lh_char_t ch,
                               lh_usize_t n)
{
    const lh_usize_t at = lh_ptr_deref(pos);

    if (lh_ptr_is_set(str))
    {
        lh_return_if(lh_math_gt(n, lh_math_sub(str_size, at)), lh_bool_false);
        lh_memory_std_set(lh_ptr_add_by_offset(lh_char_t, str, at), lh_cast_static(lh_byte_t, ch),
                          n);
    }
    lh_ptr_deref(pos) = lh_math_add(at, n);
    return lh_bool_true;
}

lh_sllong_t
lh_str_ptr_format_text_read_sint(const lh_str_scanf_spec_t *spec, va_list *args)
{
    switch (lh_str_scanf_spec_get_length(spec))
    {
    case lh_str_scanf_spec_length_char:
        return lh_cast_static(lh_s8_t, va_arg(lh_ptr_deref(args), lh_sint_t));
    case lh_str_scanf_spec_length_short:
        return lh_cast_static(lh_s16_t, va_arg(lh_ptr_deref(args), lh_sint_t));
    case lh_str_scanf_spec_length_long:
        return va_arg(lh_ptr_deref(args), lh_slong_t);
    case lh_str_scanf_spec_length_long_long:
        return va_arg(lh_ptr_deref(args), lh_sllong_t);
    case lh_str_scanf_spec_length_size:
        return va_arg(lh_ptr_deref(args), lh_ssize_t);
    default:
        return va_arg(lh_ptr_deref(args), lh_sint_t);
    }
}

lh_ullong_t
lh_str_ptr_format_text_read_uint(const lh_str_scanf_spec_t *spec, va_list *args)
{
    switch (lh_str_scanf_spec_get_length(spec))
    {
    case lh_str_scanf_spec_length_char:
        return lh_cast_static(lh_u8_t, va_arg(lh_ptr_deref(args), lh_uint_t));
    case lh_str_scanf_spec_length_short:
        return lh_cast_static(lh_u16_t, va_arg(lh_ptr_deref(args), lh_uint_t));
    case lh_str_scanf_spec_length_long:
        return va_arg(lh_ptr_deref(args), lh_ulong_t);
    case lh_str_scanf_spec_length_long_long:
        return va_arg(lh_ptr_deref(args), lh_ullong_t);
    case lh_str_scanf_spec_length_size:
        return va_arg(lh_ptr_deref(args), lh_usize_t);
    default:
        return va_arg(lh_ptr_deref(args), lh_uint_t);
    }
}

lh_usize_t
lh_str_ptr_format_text_get_fit(lh_str_cptr str, lh_usize_t str_size, lh_usize_t pos, lh_usize_t n,
                               lh_bool_t cut)
{
    return (cut && lh_ptr_is_set(str)) ? lh_math_min(n, lh_math_sub(str_size, pos)) : n;
}

lh_bool_t
lh_str_ptr_format_text_get_value(const lh_str_scanf_spec_t *spec, va_list *args, lh_str_ptr buf,
                                 lh_str_cptr *content, lh_usize_t *size)
{
    const lh_str_scanf_spec_kind_t kind = lh_str_scanf_spec_get_kind(spec);
    lh_usize_t n = 0U;

    lh_ptr_deref(content) = buf;
    switch (kind)
    {
    case lh_str_scanf_spec_kind_percent:
        buf[0] = lh_char_map_percent;
        n = 1U;
        break;
    case lh_str_scanf_spec_kind_char:
        buf[0] = lh_cast_static(lh_char_t, va_arg(lh_ptr_deref(args), lh_sint_t));
        n = 1U;
        break;
    case lh_str_scanf_spec_kind_uint:
        n = lh_str_ptr_format_ullong(lh_str_ptr_format_text_read_uint(spec, args), buf,
                                     LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
        break;
    case lh_str_scanf_spec_kind_sint:
        n = lh_str_ptr_format_sllong(lh_str_ptr_format_text_read_sint(spec, args), buf,
                                     LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
        break;
    case lh_str_scanf_spec_kind_hex:
        n = lh_str_ptr_format_hex_ullong(lh_str_ptr_format_text_read_uint(spec, args),
                                         lh_str_scanf_spec_get_uppercase(spec), buf,
                                         LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
        break;
    case lh_str_scanf_spec_kind_pointer:
        buf[0] = lh_char_map_digit_0;
        buf[1] = lh_char_map_small_x;
        n = lh_str_ptr_format_hex_ullong(
            lh_cast_static(lh_ullong_t,
                           lh_cast_reinterpret(lh_usize_t, va_arg(lh_ptr_deref(args), lh_ptr))),
            lh_bool_false, buf + 2, LH_STR_FORMAT_TEXT_VALUE_BUF_MAX - 2U);
        n = lh_math_is_zero(n) ? 0U : n + 2U;
        break;
#if LH_REAL_ENABLED
    case lh_str_scanf_spec_kind_real:
        /* A float argument arrives promoted to double, whatever lh_real_t is. */
        n = lh_str_ptr_format_real_fixed(
            lh_cast_static(lh_real_t, va_arg(lh_ptr_deref(args), double)),
            lh_str_scanf_spec_get_have_precision(spec)
                ? lh_cast_static(lh_usize_t, lh_str_scanf_spec_get_precision(spec))
                : 6U,
            buf, LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
        break;
#endif
    case lh_str_scanf_spec_kind_str:
        lh_ptr_deref(content) = va_arg(lh_ptr_deref(args), lh_str_cptr);
        n = lh_str_ptr_len(lh_ptr_deref(content));
        if (lh_str_scanf_spec_get_have_precision(spec))
        {
            n = lh_math_min(n, lh_cast_static(lh_usize_t, lh_str_scanf_spec_get_precision(spec)));
        }
        break;
    default:
        return lh_bool_false;
    }
    lh_ptr_deref(size) = n;
    return lh_math_ne(n, 0U) || lh_math_eq(kind, lh_str_scanf_spec_kind_str);
}

lh_bool_t
lh_str_ptr_format_text_put_field(lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                                 const lh_str_scanf_spec_t *spec, lh_str_cptr content,
                                 lh_usize_t size, lh_bool_t cut)
{
    const lh_usize_t width = lh_cast_static(lh_usize_t, lh_str_scanf_spec_get_width(spec));
    const lh_usize_t pad = lh_math_gt(width, size) ? lh_math_sub(width, size) : 0U;
    const lh_usize_t start = lh_ptr_deref(pos);
    lh_usize_t at = start;

    /* All or nothing: room for the whole field first, then its pieces. In cut
       mode each piece is written as far as it fits instead. */
    lh_return_if(!cut && lh_ptr_is_set(str) &&
                     lh_math_gt(lh_math_add(pad, size), lh_math_sub(str_size, at)),
                 lh_bool_false);

    const lh_bool_t has_sign = lh_math_gt(size, 0U) && lh_math_eq(content[0], lh_char_map_minus);

    if (lh_math_is_zero(pad))
    {
        /* Common path: no width padding — one copy, no put_run(0). */
        lh_str_ptr_format_text_put_text(
            str, str_size, lh_addr_of(at), content,
            lh_str_ptr_format_text_get_fit(str, str_size, at, size, cut));
    }
    else if (lh_str_scanf_spec_get_left_justify(spec))
    {
        lh_str_ptr_format_text_put_text(
            str, str_size, lh_addr_of(at), content,
            lh_str_ptr_format_text_get_fit(str, str_size, at, size, cut));
        lh_str_ptr_format_text_put_run(str, str_size, lh_addr_of(at), lh_char_map_space,
                                       lh_str_ptr_format_text_get_fit(str, str_size, at, pad, cut));
    }
    else if (lh_str_scanf_spec_get_zero_pad(spec) && has_sign)
    {
        lh_str_ptr_format_text_put_text(str, str_size, lh_addr_of(at), content,
                                        lh_str_ptr_format_text_get_fit(str, str_size, at, 1U, cut));
        lh_str_ptr_format_text_put_run(str, str_size, lh_addr_of(at), lh_char_map_digit_0,
                                       lh_str_ptr_format_text_get_fit(str, str_size, at, pad, cut));
        lh_str_ptr_format_text_put_text(
            str, str_size, lh_addr_of(at), lh_ptr_add_by_offset(const lh_char_t, content, 1U),
            lh_str_ptr_format_text_get_fit(str, str_size, at, lh_math_sub_one(size), cut));
    }
    else
    {
        lh_str_ptr_format_text_put_run(str, str_size, lh_addr_of(at),
                                       lh_str_scanf_spec_get_zero_pad(spec) ? lh_char_map_digit_0
                                                                            : lh_char_map_space,
                                       lh_str_ptr_format_text_get_fit(str, str_size, at, pad, cut));
        lh_str_ptr_format_text_put_text(
            str, str_size, lh_addr_of(at), content,
            lh_str_ptr_format_text_get_fit(str, str_size, at, size, cut));
    }
    lh_ptr_deref(pos) = at;
    return lh_math_eq(lh_math_sub(at, start), lh_math_add(pad, size));
}

lh_usize_t
lh_str_ptr_format_text_walk(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args,
                            lh_bool_t cut)
{
    /* Counting (no str) reports a bad mask as LH_STR_PTR_INVALID, writing as 0. */
    const lh_usize_t failed = lh_ptr_is_null(str) ? LH_STR_PTR_INVALID : 0U;
    lh_usize_t out_pos = 0U;
    lh_usize_t fmt_pos = 0U;
    lh_str_scanf_spec_t spec;
    lh_char_t buf[LH_STR_FORMAT_TEXT_VALUE_BUF_MAX];
    va_list ap;

    va_copy(ap, args);
    while (lh_str_ptr_scanf_next(fmt, lh_addr_of(fmt_pos), lh_addr_of(spec)))
    {
        /* Where a value's text is and how long: get_value writes both. */
        lh_str_cptr content;
        lh_usize_t size;

        lh_bool_t ok = lh_bool_false;

        if (lh_math_eq(lh_str_scanf_spec_get_kind(lh_addr_of(spec)),
                       lh_str_scanf_spec_kind_literal))
        {
            const lh_usize_t n = lh_str_scanf_spec_get_literal_size(lh_addr_of(spec));
            const lh_usize_t fit = lh_str_ptr_format_text_get_fit(str, str_size, out_pos, n, cut);

            ok = lh_str_ptr_format_text_put_text(str, str_size, lh_addr_of(out_pos),
                                                 lh_str_scanf_spec_get_literal(lh_addr_of(spec)),
                                                 fit) &&
                 lh_math_eq(fit, n);
        }
        else if (lh_math_eq(lh_str_scanf_spec_get_kind(lh_addr_of(spec)),
                            lh_str_scanf_spec_kind_invalid) ||
                 !lh_str_ptr_format_text_get_value(lh_addr_of(spec), lh_addr_of(ap), buf,
                                                   lh_addr_of(content), lh_addr_of(size)))
        {
            va_end(ap);
            return failed;
        }
        else
        {
            ok = lh_str_ptr_format_text_put_field(str, str_size, lh_addr_of(out_pos),
                                                  lh_addr_of(spec), content, size, cut);
        }
        if (!ok)
        {
            va_end(ap);
            /* Cut: what fitted stays, and it fills the buffer. */
            return cut ? out_pos : 0U;
        }
    }
    va_end(ap);
    return out_pos;
}

lh_usize_t
lh_str_ptr_format_text_v(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args)
{
    lh_assert_runtime_ref(str);
    return lh_str_ptr_format_text_walk(str, str_size, fmt, args, lh_bool_false);
}

lh_usize_t
lh_str_ptr_format_text_cut_v(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args)
{
    lh_assert_runtime_ref(str);
    return lh_str_ptr_format_text_walk(str, str_size, fmt, args, lh_bool_true);
}

lh_usize_t
lh_str_ptr_format_text_size_v(lh_str_cptr fmt, va_list args)
{
    return lh_str_ptr_format_text_walk(lh_null, 0, fmt, args, lh_bool_false);
}

lh_usize_t
lh_str_ptr_format_text(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    const lh_usize_t result = lh_str_ptr_format_text_v(str, str_size, fmt, args);

    va_end(args);
    return result;
}

lh_usize_t
lh_str_ptr_format_text_size(lh_str_cptr fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    const lh_usize_t result = lh_str_ptr_format_text_size_v(fmt, args);

    va_end(args);
    return result;
}

lh_usize_t
lh_str_ptr_format_cstr_v(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, va_list args)
{
    lh_assert_runtime_if(str_size == 0U, lh_runtime_error_code_invalid_argument);

    const lh_usize_t size = lh_str_ptr_format_text_v(str, str_size - 1U, fmt, args);

    lh_str_ptr_erase(str, size);
    return size;
}

lh_usize_t
lh_str_ptr_format_cstr_cut_v(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt,
                             va_list args)
{
    lh_assert_runtime_if(str_size == 0U, lh_runtime_error_code_invalid_argument);

    const lh_usize_t size = lh_str_ptr_format_text_cut_v(str, str_size - 1U, fmt, args);

    lh_str_ptr_erase(str, size);
    return size;
}

lh_usize_t
lh_str_ptr_format_cstr(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    const lh_usize_t size = lh_str_ptr_format_cstr_v(str, str_size, fmt, args);

    va_end(args);
    return size;
}
