/**
 * @file kind.h
 * @brief What kind of failure a database reported (::lh_sql_error_kind_t).
 *
 * The message of a failure (::lh_sql_db_get_error) is for people; the kind
 * is for code: whether to try again later (the server could not be reached),
 * to stop and fix the configuration (access denied, no client library, a
 * backend this build does not have) or to fix the statement. The backends set
 * it together with the message and the native code
 * (::lh_sql_db_get_error_code).
 */

#ifndef LH_SQL_ERROR_KIND_H
#define LH_SQL_ERROR_KIND_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_sql_error_kind_t
 * @brief The kind of a database failure; the `lh_sql_error_kind_*` constants.
 */
typedef lh_u8_t lh_sql_error_kind_t;

/** @def lh_sql_error_kind_none @brief No failure. */
#define lh_sql_error_kind_none 0U

/**
 * @def lh_sql_error_kind_usage
 * @brief The caller's mistake, found before the database was asked: not open,
 *        an empty path, no statement in the text, a parameter not bound.
 */
#define lh_sql_error_kind_usage 1U

/**
 * @def lh_sql_error_kind_unsupported
 * @brief The address names a backend this build does not have.
 */
#define lh_sql_error_kind_unsupported 2U

/**
 * @def lh_sql_error_kind_no_client
 * @brief The backend's client is missing or could not start (no MySQL client
 *        library, `mysql_init` failed, out of memory).
 */
#define lh_sql_error_kind_no_client 3U

/**
 * @def lh_sql_error_kind_unreachable
 * @brief The server could not be reached, or the connection was lost: trying
 *        again later may work.
 */
#define lh_sql_error_kind_unreachable 4U

/**
 * @def lh_sql_error_kind_denied
 * @brief The database refused the user: a wrong password, no rights, a
 *        read-only file.
 */
#define lh_sql_error_kind_denied 5U

/**
 * @def lh_sql_error_kind_not_found
 * @brief The database itself is not there: an unknown database on the
 *        server, a file that cannot be opened.
 */
#define lh_sql_error_kind_not_found 6U

/**
 * @def lh_sql_error_kind_statement
 * @brief The database rejected a statement: its syntax, a missing table, a
 *        broken constraint.
 */
#define lh_sql_error_kind_statement 7U

/** @def lh_sql_error_kind_other @brief Anything else (busy, locked, a client error). */
#define lh_sql_error_kind_other 8U

#endif /* LH_SQL_ERROR_KIND_H */
