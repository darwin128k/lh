/**
 * @file xdigit.h
 * @brief Single ASCII hexadecimal digit <-> numeric value (0-15) conversion.
 *
 * ::lh_char_is_xdigit / ::lh_char_to_xdigit / ::lh_char_from_xdigit and
 * ::LH_CHAR_XDIGIT_RADIX are hex-only — the decimal counterparts live in
 * `lh/char/digit.h`. ::lh_char_xdigit_accumulate is the overflow-checked
 * fold used by ::lh_str_ptr_parse_hex.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE — small and called
 * from other force-inlined parsers (::lh_str_ptr_parse_hex).
 */

#ifndef LH_CHAR_XDIGIT_H
#define LH_CHAR_XDIGIT_H

#include <lh/assert.h>
#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/runtime/error.h>
#include <lh/util/char.h>
#include <lh/util/numeric.h>

/**
 * @def LH_CHAR_XDIGIT_RADIX
 * @brief Base of the hexadecimal digits ::lh_char_is_xdigit/::lh_char_to_xdigit/
 *        ::lh_char_from_xdigit work with (16).
 */
#define LH_CHAR_XDIGIT_RADIX 16U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p ch is an ASCII hexadecimal digit (`0`-`9`, `a`-`f`,
 *        `A`-`F`).
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is a hex digit, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_xdigit(lh_char_t ch)
{
    return ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
               ? lh_bool_true
               : lh_bool_false;
}

/**
 * @brief Convert an ASCII hexadecimal digit to its numeric value.
 *
 * @param ch Digit character; must satisfy ::lh_char_is_xdigit.
 * @return Value in `[0, 15]`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_uchar_t
lh_char_to_xdigit(lh_char_t ch)
{
    lh_assert_runtime_if(!lh_char_is_xdigit(ch),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    if (ch >= '0' && ch <= '9')
    {
        return lh_cast_static(lh_uchar_t, (lh_char_ord(ch) - lh_char_ord('0')));
    }
    if (ch >= 'a' && ch <= 'f')
    {
        return lh_cast_static(lh_uchar_t, (lh_char_ord(ch) - lh_char_ord('a') + 10));
    }
    return lh_cast_static(lh_uchar_t, (lh_char_ord(ch) - lh_char_ord('A') + 10));
}

/**
 * @brief Convert a numeric value to its ASCII hexadecimal digit.
 *
 * @param digit     Value in `[0, 15]`.
 * @param uppercase ::lh_bool_true for `A`-`F`, ::lh_bool_false for `a`-`f`.
 * @return Digit character `0`-`9` / `A`-`F` / `a`-`f`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_char_t
lh_char_from_xdigit(lh_uchar_t digit, lh_bool_t uppercase)
{
    lh_assert_runtime_if(digit > 15U,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    if (digit < 10U)
    {
        return lh_char_ord_to(lh_char_t, lh_char_ord('0') + digit);
    }
    return lh_char_ord_to(lh_char_t,
                          (uppercase ? lh_char_ord('A') : lh_char_ord('a')) + (digit - 10U));
}

/**
 * @brief Fold @p digit into @p value as the next (least significant)
 *        hexadecimal digit: `*value = *value * ::LH_CHAR_XDIGIT_RADIX + digit`.
 *
 * @param value Accumulator, updated in place on success; left unmodified on
 *              overflow.
 * @param digit Next digit, `[0, 15]`.
 * @return ::lh_bool_true if it fit in ::lh_uint_t, ::lh_bool_false if it
 *         would have overflowed (@p value is then left unmodified).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_xdigit_accumulate(lh_uint_t *value, lh_uchar_t digit)
{
    lh_assert_runtime_ref(value);
    lh_assert_runtime_if(digit > 15U,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    if (*value > (lh_numeric_limit_max(lh_uint_t) - digit) / LH_CHAR_XDIGIT_RADIX)
    {
        return lh_bool_false;
    }
    *value = *value * LH_CHAR_XDIGIT_RADIX + digit;
    return lh_bool_true;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_XDIGIT_H */
