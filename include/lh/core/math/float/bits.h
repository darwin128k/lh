/**
 * @file bits.h
 * @brief The IEEE 754 bits of a ::lh_float_t: ::lh_math_float_to_bits,
 *        ::lh_math_float_from_bits.
 *
 * The transcendentals take a number apart into its exponent and its fraction
 * and put it back together from them, in integers: that is how
 * ::lh_math_float_exp scales by a power of two and how ::lh_math_float_log
 * reads the exponent of the number it is given. The format is binary32 (IEEE
 * 754 single precision) -- the one ::lh_float_t is -- and the constants of it
 * are here, next to the two functions that read and write it.
 *
 * ::lh/core/real/bits.h is the same trick for the optional numeric subsystem
 * (::lh_real_t, binary32 or binary64 by option). This one belongs to the
 * float math, which is always binary32.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FLOAT.
 */

#ifndef LH_MATH_FLOAT_BITS_H
#define LH_MATH_FLOAT_BITS_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>
#include <lh/core/numeric/float.h>

/** @brief Stored fraction bits (the leading 1 is implicit): 23. */
#define LH_MATH_FLOAT_MANTISSA_BITS 23
/** @brief What is subtracted from the stored exponent: 127. */
#define LH_MATH_FLOAT_EXPONENT_BIAS 127
/** @brief The stored exponent of infinity and NaN: `0xFF`. */
#define LH_MATH_FLOAT_EXPONENT_MAX 0xFF
/** @brief Where the sign bit is: 31. */
#define LH_MATH_FLOAT_SIGN_SHIFT 31
/** @brief The bits of a quiet NaN: the exponent all ones, a non-zero fraction. */
#define LH_MATH_FLOAT_NAN_BITS 0x7FC00000U
/** @brief The bits of a positive infinity: the exponent all ones, no fraction. */
#define LH_MATH_FLOAT_INFINITY_BITS 0x7F800000U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The bits of @p value.
 *
 * @param value The number to take apart.
 * @return The 32 bits of the binary32 format, the sign bit first.
 */
lh_u32_t
lh_math_float_to_bits(lh_float_t value);

/**
 * @brief The ::lh_float_t whose bits are @p bits.
 *
 * @param bits The 32 bits of the binary32 format, the sign bit first.
 * @return The number those bits spell.
 */
lh_float_t
lh_math_float_from_bits(lh_u32_t bits);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_BITS_H */
