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
 */

#ifndef LH_CHAR_DIGIT_H
#define LH_CHAR_DIGIT_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>

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
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_char_is_digit(lh_char_t ch);

/**
 * @brief Convert an ASCII decimal digit to its numeric value.
 *
 * @param ch Digit character; must satisfy ::lh_char_is_digit.
 * @return Value in `[0, 9]`.
 */
LH_ATTRIBUTE_SYMBOL
lh_uchar_t
lh_char_to_digit(lh_char_t ch);

/**
 * @brief Convert a numeric value to its ASCII decimal digit.
 *
 * @param digit Value in `[0, 9]`.
 * @return Digit character `0`-`9`.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_char_from_digit(lh_uchar_t digit);

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
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_char_digit_accumulate(lh_uint_t *value, lh_uchar_t digit);

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
LH_ATTRIBUTE_SYMBOL
lh_uchar_t
lh_char_digit_extract(lh_uint_t *value, lh_uint_t radix);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_DIGIT_H */
