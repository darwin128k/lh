/**
 * @file mul.h
 * @brief The full 128-bit product of two 64-bit numbers.
 *
 * C has no 128-bit integer, and floating point conversion (lh/real) lives
 * on this one operation. GCC and Clang on 64-bit targets do it in one
 * instruction through `unsigned __int128`; everywhere else it is four
 * 32-bit products.
 */

#ifndef LH_NUMERIC_MUL_H
#define LH_NUMERIC_MUL_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief ::lh_numeric_mul_u64_full from four 32-bit products, on any target.
 *
 * @param high Receives the upper 64 bits.
 * @return The lower 64 bits.
 */
lh_u64_t
lh_numeric_mul_u64_full_halves(lh_u64_t a, lh_u64_t b, lh_u64_t *high);

/**
 * @brief @p a times @p b, all 128 bits of it: one instruction where the
 *        compiler has a 128-bit type, ::lh_numeric_mul_u64_full_halves
 *        elsewhere.
 *
 * @param high Receives the upper 64 bits.
 * @return The lower 64 bits.
 */
lh_u64_t
lh_numeric_mul_u64_full(lh_u64_t a, lh_u64_t b, lh_u64_t *high);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NUMERIC_MUL_H */
