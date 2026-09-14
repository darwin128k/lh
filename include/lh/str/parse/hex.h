/**
 * @file hex.h
 * @brief Parse an unsigned hexadecimal integer from text.
 */

#ifndef LH_STR_PARSE_HEX_H
#define LH_STR_PARSE_HEX_H

#include <lh/assert.h>
#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/char/xdigit.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/util/addr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse an unsigned hexadecimal integer, rejecting values above @p max.
 *
 * Accepts ASCII digits `0`-`9` and letters `a`-`f` / `A`-`F`; all of
 * @p str_size must be consumed. Rejects empty input. Unlike
 * ::lh_str_ptr_parse_uint, leading zeros are allowed — padded hex (`00FF`)
 * is the usual written form. No `0x` prefix.
 *
 * Safe against ::lh_uint_t overflow regardless of @p max.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a hexadecimal
 *         integer not exceeding @p max, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_parse_hex(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    lh_uint_t value = 0;
    lh_usize_t pos = 0;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (str_size == 0)
    {
        return lh_bool_false;
    }

    while (pos < str_size && lh_char_is_xdigit(str[pos]))
    {
        if (!lh_char_xdigit_accumulate(lh_addr_of(value), lh_char_to_xdigit(str[pos])))
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

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_HEX_H */
