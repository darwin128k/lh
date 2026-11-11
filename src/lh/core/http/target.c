#include <lh/core/http/target.h>
#include <lh/core/assert.h>
#include <lh/core/char/digit.h>
#include <lh/core/http/char.h>
#include <lh/core/str/parse/hex.h>

/* Where the path of a request target ends: the query or the fragment. */
static const lh_char_t m_http_target_path_ends[] = {lh_char_map_question, lh_char_map_hash};

/* What a component may not end in: Windows trims both. */
static const lh_char_t m_http_target_trimmed[] = {lh_char_map_period, lh_char_map_space};

lh_usize_t
lh_http_target_try_decode_path(const lh_str_view_t *target, lh_str_ptr out, lh_usize_t capacity,
                               lh_http_target_error_t *error)
{
    lh_assert_runtime_ref(error);
    lh_assert_runtime_if(lh_math_is_zero(capacity), lh_runtime_error_code_invalid_argument);

    const lh_char_t *text = lh_str_view_get_data(target);
    lh_usize_t size = lh_str_view_get_size(target);
    /* The query and fragment never name a file. */
    const lh_char_t *end = lh_str_ptr_find_of_chars(text, size, m_http_target_path_ends,
                                                    lh_array_ptr_get_size(m_http_target_path_ends));
    if (lh_ptr_is_set(end))
    {
        size = lh_cast_static(lh_usize_t, lh_ptr_udiff(end, text));
    }
    if (lh_math_lt(size, 2U) || lh_math_ne(text[0], lh_char_map_slash))
    {
        lh_ptr_deref(error) = lh_http_target_error_bad_target;
        lh_str_ptr_clear(out);
        return 0U;
    }
    /* Decode exactly once; an encoded separator or NUL never becomes one. */
    lh_usize_t n = 0U;
    for (lh_usize_t i = 1U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        lh_byte_t byte = lh_cast_static(lh_byte_t, text[i]);
        if (lh_math_eq(byte, lh_char_map_percent))
        {
            if (lh_math_ge(lh_math_add(i, 2U), size) ||
                !lh_str_ptr_parse_hex_bytes(text + lh_math_add_one(i), 2U, lh_addr_of(byte), 1U))
            {
                lh_ptr_deref(error) = lh_http_target_error_bad_encoding;
                lh_str_ptr_clear(out);
                return 0U;
            }
            i = lh_math_add(i, 2U);
            if (lh_math_eq(byte, lh_char_map_slash) || lh_math_eq(byte, lh_char_map_nul))
            {
                lh_ptr_deref(error) = lh_http_target_error_encoded_separator;
                lh_str_ptr_clear(out);
                return 0U;
            }
        }
        const lh_char_t ch = lh_cast_static(lh_char_t, byte);
        if (lh_math_ne(ch, lh_char_map_slash) && !lh_http_is_name_char(ch))
        {
            lh_ptr_deref(error) = lh_http_target_error_bad_char;
            lh_str_ptr_clear(out);
            return 0U;
        }
        if (lh_math_ge(lh_math_add_one(n), capacity))
        {
            lh_ptr_deref(error) = lh_http_target_error_too_long;
            lh_str_ptr_clear(out);
            return 0U;
        }
        out[n] = ch;
        n = lh_math_add_one(n);
    }
    lh_str_ptr_erase(out, n);
    lh_ptr_deref(error) = lh_http_target_error_none;
    return n;
}

lh_http_target_error_t
lh_http_target_get_path_error(const lh_str_view_t *path)
{
    const lh_char_t *text = lh_str_view_get_data(path);
    const lh_usize_t size = lh_str_view_get_size(path);
    lh_usize_t start = 0U;
    for (lh_usize_t i = 0U; lh_math_le(i, size); i = lh_math_add_one(i))
    {
        if (lh_math_lt(i, size) && lh_math_ne(text[i], lh_char_map_slash))
        {
            if (lh_math_eq(text[i], lh_char_map_tilde) && lh_math_lt(lh_math_add_one(i), size) &&
                lh_char_is_digit(text[lh_math_add_one(i)]))
            {
                return lh_http_target_error_short_name;
            }
            continue;
        }
        if (lh_math_eq(i, start))
        {
            return lh_http_target_error_empty_component;
        }
        if (lh_math_eq(text[start], lh_char_map_period))
        {
            return lh_http_target_error_hidden_or_dot;
        }
        if (lh_str_ptr_contains_char(m_http_target_trimmed,
                                     lh_array_ptr_get_size(m_http_target_trimmed),
                                     text[lh_math_sub_one(i)]))
        {
            return lh_http_target_error_trailing_dot_or_space;
        }
        start = lh_math_add_one(i);
    }
    return lh_http_target_error_none;
}

lh_usize_t
lh_http_target_try_copy_path(const lh_str_view_t *target, lh_str_ptr out, lh_usize_t capacity,
                             lh_http_target_error_t *error)
{
    const lh_usize_t n = lh_http_target_try_decode_path(target, out, capacity, error);
    if (lh_math_is_zero(n))
    {
        return 0U;
    }
    lh_str_view_t path;
    lh_str_view_init_by_size(lh_addr_of(path), out, n);
    lh_ptr_deref(error) = lh_http_target_get_path_error(lh_addr_of(path));
    if (lh_math_ne(lh_ptr_deref(error), lh_http_target_error_none))
    {
        lh_str_ptr_clear(out);
        return 0U;
    }
    return n;
}
