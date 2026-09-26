#include <lh/str/parse/text.h>
#include <lh/assert.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/memory/std.h>
#include <lh/null.h>
#include <lh/numeric/limits.h>
#include <lh/numeric/types.h>
#include <lh/str/parse/hex.h>
#include <lh/str/parse/uint.h>
#include <lh/str/scanf/next.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/str/ptr.h>

static lh_usize_t
lh_str_ptr_parse_text_limit(lh_usize_t remaining, const lh_str_scanf_spec_t *spec)
{
    lh_usize_t limit = remaining;

    if (spec->width > 0U && spec->width < limit)
    {
        limit = spec->width;
    }
    return limit;
}

lh_usize_t
lh_str_ptr_parse_text_v(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args)
{
    lh_usize_t in_pos = 0;
    lh_usize_t fmt_pos = 0;
    lh_str_scanf_spec_t spec;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(fmt);

    while (lh_str_ptr_scanf_next(fmt, lh_addr_of(fmt_pos), lh_addr_of(spec)))
    {
        lh_usize_t remaining;
        lh_usize_t limit;
        lh_usize_t consumed;

        if (spec.kind == lh_str_scanf_spec_kind_invalid)
        {
            return 0;
        }

        remaining = str_size - in_pos;
        limit = lh_str_ptr_parse_text_limit(remaining, lh_addr_of(spec));

        if (spec.kind == lh_str_scanf_spec_kind_literal)
        {
            if (spec.literal_size > remaining)
            {
                return 0;
            }
            if (lh_memory_std_compare(str + in_pos, spec.literal, spec.literal_size) != lh_null)
            {
                return 0;
            }
            in_pos += spec.literal_size;
            continue;
        }

        switch (spec.kind)
        {
        case lh_str_scanf_spec_kind_percent:
            if (remaining == 0U || str[in_pos] != '%')
            {
                return 0;
            }
            in_pos++;
            break;
        case lh_str_scanf_spec_kind_char:
        {
            lh_char_t *out = va_arg(args, lh_char_t *);

            lh_assert_runtime_ref(out);
            if (limit == 0U)
            {
                return 0;
            }
            *out = str[in_pos];
            in_pos++;
            break;
        }
        case lh_str_scanf_spec_kind_str:
        {
            lh_str_ptr out = va_arg(args, lh_str_ptr);
            lh_usize_t out_size = va_arg(args, lh_usize_t);
            lh_usize_t field_size = limit;
            lh_usize_t peek_pos = fmt_pos;
            lh_str_scanf_spec_t next;

            lh_assert_runtime_ref(out);

            if (spec.have_precision && spec.precision < field_size)
            {
                field_size = spec.precision;
            }

            if (lh_str_ptr_scanf_next(fmt, lh_addr_of(peek_pos), lh_addr_of(next)) &&
                next.kind == lh_str_scanf_spec_kind_literal && next.literal_size > 0U)
            {
                const lh_str_ptr found =
                    lh_str_ptr_find(str + in_pos, remaining, next.literal, next.literal_size,
                                    lh_bool_false);
                lh_usize_t offset;

                if (found == lh_null)
                {
                    return 0;
                }
                offset = lh_cast_static(lh_usize_t, (found - (str + in_pos)));
                if (offset > field_size)
                {
                    return 0;
                }
                field_size = offset;
            }

            if (field_size > out_size)
            {
                return 0;
            }
            lh_memory_std_copy(out, str + in_pos, field_size);
            in_pos += field_size;
            break;
        }
        case lh_str_scanf_spec_kind_uint:
        {
            lh_uint_t *out = va_arg(args, lh_uint_t *);
            lh_uint_t value;

            lh_assert_runtime_ref(out);
            consumed =
                lh_str_ptr_parse_uint_prefix(str + in_pos, limit, LH_UINT_T_MAX, lh_addr_of(value));
            if (lh_math_is_zero(consumed))
            {
                return 0;
            }
            in_pos = lh_math_add(in_pos, consumed);
            *out = value;
            break;
        }
        case lh_str_scanf_spec_kind_sint:
        {
            lh_sint_t *out = va_arg(args, lh_sint_t *);
            lh_bool_t is_negative = lh_bool_false;
            lh_uint_t magnitude;
            lh_uint_t max;
            lh_usize_t digit_limit = limit;

            lh_assert_runtime_ref(out);
            if (limit == 0U)
            {
                return 0;
            }
            if (str[in_pos] == '-')
            {
                is_negative = lh_bool_true;
                in_pos++;
                digit_limit = limit - 1U;
            }
            max = is_negative ? (0U - lh_cast_static(lh_uint_t, LH_SINT_T_MIN)) : lh_cast_static(lh_uint_t, LH_SINT_T_MAX);
            consumed =
                lh_str_ptr_parse_uint_prefix(str + in_pos, digit_limit, max, lh_addr_of(magnitude));
            if (lh_math_is_zero(consumed))
            {
                return 0;
            }
            in_pos = lh_math_add(in_pos, consumed);
            if (is_negative)
            {
                *out = (magnitude == (0U - lh_cast_static(lh_uint_t, LH_SINT_T_MIN))) ? LH_SINT_T_MIN
                                                                      : -lh_cast_static(lh_sint_t, magnitude);
            }
            else
            {
                *out = lh_cast_static(lh_sint_t, magnitude);
            }
            break;
        }
        case lh_str_scanf_spec_kind_hex:
        {
            lh_uint_t *out = va_arg(args, lh_uint_t *);
            lh_uint_t value;

            lh_assert_runtime_ref(out);
            consumed =
                lh_str_ptr_parse_hex_prefix(str + in_pos, limit, LH_UINT_T_MAX, lh_addr_of(value));
            if (lh_math_is_zero(consumed))
            {
                return 0;
            }
            in_pos = lh_math_add(in_pos, consumed);
            *out = value;
            break;
        }
        default:
            return 0;
        }
    }

    return in_pos == str_size ? in_pos : 0;
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
