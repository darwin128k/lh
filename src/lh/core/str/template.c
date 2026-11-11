/**
 * @file template.c
 * @brief `{name}` expansion over a lookup.
 */

#include <lh/core/str/template.h>
#include <lh/core/char/word.h>
#include <lh/core/math.h>

lh_bool_t
lh_str_template_is_name(const lh_str_view_t *name)
{
    const lh_usize_t size = lh_str_view_get_size(name);

    if (lh_math_is_zero(size))
    {
        return lh_bool_false;
    }
    for (lh_usize_t i = 0U; lh_math_lt(i, size); ++i)
    {
        if (!lh_char_is_word(lh_str_view_get_char_from_begin(name, i)))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_str_template_try_expand(const lh_str_view_t *pattern, lh_str_template_lookup_cb lookup,
                           lh_ptr user, lh_str_t *out)
{
    const lh_char_t brace_chars[2] = {lh_char_map_lbrace, lh_char_map_rbrace};

    lh_assert_runtime_ref(lookup);
    lh_assert_runtime_ref(out);

    lh_str_view_t braces;

    lh_str_view_init_by_size(lh_addr_of(braces), brace_chars, sizeof(brace_chars));

    lh_str_view_t rest;

    lh_str_view_init_by_other(lh_addr_of(rest), pattern);
    lh_str_clear(out);
    while (!lh_str_view_is_empty(lh_addr_of(rest)))
    {
        const lh_usize_t at = lh_str_view_find_of(lh_addr_of(rest), lh_addr_of(braces));

        if (lh_math_eq(at, LH_STR_VIEW_INVALID))
        {
            lh_str_append_view(out, rest);
            break;
        }

        lh_str_view_t chunk;

        lh_str_view_from_offset(lh_addr_of(rest), 0U, at, lh_addr_of(chunk));
        lh_str_append_view(out, chunk);
        if (lh_math_eq(lh_str_view_get_char_from_begin(lh_addr_of(rest), at), lh_char_map_rbrace))
        {
            lh_str_clear(out);
            return lh_bool_false;
        }

        lh_str_view_t token;

        lh_str_view_tail(lh_addr_of(rest), lh_math_add_one(at), lh_addr_of(token));

        const lh_usize_t close = lh_str_view_find_char(lh_addr_of(token), lh_char_map_rbrace);
        lh_str_view_t name;
        lh_str_view_t value;

        if (lh_math_eq(close, LH_STR_VIEW_INVALID))
        {
            lh_str_clear(out);
            return lh_bool_false;
        }
        lh_str_view_from_offset(lh_addr_of(token), 0U, close, lh_addr_of(name));
        if (!lookup(user, lh_addr_of(name), lh_addr_of(value)))
        {
            lh_str_clear(out);
            return lh_bool_false;
        }
        lh_str_append_view(out, value);
        lh_str_view_tail(lh_addr_of(token), lh_math_add_one(close), lh_addr_of(rest));
    }
    return lh_bool_true;
}
