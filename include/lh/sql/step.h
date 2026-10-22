/**
 * @file step.h
 * @brief What one ::lh_sql_stmt_step produced (::lh_sql_step_t).
 */

#ifndef LH_SQL_STEP_H
#define LH_SQL_STEP_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_sql_step_t
 * @brief Outcome of a step. Alias for ::lh_u8_t; values are the
 *        `lh_sql_step_*` constants below.
 */
typedef lh_u8_t lh_sql_step_t;

/**
 * @def lh_sql_step_error
 * @brief The statement failed; ::lh_sql_db_get_error says why.
 */
#define lh_sql_step_error 0U

/**
 * @def lh_sql_step_row
 * @brief A row is ready: read it with `lh_sql_stmt_get_column_*`, step again
 *        for the next.
 */
#define lh_sql_step_row 1U

/**
 * @def lh_sql_step_done
 * @brief The statement ran to the end: no (more) rows.
 */
#define lh_sql_step_done 2U

#endif /* LH_SQL_STEP_H */
