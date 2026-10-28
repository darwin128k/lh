#include <lh/str/parse/text.h>
#include <lh/assert.h>
#include <lh/memory/std.h>
#include <lh/str/parse/hex.h>
#include <lh/str/parse/uint.h>
#include <lh/str/scanf/next.h>
#include <lh/util/str/ptr.h>
#include <lh/math.h>
#include <lh/char/map.h>
#include <lh/util/return.h>

lh_bool_t
lh_str_ptr_parse_text_literal(lh_str_cptr str, lh_usize_t str_size, lh_usize_t *pos,
                              const lh_str_scanf_spec_t *spec)
{
    const lh_usize_t at = lh_ptr_deref(pos);
    const lh_usize_t size = lh_str_scanf_spec_get_literal_size(spec);

    lh_return_if(lh_math_gt(size, lh_math_sub(str_size, at)), lh_bool_false);
    lh_return_if(lh_ptr_is_set(lh_memory_std_compare(lh_ptr_add_by_offset_unsafe(const lh_char_t, str, at),
                                                      lh_str_scanf_spec_get_literal(spec), size)),
                 lh_bool_false);
    lh_ptr_deref(pos) = lh_math_add(at, size);
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_parse_text_percent(lh_str_cptr str, lh_usize_t str_size, lh_usize_t *pos)
{
    const lh_usize_t at = lh_ptr_deref(pos);

    lh_return_if(lh_math_ge(at, str_size) || lh_math_ne(str[at], lh_char_map_percent), lh_bool_false);
    lh_ptr_deref(pos) = lh_math_add_one(at);
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_parse_text_char(lh_str_cptr str, lh_usize_t *pos, lh_usize_t limit, va_list *args)
{
    lh_char_t *out = va_arg(lh_ptr_deref(args), lh_char_t *);
    const lh_usize_t at = lh_ptr_deref(pos);

    lh_assert_runtime_ref(out);
    lh_return_if(lh_math_is_zero(limit), lh_bool_false);
    lh_ptr_deref(out) = str[at];
    lh_ptr_deref(pos) = lh_math_add_one(at);
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_parse_text_str(lh_str_cptr str, lh_usize_t str_size, lh_usize_t *pos,
                          const lh_str_scanf_spec_t *spec, lh_usize_t limit, lh_str_cptr fmt,
                          lh_usize_t fmt_pos, va_list *args)
{
    lh_str_ptr out = va_arg(lh_ptr_deref(args), lh_str_ptr);
    const lh_usize_t out_size = va_arg(lh_ptr_deref(args), lh_usize_t);
    const lh_usize_t at = lh_ptr_deref(pos);
    lh_str_cptr from = lh_ptr_add_by_offset_unsafe(const lh_char_t, str, at);
    lh_usize_t field_size = limit;
    lh_str_scanf_spec_t next;

    lh_assert_runtime_ref(out);
    if (lh_str_scanf_spec_get_have_precision(spec))
    {
        field_size =
            lh_math_min(field_size, lh_cast_static(lh_usize_t, lh_str_scanf_spec_get_precision(spec)));
    }

    /* A literal right after %s ends the field where it starts. */
    if (lh_str_ptr_scanf_next(fmt, lh_addr_of(fmt_pos), lh_addr_of(next)) &&
        lh_math_eq(lh_str_scanf_spec_get_kind(lh_addr_of(next)), lh_str_scanf_spec_kind_literal) &&
        lh_math_gt(lh_str_scanf_spec_get_literal_size(lh_addr_of(next)), 0U))
    {
        const lh_str_cptr found =
            lh_str_ptr_find(from, lh_math_sub(str_size, at), lh_str_scanf_spec_get_literal(lh_addr_of(next)),
                            lh_str_scanf_spec_get_literal_size(lh_addr_of(next)), lh_bool_false);
        lh_usize_t offset;

        lh_return_ifn(found, lh_bool_false);
        offset = lh_ptr_udiff(found, from);
        lh_return_if(lh_math_gt(offset, field_size), lh_bool_false);
        field_size = offset;
    }

    lh_return_if(lh_math_gt(field_size, out_size), lh_bool_false);
    lh_memory_std_copy(out, from, field_size);
    lh_ptr_deref(pos) = lh_math_add(at, field_size);
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_parse_text_uint(lh_str_cptr str, lh_usize_t *pos, lh_usize_t limit, va_list *args)
{
    lh_uint_t *out = va_arg(lh_ptr_deref(args), lh_uint_t *);
    const lh_usize_t at = lh_ptr_deref(pos);
    lh_uint_t value;
    lh_usize_t consumed;

    lh_assert_runtime_ref(out);
    consumed = lh_str_ptr_parse_uint_prefix(lh_ptr_add_by_offset_unsafe(const lh_char_t, str, at), limit,
                                            LH_UINT_T_MAX, lh_addr_of(value));
    lh_return_if(lh_math_is_zero(consumed), lh_bool_false);
    lh_ptr_deref(out) = value;
    lh_ptr_deref(pos) = lh_math_add(at, consumed);
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_parse_text_sint(lh_str_cptr str, lh_usize_t *pos, lh_usize_t limit, va_list *args)
{
    lh_sint_t *out = va_arg(lh_ptr_deref(args), lh_sint_t *);
    /* |LH_SINT_T_MIN|, which LH_SINT_T_MAX is one short of. */
    const lh_uint_t min_magnitude = lh_math_sub(0U, lh_cast_static(lh_uint_t, LH_SINT_T_MIN));
    lh_usize_t at = lh_ptr_deref(pos);
    lh_usize_t digit_limit = limit;
    lh_bool_t is_negative = lh_bool_false;
    lh_uint_t magnitude;
    lh_usize_t consumed;

    lh_assert_runtime_ref(out);
    lh_return_if(lh_math_is_zero(limit), lh_bool_false);
    if (lh_math_eq(str[at], lh_char_map_minus))
    {
        is_negative = lh_bool_true;
        at = lh_math_add_one(at);
        digit_limit = lh_math_sub_one(limit);
    }
    consumed = lh_str_ptr_parse_uint_prefix(
        lh_ptr_add_by_offset_unsafe(const lh_char_t, str, at), digit_limit,
        is_negative ? min_magnitude : lh_cast_static(lh_uint_t, LH_SINT_T_MAX), lh_addr_of(magnitude));
    lh_return_if(lh_math_is_zero(consumed), lh_bool_false);

    if (!is_negative)
    {
        lh_ptr_deref(out) = lh_cast_static(lh_sint_t, magnitude);
    }
    else if (lh_math_eq(magnitude, min_magnitude))
    {
        lh_ptr_deref(out) = LH_SINT_T_MIN;
    }
    else
    {
        lh_ptr_deref(out) = lh_math_neg(lh_cast_static(lh_sint_t, magnitude));
    }
    lh_ptr_deref(pos) = lh_math_add(at, consumed);
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_parse_text_hex(lh_str_cptr str, lh_usize_t *pos, lh_usize_t limit, va_list *args)
{
    lh_uint_t *out = va_arg(lh_ptr_deref(args), lh_uint_t *);
    const lh_usize_t at = lh_ptr_deref(pos);
    lh_uint_t value;
    lh_usize_t consumed;

    lh_assert_runtime_ref(out);
    consumed = lh_str_ptr_parse_hex_prefix(lh_ptr_add_by_offset_unsafe(const lh_char_t, str, at), limit,
                                           LH_UINT_T_MAX, lh_addr_of(value));
    lh_return_if(lh_math_is_zero(consumed), lh_bool_false);
    lh_ptr_deref(out) = value;
    lh_ptr_deref(pos) = lh_math_add(at, consumed);
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_parse_text_field(lh_str_cptr str, lh_usize_t str_size, lh_usize_t *pos,
                            const lh_str_scanf_spec_t *spec, lh_str_cptr fmt, lh_usize_t fmt_pos,
                            va_list *args)
{
    const lh_usize_t remaining = lh_math_sub(str_size, lh_ptr_deref(pos));
    const lh_usize_t limit =
        (lh_str_scanf_spec_get_width(spec) > 0U && lh_str_scanf_spec_get_width(spec) < remaining) ? lh_str_scanf_spec_get_width(spec) : remaining;

    switch (lh_str_scanf_spec_get_kind(spec))
    {
    case lh_str_scanf_spec_kind_literal:
        return lh_str_ptr_parse_text_literal(str, str_size, pos, spec);
    case lh_str_scanf_spec_kind_percent:
        return lh_str_ptr_parse_text_percent(str, str_size, pos);
    case lh_str_scanf_spec_kind_char:
        return lh_str_ptr_parse_text_char(str, pos, limit, args);
    case lh_str_scanf_spec_kind_str:
        return lh_str_ptr_parse_text_str(str, str_size, pos, spec, limit, fmt, fmt_pos, args);
    case lh_str_scanf_spec_kind_uint:
        return lh_str_ptr_parse_text_uint(str, pos, limit, args);
    case lh_str_scanf_spec_kind_sint:
        return lh_str_ptr_parse_text_sint(str, pos, limit, args);
    case lh_str_scanf_spec_kind_hex:
        return lh_str_ptr_parse_text_hex(str, pos, limit, args);
    default:
        return lh_bool_false;
    }
}

lh_usize_t
lh_str_ptr_parse_text_v(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args)
{
    lh_usize_t in_pos = 0U;
    lh_usize_t fmt_pos = 0U;
    lh_bool_t matched = lh_bool_true;
    lh_str_scanf_spec_t spec;
    va_list ap;

    lh_assert_runtime_ref(str);
    va_copy(ap, args);
    while (matched && lh_str_ptr_scanf_next(fmt, lh_addr_of(fmt_pos), lh_addr_of(spec)))
    {
        matched = lh_str_ptr_parse_text_field(str, str_size, lh_addr_of(in_pos), lh_addr_of(spec), fmt,
                                              fmt_pos, lh_addr_of(ap));
    }
    va_end(ap);
    return matched && lh_math_eq(in_pos, str_size) ? in_pos : 0U;
}

lh_usize_t
lh_str_ptr_parse_text(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr fmt, ...)
{
    lh_usize_t result;
    va_list args;

    va_start(args, fmt);
    result = lh_str_ptr_parse_text_v(str, str_size, fmt, args);
    va_end(args);

    return result;
}
