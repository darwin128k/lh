/**
 * @file fields.h
 * @brief Macro for declaring year/month/day member fields.
 */

#ifndef LH_DATE_FIELDS_H
#define LH_DATE_FIELDS_H

/**
 * @def lh_date_fields(Year, Month, Day)
 * @brief Expands to `year`, `month`, and `day` members.
 */
#define lh_date_fields(Year, Month, Day)                                                           \
    Year year;                                                                                     \
    Month month;                                                                                   \
    Day day

#endif /* LH_DATE_FIELDS_H */
