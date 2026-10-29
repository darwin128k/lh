#include <lh/str/format/text.h>
#include <lh/assert.h>
#include <lh/memory/std.h>
#include <lh/str/format/hex.h>
#include <lh/str/format/sint.h>
#include <lh/str/scanf/next.h>
#include <lh/util/str/ptr.h>
#include <lh/char/map.h>
#include <lh/math.h>
#include <lh/util/return.h>

lh_bool_t
lh_str_ptr_format_text_put_text(lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                                lh_str_cptr text, lh_usize_t n)
{
    const lh_usize_t at = lh_ptr_deref(pos);

    if (lh_ptr_is_set(str))
    {
        lh_return_if(lh_math_gt(n, lh_math_sub(str_size, at)), lh_bool_false);
        lh_memory_std_copy(lh_ptr_add_by_offset_unsafe(lh_char_t, str, at), text, n);
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
        lh_memory_std_set(lh_ptr_add_by_offset_unsafe(lh_char_t, str, at), lh_cast_static(lh_byte_t, ch),
                          n);
    }
    lh_ptr_deref(pos) = lh_math_add(at, n);
    return lh_bool_true;
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
        n = lh_str_ptr_format_uint(va_arg(lh_ptr_deref(args), lh_uint_t), buf,
                                   LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
        break;
    case lh_str_scanf_spec_kind_sint:
        n = lh_str_ptr_format_sint(va_arg(lh_ptr_deref(args), lh_sint_t), buf,
                                   LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
        break;
    case lh_str_scanf_spec_kind_hex:
        n = lh_str_ptr_format_hex(va_arg(lh_ptr_deref(args), lh_uint_t), lh_str_scanf_spec_get_uppercase(spec), buf,
                                  LH_STR_FORMAT_TEXT_VALUE_BUF_MAX);
        break;
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
                                 lh_usize_t size)
{
    const lh_usize_t width = lh_cast_static(lh_usize_t, lh_str_scanf_spec_get_width(spec));
    const lh_usize_t pad = lh_math_gt(width, size) ? lh_math_sub(width, size) : 0U;
    lh_usize_t at = lh_ptr_deref(pos);

    /* All or nothing: room for the whole field first, then its pieces. */
    lh_return_if(lh_ptr_is_set(str) && lh_math_gt(lh_math_add(pad, size), lh_math_sub(str_size, at)),
                 lh_bool_false);

    /* Common path: no width padding — one copy, no put_run(0). */
    if (lh_math_is_zero(pad))
    {
        lh_str_ptr_format_text_put_text(str, str_size, lh_addr_of(at), content, size);
        lh_ptr_deref(pos) = at;
        return lh_bool_true;
    }

    {
        const lh_bool_t has_sign =
            lh_math_gt(size, 0U) && lh_math_eq(content[0], lh_char_map_minus);

        if (lh_str_scanf_spec_get_left_justify(spec))
        {
            lh_str_ptr_format_text_put_text(str, str_size, lh_addr_of(at), content, size);
            lh_str_ptr_format_text_put_run(str, str_size, lh_addr_of(at), lh_char_map_space, pad);
        }
        else if (lh_str_scanf_spec_get_zero_pad(spec) && has_sign)
        {
            lh_str_ptr_format_text_put_text(str, str_size, lh_addr_of(at), content, 1U);
            lh_str_ptr_format_text_put_run(str, str_size, lh_addr_of(at), lh_char_map_digit_0, pad);
            lh_str_ptr_format_text_put_text(str, str_size, lh_addr_of(at),
                                            lh_ptr_add_by_offset_unsafe(const lh_char_t, content, 1U),
                                            lh_math_sub_one(size));
        }
        else
        {
            lh_str_ptr_format_text_put_run(str, str_size, lh_addr_of(at),
                                            lh_str_scanf_spec_get_zero_pad(spec) ? lh_char_map_digit_0 : lh_char_map_space,
                                            pad);
            lh_str_ptr_format_text_put_text(str, str_size, lh_addr_of(at), content, size);
        }
    }
    lh_ptr_deref(pos) = at;
    return lh_bool_true;
}

lh_usize_t
lh_str_ptr_format_text_walk(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args)
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
        lh_str_cptr content;
        lh_usize_t size;
        lh_bool_t ok;

        if (lh_math_eq(lh_str_scanf_spec_get_kind(lh_addr_of(spec)), lh_str_scanf_spec_kind_literal))
        {
            ok = lh_str_ptr_format_text_put_text(str, str_size, lh_addr_of(out_pos), lh_str_scanf_spec_get_literal(lh_addr_of(spec)),
                                                 lh_str_scanf_spec_get_literal_size(lh_addr_of(spec)));
        }
        else if (lh_math_eq(lh_str_scanf_spec_get_kind(lh_addr_of(spec)), lh_str_scanf_spec_kind_invalid) ||
                 !lh_str_ptr_format_text_get_value(lh_addr_of(spec), lh_addr_of(ap), buf,
                                                   lh_addr_of(content), lh_addr_of(size)))
        {
            va_end(ap);
            return failed;
        }
        else
        {
            ok = lh_str_ptr_format_text_put_field(str, str_size, lh_addr_of(out_pos),
                                                  lh_addr_of(spec), content, size);
        }
        if (!ok)
        {
            va_end(ap);
            return 0U;
        }
    }
    va_end(ap);
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

lh_usize_t
lh_str_ptr_format_cstr_v(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, va_list args)
{
    lh_usize_t size;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_if(str_size == 0U, lh_runtime_error_code_invalid_argument);
    size = lh_str_ptr_format_text_v(str, str_size - 1U, fmt, args);
    str[size] = lh_char_map_nul;
    return size;
}

lh_usize_t
lh_str_ptr_format_cstr(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, ...)
{
    va_list args;
    lh_usize_t size;

    va_start(args, fmt);
    size = lh_str_ptr_format_cstr_v(str, str_size, fmt, args);
    va_end(args);
    return size;
}
