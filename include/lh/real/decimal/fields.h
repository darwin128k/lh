/**
 * @file fields.h
 * @brief Member fields of ::lh_real_decimal_t.
 */

#ifndef LH_REAL_DECIMAL_FIELDS_H
#define LH_REAL_DECIMAL_FIELDS_H

/**
 * @def lh_real_decimal_fields(mantissa_type, exponent_type, flag_type)
 * @brief A decimal number as `±mantissa × 10^exponent`.
 *
 * `mantissa` holds the first significant digits, `exponent` the power of
 * ten they are scaled by, `negative` the sign, `truncated` whether nonzero
 * digits past what `mantissa` holds were dropped.
 *
 * @param mantissa_type Type of `mantissa` (::lh_u64_t).
 * @param exponent_type Type of `exponent` (::lh_s64_t).
 * @param flag_type     Type of `negative` and `truncated` (::lh_bool_t).
 */
#define lh_real_decimal_fields(mantissa_type, exponent_type, flag_type)                            \
    mantissa_type mantissa;                                                                        \
    exponent_type exponent;                                                                        \
    flag_type negative;                                                                            \
    flag_type truncated

#endif /* LH_REAL_DECIMAL_FIELDS_H */
