#include <lh/core/str/scanf/next.h>
#include <lh/core/assert.h>
#include <lh/core/char/digit.h>
#include <lh/core/math.h>

lh_void
lh_str_ptr_scanf_next_literal(lh_str_cptr fmt, lh_usize_t *pos, lh_str_scanf_spec_t *out)
{
    lh_assert_runtime_ref(fmt);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(out);
    {
        const lh_usize_t run_start = lh_ptr_deref(pos);
        lh_usize_t fmt_pos = run_start;

        while (lh_math_ne(fmt[fmt_pos], lh_char_map_nul) &&
               lh_math_ne(fmt[fmt_pos], lh_char_map_percent))
        {
            fmt_pos = lh_math_add_one(fmt_pos);
        }
        lh_str_scanf_spec_set_kind(out, lh_str_scanf_spec_kind_literal);
        lh_str_scanf_spec_set_literal(out, lh_ptr_add_by_offset(const lh_char_t, fmt, run_start));
        lh_str_scanf_spec_set_literal_size(out, lh_math_sub(fmt_pos, run_start));
        lh_ptr_deref(pos) = fmt_pos;
    }
}

lh_void
lh_str_ptr_scanf_next_flags(lh_str_cptr fmt, lh_usize_t *pos, lh_str_scanf_spec_t *out)
{
    lh_assert_runtime_ref(fmt);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(out);
    {
        lh_usize_t fmt_pos = lh_ptr_deref(pos);

        for (;;)
        {
            if (lh_math_eq(fmt[fmt_pos], lh_char_map_digit_0))
            {
                lh_str_scanf_spec_set_zero_pad(out, lh_bool_true);
            }
            else if (lh_math_eq(fmt[fmt_pos], lh_char_map_minus))
            {
                lh_str_scanf_spec_set_left_justify(out, lh_bool_true);
            }
            else
            {
                break;
            }
            fmt_pos = lh_math_add_one(fmt_pos);
        }
        lh_ptr_deref(pos) = fmt_pos;
    }
}

lh_bool_t
lh_str_ptr_scanf_next_number(lh_str_cptr fmt, lh_usize_t *pos, lh_uint_t *value)
{
    lh_assert_runtime_ref(fmt);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(value);
    {
        lh_usize_t fmt_pos = lh_ptr_deref(pos);
        lh_uint_t number = 0U;
        lh_bool_t fits = lh_bool_true;

        while (lh_char_is_digit(fmt[fmt_pos]))
        {
            if (!lh_char_digit_accumulate(lh_addr_of(number), lh_char_to_digit(fmt[fmt_pos])))
            {
                fits = lh_bool_false;
                break;
            }
            fmt_pos = lh_math_add_one(fmt_pos);
        }
        lh_ptr_deref(value) = number;
        lh_ptr_deref(pos) = fmt_pos;
        return fits;
    }
}

lh_void
lh_str_ptr_scanf_next_length(lh_str_cptr fmt, lh_usize_t *pos, lh_str_scanf_spec_t *out)
{
    lh_usize_t fmt_pos;
    lh_char_t ch;

    lh_assert_runtime_ref(fmt);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(out);
    fmt_pos = lh_ptr_deref(pos);
    ch = fmt[fmt_pos];
    if (lh_math_eq(ch, lh_char_map_small_h) || lh_math_eq(ch, lh_char_map_small_l))
    {
        const lh_bool_t twice = lh_math_eq(fmt[lh_math_add_one(fmt_pos)], ch);

        lh_str_scanf_spec_set_length(
            out,
            lh_math_eq(ch, lh_char_map_small_h)
                ? (twice ? lh_str_scanf_spec_length_char : lh_str_scanf_spec_length_short)
                : (twice ? lh_str_scanf_spec_length_long_long : lh_str_scanf_spec_length_long));
        lh_ptr_deref(pos) = lh_math_add(fmt_pos, twice ? 2U : 1U);
    }
    else if (lh_math_eq(ch, lh_char_map_small_j))
    {
        lh_str_scanf_spec_set_length(out, lh_str_scanf_spec_length_long_long);
        lh_ptr_deref(pos) = lh_math_add_one(fmt_pos);
    }
    else if (lh_math_eq(ch, lh_char_map_small_z) || lh_math_eq(ch, lh_char_map_small_t))
    {
        lh_str_scanf_spec_set_length(out, lh_str_scanf_spec_length_size);
        lh_ptr_deref(pos) = lh_math_add_one(fmt_pos);
    }
}

lh_bool_t
lh_str_ptr_scanf_next(lh_str_cptr fmt, lh_usize_t *pos, lh_str_scanf_spec_t *out)
{
    lh_assert_runtime_ref(fmt);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(out);
    {
        lh_usize_t fmt_pos = lh_ptr_deref(pos);
        lh_uint_t number;
        lh_char_t ch;

        /* kind_invalid is 0; wipe + keep fmt as the default literal base. */
        lh_ptr_deref(out) = (lh_str_scanf_spec_t){0};
        lh_str_scanf_spec_set_literal(out, fmt);

        ch = fmt[fmt_pos];
        if (lh_math_eq(ch, lh_char_map_nul))
        {
            return lh_bool_false;
        }
        if (lh_math_ne(ch, lh_char_map_percent))
        {
            lh_str_ptr_scanf_next_literal(fmt, pos, out);
            return lh_bool_true;
        }

        fmt_pos = lh_math_add_one(fmt_pos); /* consume '%' */
        ch = fmt[fmt_pos];

        /*
         * Common masks: "%u", "%d", "%x", "%%" — no flags/width/precision.
         * Skip the number/flag scanner entirely.
         */
        if (lh_math_ne(ch, lh_char_map_digit_0) && lh_math_ne(ch, lh_char_map_minus) &&
            lh_math_ne(ch, lh_char_map_period) && lh_math_ne(ch, lh_char_map_nul) &&
            !lh_char_is_digit(ch))
        {
            lh_str_ptr_scanf_next_length(fmt, lh_addr_of(fmt_pos), out);
            lh_str_scanf_spec_set_conversion(out, fmt[fmt_pos]);
            lh_ptr_deref(pos) =
                lh_math_eq(fmt[fmt_pos], lh_char_map_nul) ? fmt_pos : lh_math_add_one(fmt_pos);
            return lh_bool_true;
        }

        lh_str_ptr_scanf_next_flags(fmt, lh_addr_of(fmt_pos), out);

        if (!lh_str_ptr_scanf_next_number(fmt, lh_addr_of(fmt_pos), lh_addr_of(number)))
        {
            lh_ptr_deref(pos) = fmt_pos;
            return lh_bool_true; /* kind stays invalid */
        }
        lh_str_scanf_spec_set_width(out, number);

        if (lh_math_eq(fmt[fmt_pos], lh_char_map_period))
        {
            fmt_pos = lh_math_add_one(fmt_pos);
            lh_str_scanf_spec_set_have_precision(out, lh_bool_true);
            if (!lh_str_ptr_scanf_next_number(fmt, lh_addr_of(fmt_pos), lh_addr_of(number)))
            {
                lh_ptr_deref(pos) = fmt_pos;
                return lh_bool_true;
            }
            lh_str_scanf_spec_set_precision(out, number);
        }

        lh_str_ptr_scanf_next_length(fmt, lh_addr_of(fmt_pos), out);
        if (lh_math_eq(fmt[fmt_pos], lh_char_map_nul))
        {
            lh_ptr_deref(pos) = fmt_pos;
            return lh_bool_true; /* '%' at the end, no conversion */
        }
        lh_str_scanf_spec_set_conversion(out, fmt[fmt_pos]);
        lh_ptr_deref(pos) = lh_math_add_one(fmt_pos);
        return lh_bool_true;
    }
}
