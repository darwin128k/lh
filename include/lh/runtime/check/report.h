/**
 * @file report.h
 * @brief How much a failed runtime check tells its handler
 *        (::LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT values).
 *
 * Every level up costs flash for the strings each check site keeps, which
 * matters on small bare-metal targets.
 */

#ifndef LH_RUNTIME_CHECK_REPORT_H
#define LH_RUNTIME_CHECK_REPORT_H

/**
 * @def LH_RUNTIME_CHECK_REPORT_NONE
 * @brief The handler gets ::lh_null — no strings stored at all.
 */
#define LH_RUNTIME_CHECK_REPORT_NONE 0

/**
 * @def LH_RUNTIME_CHECK_REPORT_LOCATION
 * @brief The handler gets the file and line of the failed check.
 */
#define LH_RUNTIME_CHECK_REPORT_LOCATION 1

/**
 * @def LH_RUNTIME_CHECK_REPORT_FULL
 * @brief File and line, plus the enclosing function and the text of the
 *        failing condition.
 */
#define LH_RUNTIME_CHECK_REPORT_FULL 2

#endif /* LH_RUNTIME_CHECK_REPORT_H */
