#ifndef LH_MATH_UTIL_H
#define LH_MATH_UTIL_H

#define lh_math_add(a, b) ((a) + (b))
#define lh_math_sub(a, b) ((a) - (b))
#define lh_math_mul(a, b) ((a) * (b))
#define lh_math_div(a, b) ((a) / (b))
#define lh_math_mod(a, b) ((a) % (b))

#define lh_math_add_one(v) lh_math_add(v, 1)
#define lh_math_sub_one(v) lh_math_sub(v, 1)

#define lh_math_bit_and(a, b) ((a) & (b))
#define lh_math_bit_or(a, b) ((a) | (b))
#define lh_math_bit_xor(a, b) ((a) ^ (b))
#define lh_math_bit_not(v) (~(v))
#define lh_math_bit_shl(a, n) ((a) << (n))
#define lh_math_bit_shr(a, n) ((a) >> (n))

#endif // LH_MATH_UTIL_H
