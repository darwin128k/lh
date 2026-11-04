/**
 * @file fields.h
 * @brief Macro for declaring year/month/day member fields.
 */

#ifndef LH_DATE_FIELDS_H
#define LH_DATE_FIELDS_H

/**
 * @def lh_date_fields(year_type, month_type, day_type)
 * @brief Expands to `year`, `month`, and `day` members.
 *
 * @param year_type  Type of the `year` field.
 * @param month_type Type of the `month` field.
 * @param day_type   Type of the `day` field.
 *
 * Example usage:
 * @code{.c}
 * struct lh_date {
 *     lh_date_fields(lh_date_year_t, lh_date_month_t, lh_date_day_t);
 * };
 * @endcode
 */
#define lh_date_fields(year_type, month_type, day_type)                                            \
    year_type year;                                                                                \
    month_type month;                                                                              \
    day_type day

#endif /* LH_DATE_FIELDS_H */
