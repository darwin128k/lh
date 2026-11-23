/**
 * @file ln2.h
 * @brief The natural logarithm of two, in ::lh_float_t, in two parts.
 *
 * The transcendentals cut their argument into whole turns of ln(2) and a
 * remainder below half a turn. The high part has 16 bits, so a whole number
 * of turns times it is exact; the low part is what the high part is short of.
 * The remainder then keeps the digits of the argument instead of rounding them
 * away.
 */

#ifndef LH_MATH_FLOAT_LN2_H
#define LH_MATH_FLOAT_LN2_H

#include <lh/core/numeric/float.h>

/** @brief ln(2), the high 16 bits: a whole number of turns times it is exact. */
#define LH_MATH_FLOAT_LN2_HI 0.693145751953125f

/** @brief What the high part is short of ln(2). */
#define LH_MATH_FLOAT_LN2_LO 1.4286068e-6f

/** @brief ln(2) as the float nearest to it. */
#define LH_MATH_FLOAT_LN2 (LH_MATH_FLOAT_LN2_HI + LH_MATH_FLOAT_LN2_LO)

#endif /* LH_MATH_FLOAT_LN2_H */
