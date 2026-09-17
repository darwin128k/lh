/**
 * @file uint.h
 * @brief Parse an unsigned decimal integer from text.
 */

#ifndef LH_STR_PARSE_UINT_H
#define LH_STR_PARSE_UINT_H

#include <lh/assert.h>
#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/char/digit.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse an unsigned decimal integer, rejecting values above @p max.
 *
 * Accepts only ASCII digits `0`-`9`; all of @p str_size must be consumed.
 * Leading zeros are allowed (`09`). Empty input is rejected. Safe against
 * ::lh_uint_t overflow regardless of @p max.
 *
 * The digit loop shared by date/time (`09/05`) and by ::lh_str_ptr_parse_uint
 * (which then rejects padding).
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a decimal
 *         integer not exceeding @p max, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_parse_uint_digits(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    lh_uint_t value = 0;
    lh_usize_t pos = 0;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (str_size == 0)
    {
        return lh_bool_false;
    }

    while (pos < str_size && lh_char_is_digit(str[pos]))
    {
        if (!lh_char_digit_accumulate(lh_addr_of(value), lh_char_to_digit(str[pos])))
        {
            return lh_bool_false;
        }
        if (value > max)
        {
            return lh_bool_false;
        }
        pos++;
    }

    if (pos != str_size)
    {
        return lh_bool_false;
    }

    *out = value;
    return lh_bool_true;
}

/**
 * @brief Parse an unsigned decimal integer without padded leading zeros.
 *
 * Same digit loop as ::lh_str_ptr_parse_uint_digits, then rejects a leading
 * zero on input longer than one digit (e.g. `0080`). Used by IPv4/port.
 *
 * ::LH_ATTRIBUTE_FORCE_INLINE — this is the digit loop under
 * ::lh_net_ip4_parse; see the note there for why.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 *
 * @return ::lh_bool_true if the field is a decimal integer not exceeding
 *         @p max and not zero-padded, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_parse_uint(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (str_size > 1U && str[0] == '0')
    {
        return lh_bool_false;
    }

    return lh_str_ptr_parse_uint_digits(str, str_size, max, out);
}

/**
 * @brief Next @p delim-separated field, parsed as a padded unsigned integer.
 *
 * ::lh_str_ptr_split_next then ::lh_str_ptr_parse_uint_digits. Date and time
 * parse walk `Y/M/D` and `H:M:S` through this instead of a private copy of
 * both helpers.
 *
 * @param str       Buffer to scan.
 * @param str_size  Total characters in @p str.
 * @param delim     Field delimiter (`/` or `:`).
 * @param pos       In/out cursor. Start at `0` for the first field.
 * @param max       Inclusive maximum for this field.
 * @param out       Receives the parsed value on success.
 * @param had_delim Out: whether a delimiter followed the field.
 *
 * @return ::lh_bool_true if a field was split and parsed, ::lh_bool_false
 *         otherwise.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_split_next_uint_digits(lh_str_cptr str, lh_usize_t str_size, lh_char_t delim,
                                  lh_usize_t *pos, lh_uint_t max, lh_uint_t *out,
                                  lh_bool_t *had_delim)
{
    lh_str_cptr field;
    lh_usize_t field_size;

    if (!lh_str_ptr_split_next(str, str_size, delim, pos, lh_addr_of(field),
                               lh_addr_of(field_size), had_delim))
    {
        return lh_bool_false;
    }

    return lh_str_ptr_parse_uint_digits(field, field_size, max, out);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_UINT_H */
