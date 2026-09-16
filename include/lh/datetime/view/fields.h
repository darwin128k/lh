/**
 * @file fields.h
 * @brief Macro for declaring date+time member fields.
 */

#ifndef LH_DATETIME_FIELDS_H
#define LH_DATETIME_FIELDS_H

/**
 * @def lh_datetime_fields(Date, Time)
 * @brief Expands to `date` and `time` members.
 */
#define lh_datetime_fields(Date, Time)                                                             \
    Date date;                                                                                     \
    Time time

#endif /* LH_DATETIME_FIELDS_H */
