/**
 * @file fields.h
 * @brief Member fields of ::lh_real_diy_t.
 */

#ifndef LH_REAL_DIY_FIELDS_H
#define LH_REAL_DIY_FIELDS_H

/**
 * @def lh_real_diy_fields(f_type, e_type)
 * @brief The number `f × 2^e`.
 *
 * @param f_type Type of `f`, the 64-bit significand (::lh_u64_t).
 * @param e_type Type of `e`, the binary exponent (::lh_s32_t).
 */
#define lh_real_diy_fields(f_type, e_type)                                                         \
    f_type f;                                                                                      \
    e_type e

#endif /* LH_REAL_DIY_FIELDS_H */
