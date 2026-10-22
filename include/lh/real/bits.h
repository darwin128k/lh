/**
 * @file bits.h
 * @brief The IEEE 754 bits of an ::lh_real_t: sign, biased exponent and
 *        fraction, and the constants of its format.
 *
 * Text to number (lh/str/parse/real.h) and number to text
 * (lh/str/format/real.h) never compute in floating point: they take the
 * number apart into these bits and put it together from them, in integers.
 * So they give the same digits on every target and pull no soft-float code
 * into an image without an FPU. The format follows ::lh_real_t: binary64
 * for ::LH_REAL_DOUBLE, binary32 for ::LH_REAL_FLOAT; nothing here exists
 * for ::LH_REAL_NONE.
 */

#ifndef LH_REAL_BITS_H
#define LH_REAL_BITS_H

#include <lh/real.h>

#if LH_REAL_ENABLED

#    include <lh/bool.h>
#    include <lh/compiler/extern/c.h>
#    include <lh/numeric/fixed/types.h>

#    if LH_LIBRARY_OPTION_REAL == LH_REAL_DOUBLE
/** @brief Stored fraction bits (the leading 1 is implicit): 52 or 23. */
#        define LH_REAL_MANTISSA_BITS 52
/** @brief What is subtracted from the stored exponent: 1023 or 127. */
#        define LH_REAL_EXPONENT_BIAS 1023
/** @brief The stored exponent of infinity and NaN: `0x7FF` or `0xFF`. */
#        define LH_REAL_EXPONENT_MAX 0x7FF
/** @brief Where the sign bit is: 63 or 31. */
#        define LH_REAL_SIGN_SHIFT 63
#    else
#        define LH_REAL_MANTISSA_BITS 23
#        define LH_REAL_EXPONENT_BIAS 127
#        define LH_REAL_EXPONENT_MAX 0xFF
#        define LH_REAL_SIGN_SHIFT 31
#    endif

LH_COMPILER_EXTERN_C_BEGIN

/** The bits of @p value, in the low bits of the result. */
lh_u64_t
lh_real_to_bits(lh_real_t value);

/** The ::lh_real_t whose bits are @p bits. */
lh_real_t
lh_real_from_bits(lh_u64_t bits);

/** True when the sign bit of @p bits is set (`-0` included). */
lh_bool_t
lh_real_bits_is_negative(lh_u64_t bits);

/** The stored (biased) exponent of @p bits; `0` for zero and subnormals. */
lh_u32_t
lh_real_bits_get_exponent(lh_u64_t bits);

/** The stored fraction of @p bits, without the implicit leading 1. */
lh_u64_t
lh_real_bits_get_fraction(lh_u64_t bits);

/** @p bits without the sign bit: the bits of the absolute value; `0` for both zeros. */
lh_u64_t
lh_real_bits_get_magnitude(lh_u64_t bits);

/** True when @p bits is neither an infinity nor a NaN. */
lh_bool_t
lh_real_bits_is_finite(lh_u64_t bits);

/** The bits of the number with this sign, stored exponent and fraction. */
lh_u64_t
lh_real_bits_make(lh_bool_t negative, lh_u32_t exponent, lh_u64_t fraction);

LH_COMPILER_EXTERN_C_END

#endif /* LH_REAL_ENABLED */

#endif /* LH_REAL_BITS_H */
