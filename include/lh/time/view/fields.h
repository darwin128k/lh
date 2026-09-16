/**
 * @file fields.h
 * @brief Macro for declaring hour/minute/second member fields.
 */

#ifndef LH_TIME_FIELDS_H
#define LH_TIME_FIELDS_H

/**
 * @def lh_time_fields(Hour, Minute, Second)
 * @brief Expands to `hour`, `minute`, and `second` members.
 */
#define lh_time_fields(Hour, Minute, Second)                                                       \
    Hour hour;                                                                                     \
    Minute minute;                                                                                 \
    Second second

#endif /* LH_TIME_FIELDS_H */
