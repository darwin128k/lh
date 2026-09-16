/**
 * @file fields.h
 * @brief Macro for declaring date+time member fields.
 */

#ifndef LH_DATETIME_FIELDS_H
#define LH_DATETIME_FIELDS_H

/**
 * @def lh_datetime_fields(date_type, time_type)
 * @brief Expands to `date` and `time` members.
 *
 * @param date_type Type of the `date` field.
 * @param time_type Type of the `time` field.
 */
#define lh_datetime_fields(date_type, time_type)                                                   \
    date_type date;                                                                                \
    time_type time

#endif /* LH_DATETIME_FIELDS_H */
