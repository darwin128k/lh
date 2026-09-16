/**
 * @file fields.h
 * @brief Macro for declaring hour/minute/second member fields.
 */

#ifndef LH_TIME_FIELDS_H
#define LH_TIME_FIELDS_H

/**
 * @def lh_time_fields(hour_type, minute_type, second_type)
 * @brief Expands to `hour`, `minute`, and `second` members.
 *
 * @param hour_type   Type of the `hour` field.
 * @param minute_type Type of the `minute` field.
 * @param second_type Type of the `second` field.
 */
#define lh_time_fields(hour_type, minute_type, second_type)                                        \
    hour_type hour;                                                                                \
    minute_type minute;                                                                            \
    second_type second

#endif /* LH_TIME_FIELDS_H */
