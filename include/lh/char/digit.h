/**
 * @file digit.h
 * @brief Single ASCII decimal digit <-> numeric value (0-9) conversion, plus
 *        the accumulate/extract steps that build a number's text.
 *
 * ::lh_char_is_digit / ::lh_char_to_digit / ::lh_char_from_digit and
 * ::LH_CHAR_DIGIT_RADIX are decimal-only. ::lh_char_digit_accumulate (build
 * up, most-significant digit first — ::lh_str_ptr_parse_uint,
 * ::lh_str_ptr_format_text width/precision) is decimal-only for the same
 * reason: nothing in this library currently accumulates in another base.
 * ::lh_char_digit_extract (pull out, least-significant digit first —
 * ::lh_str_ptr_format_uint, ::lh_str_ptr_format_hex) takes its radix as a
 * parameter instead, because it already has two real callers needing
 * different bases.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE — small and called
 * from other hot, force-inlined callers (::lh_str_ptr_parse_uint,
 * ::lh_net_ip4_parse's digit loop by way of it); measured to cost real,
 * compounding overhead as ordinary exported calls. See the note in
 * `net/ip.c` for the numbers.
 */

#ifndef LH_CHAR_DIGIT_H
#define LH_CHAR_DIGIT_H

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
 * @def LH_CHAR_DIGIT_RADIX
 * @brief Base of the decimal digits ::lh_char_is_digit/::lh_char_to_digit/
 *        ::lh_char_from_digit work with (10).
 *
 * The one named spot for the `10` that would otherwise show up as a bare
 * literal everywhere a decimal digit is accumulated or extracted (multiply
 * to shift a digit in, divide/modulo to pull one out).
 */
#define LH_CHAR_DIGIT_RADIX 10U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p ch is an ASCII decimal digit (`0`-`9`).
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is `0`-`9`, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_digit(lh_char_t ch)
{
    return (ch >= '0' && ch <= '9') ? lh_bool_true : lh_bool_false;
}

/**
 * @brief Convert an ASCII decimal digit to its numeric value.
 *
 * @param ch Digit character; must satisfy ::lh_char_is_digit.
 * @return Value in `[0, 9]`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_uchar_t
lh_char_to_digit(lh_char_t ch)
{
    lh_assert_runtime_if(!lh_char_is_digit(ch),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    return lh_cast_static(lh_uchar_t, (lh_char_ord(ch) - lh_char_ord('0')));
}

/**
 * @brief Convert a numeric value to its ASCII decimal digit.
 *
 * @param digit Value in `[0, 9]`.
 * @return Digit character `0`-`9`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_char_t
lh_char_from_digit(lh_uchar_t digit)
{
    lh_assert_runtime_if(digit > 9U,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    return lh_char_ord_to(lh_char_t, lh_char_ord('0') + digit);
}

/**
 * @brief Fold @p digit into @p value as the next (least significant)
 *        decimal digit: `*value = *value * ::LH_CHAR_DIGIT_RADIX + digit`.
 *
 * The overflow-checked accumulation step shared by anything that reads a
 * run of decimal digits into an ::lh_uint_t (::lh_str_ptr_parse_uint and
 * ::lh_str_ptr_format_text's width/precision parsing).
 *
 * @param value Accumulator, updated in place on success; left unmodified on
 *              overflow.
 * @param digit Next digit, `[0, 9]`.
 * @return ::lh_bool_true if it fit in ::lh_uint_t, ::lh_bool_false if it
 *         would have overflowed (@p value is then left unmodified).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_digit_accumulate(lh_uint_t *value, lh_uchar_t digit)
{
    lh_assert_runtime_ref(value);
    lh_assert_runtime_if(digit > 9U,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    if (*value > (lh_numeric_limit_max(lh_uint_t) - digit) / LH_CHAR_DIGIT_RADIX)
    {
        return lh_bool_false; /* would overflow lh_uint_t */
    }
    *value = *value * LH_CHAR_DIGIT_RADIX + digit;
    return lh_bool_true;
}

/**
 * @brief Pull the next (least significant) digit out of @p value in base
 *        @p radix — the inverse of ::lh_char_digit_accumulate, for anything
 *        that builds a number's text right-to-left (::lh_str_ptr_format_uint
 *        with ::LH_CHAR_DIGIT_RADIX, ::lh_str_ptr_format_hex with its own
 *        base-16 radix).
 *
 * Unlike ::lh_char_digit_accumulate this takes @p radix as a parameter
 * rather than hard-coding ::LH_CHAR_DIGIT_RADIX — the two current callers
 * genuinely need different bases, so there is no one decimal-only radix to
 * bake in here.
 *
 * @param value Accumulator; divided by @p radix in place.
 * @param radix Base to extract in (e.g. ::LH_CHAR_DIGIT_RADIX for decimal).
 * @return The extracted digit, `[0, radix)`. Callers map it to a character
 *         themselves (::lh_char_from_digit for decimal; a lookup table for
 *         radixes with letter digits, like hex).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_uchar_t
lh_char_digit_extract(lh_uint_t *value, lh_uint_t radix)
{
    lh_uchar_t digit;

    lh_assert_runtime_ref(value);

    digit = lh_cast_static(lh_uchar_t, (*value % radix));
    *value /= radix;
    return digit;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_DIGIT_H */
