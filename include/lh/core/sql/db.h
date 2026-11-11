/**
 * @file db.h
 * @brief A database connection (::lh_sql_db_t), whatever backend it runs on.
 *
 * A backend opens it (::lh_os_sql_sqlite_open, ...): sets its driver and its
 * handle. From then on every operation goes through the driver, and a
 * failure leaves its message in the database for ::lh_sql_db_get_error —
 * owned text, no fixed-size buffer to cut it.
 *
 * Example usage:
 * @code{.c}
 * lh_sql_db_t db;
 *
 * lh_sql_db_init(&db);
 * if (!lh_os_sql_sqlite_open(&db, lh_str_view_lit("bans.db")) ||
 *     !lh_sql_db_exec(&db, lh_str_view_lit("CREATE TABLE IF NOT EXISTS bans(id TEXT)"))) {
 *     // lh_sql_db_get_error(&db)
 * }
 * lh_sql_db_deinit(&db);
 * @endcode
 */

#ifndef LH_SQL_DB_H
#define LH_SQL_DB_H

#include <lh/core/sql/db/fields.h>
#include <lh/core/sql/driver.h>
#include <lh/core/sql/error/kind.h>
#include <lh/core/str.h>

/**
 * @struct lh_sql_db
 * @brief Driver, handle and last error. Fields via ::lh_sql_db_fields.
 */
struct lh_sql_db
{
    /** Members: ::lh_sql_db_fields. */
    lh_sql_db_fields(const lh_sql_driver_t *, lh_ptr, lh_str_t, lh_sql_error_kind_t, lh_sint_t);
};
/** @copybrief lh_sql_db */
typedef struct lh_sql_db lh_sql_db_t;

struct lh_sql_stmt;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The backend @p self is open on; ::lh_null while closed. */
const lh_sql_driver_t *
lh_sql_db_get_driver(const lh_sql_db_t *self);

/** Make @p driver the backend @p self dispatches through. For backends. */
lh_void
lh_sql_db_set_driver(lh_sql_db_t *self, const lh_sql_driver_t *driver);

/** The backend's own connection; ::lh_null while closed. */
lh_ptr
lh_sql_db_get_handle(const lh_sql_db_t *self);

/** Make @p handle the backend's own connection. For backends. */
lh_void
lh_sql_db_set_handle(lh_sql_db_t *self, lh_ptr handle);

/** The text of the last failure, to change. */
lh_str_t *
lh_sql_db_get_error_mut(lh_sql_db_t *self);

/** The message of the last failure; empty after a success. */
lh_str_view_t
lh_sql_db_get_error(const lh_sql_db_t *self);

/** The kind of the last failure; ::lh_sql_error_kind_none after a success. */
lh_sql_error_kind_t
lh_sql_db_get_error_kind(const lh_sql_db_t *self);

/** Store the kind of the last failure (level 1). For backends. */
lh_void
lh_sql_db_set_error_kind(lh_sql_db_t *self, lh_sql_error_kind_t kind);

/**
 * @brief The backend's own number for the last failure (`mysql_errno`, SQLite's
 *        extended result code); 0 when it gave none or after a success.
 */
lh_sint_t
lh_sql_db_get_error_code(const lh_sql_db_t *self);

/** Store the backend's number for the last failure (level 1). For backends. */
lh_void
lh_sql_db_set_error_code(lh_sql_db_t *self, lh_sint_t code);

/**
 * @brief Make @p message (copied), @p kind and @p code the last failure. For
 *        backends.
 */
lh_void
lh_sql_db_set_error_as(lh_sql_db_t *self, lh_sql_error_kind_t kind, lh_sint_t code,
                       lh_str_view_t message);

/** Forget the last failure: no message, ::lh_sql_error_kind_none, code 0. */
lh_void
lh_sql_db_clear_error(lh_sql_db_t *self);

/** True when a backend opened @p self and it is not closed yet. */
lh_bool_t
lh_sql_db_is_open(const lh_sql_db_t *self);

/** Set @p self to closed, without an error. */
lh_void
lh_sql_db_init(lh_sql_db_t *self);

/**
 * @brief Make @p message (copied) the message of the last failure, of kind
 *        ::lh_sql_error_kind_other and without a code. For backends.
 */
lh_void
lh_sql_db_set_error(lh_sql_db_t *self, lh_str_view_t message);

/**
 * @brief ::lh_sql_db_set_error_as and return ::lh_bool_false, so a backend can
 *        `return lh_sql_db_fail_as(...)`.
 */
lh_bool_t
lh_sql_db_fail_as(lh_sql_db_t *self, lh_sql_error_kind_t kind, lh_sint_t code,
                  lh_str_view_t message);

/** Close @p self if open; it stays usable for another open. */
lh_void
lh_sql_db_close(lh_sql_db_t *self);

/** The row id of the last `INSERT` on @p self; `0` while closed. */
lh_sllong_t
lh_sql_db_get_last_insert_id(lh_sql_db_t *self);

/**
 * @brief Set @p message as the last failure (kind ::lh_sql_error_kind_other)
 *        and return ::lh_bool_false, so a backend can `return lh_sql_db_fail(...)`.
 */
lh_bool_t
lh_sql_db_fail(lh_sql_db_t *self, lh_str_view_t message);

/** Close @p self and free its error text. */
lh_void
lh_sql_db_deinit(lh_sql_db_t *self);

/**
 * @brief Run every statement in @p sql (`;`-separated where the backend
 *        allows it), ignoring rows. For schema and one-off writes.
 *
 * @return ::lh_bool_false, with the error set, when @p self is closed or a
 *         statement fails.
 */
lh_bool_t
lh_sql_db_exec(lh_sql_db_t *self, lh_str_view_t sql);

/**
 * @brief Compile the statement @p sql into @p stmt, with `?` for the values
 *        to bind. @p stmt needs no init; finalize it even when this fails.
 */
lh_bool_t
lh_sql_db_prepare(lh_sql_db_t *self, struct lh_sql_stmt *stmt, lh_str_view_t sql);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_DB_H */
