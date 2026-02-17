#ifndef LH_TYPE_UTIL_H
#define LH_TYPE_UTIL_H

#include <lh/bit_util.h>
#include <lh/util.h>

#define lh_type_size(T) lh_size_of(T)
#define lh_type_bits(T) lh_bits_of(T)
#define lh_type_zero_v(T) lh_cast(T, 0)
#define lh_type_one_v(T) lh_cast(T, 1)
#define lh_type_neg_one_v(T) lh_cast(T, -1)
#define lh_type_is_signed(T) (lh_type_zero_v(T) > lh_type_neg_one_v(T))
#define lh_type_is_unsigned(T) (!lh_type_is_signed(T))
#define lh_type_sign_pos_of(v) lh_math_sub_one(lh_type_bits(v))
#define lh_type_sign_of(v) lh_bit_shl(lh_type_one_v(v), lh_type_sign_pos_of(v))

#endif // LH_TYPE_UTIL_H
