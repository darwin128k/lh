#ifndef LH_NUMERIC_LIMIT_H
#define LH_NUMERIC_LIMIT_H

#include <lh/type_util.h>

#define lh_numeric_limit_smin(T) lh_cast(T, lh_type_sign_of(T))
#define lh_numeric_limit_smax(T) lh_bit_not(lh_numeric_limit_smin(T))
#define lh_numeric_limit_umin(T) lh_type_zero_v(T)
#define lh_numeric_limit_umax(T) lh_type_neg_one_v(T)

#endif // LH_NUMERIC_LIMIT_H
