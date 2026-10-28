/**
 * @file db.h
 * @brief A database connection (::lh_sql_db_t), whatever backend it runs on.
 *
 * A backend opens it (::lh_sql_db_open_sqlite, ...): sets its driver and its
 * handle. From then on every operation goes through the driver, and a
 * failure leaves its message in the database for ::lh_sql_db_get_error —
 * owned text, no fixed-size buffer to cut it.
 *
 * Example usage:
 * @code{.c}
 * lh_sql_db_t db;
 *
 * lh_sql_db_init(&db);
 * if (!lh_sql_db_open_sqlite(&db, lh_str_view_lit("bans.db")) ||
 *     !lh_sql_db_exec(&db, lh_str_view_lit("CREATE TABLE IF NOT EXISTS bans(id TEXT)"))) {
 *     // lh_sql_db_get_error(&db)
 * }
 * lh_sql_db_deinit(&db);
 * @endcode
 */

#ifndef LH_SQL_DB_H
#define LH_SQL_DB_H

#include <lh/sql/db/fields.h>
#include <lh/sql/driver.h>
#include <lh/str.h>

/**
 * @struct lh_sql_db
 * @brief Driver, handle and last error. Fields via ::lh_sql_db_fields.
 */
struct lh_sql_db
{
    /** Members: ::lh_sql_db_fields. */
    lh_sql_db_fields(const lh_sql_driver_t *, lh_ptr, lh_str_t);
};
/** @copybrief lh_sql_db */
typedef struct lh_sql_db lh_sql_db_t;

struct lh_sql_stmt;

LH_COMPILER_EXTERN_C_BEGIN

/* ── accessors ───────────────────────────────────────────────────────────── */

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

/** Make @p message (copied) the message of the last failure. For backends. */
lh_void
lh_sql_db_set_error(lh_sql_db_t *self, lh_str_view_t message);

/** Forget the last failure. */
lh_void
lh_sql_db_clear_error(lh_sql_db_t *self);

/**
 * @brief Set @p message as the last failure and return ::lh_bool_false, so
 *        a backend can `return lh_sql_db_fail(...)`.
 */
lh_bool_t
lh_sql_db_fail(lh_sql_db_t *self, lh_str_view_t message);

/** True when a backend opened @p self and it is not closed yet. */
lh_bool_t
lh_sql_db_is_open(const lh_sql_db_t *self);

/* ── init / deinit ───────────────────────────────────────────────────────── */

/** Set @p self to closed, without an error. */
lh_void
lh_sql_db_init(lh_sql_db_t *self);

/** Close @p self if open; it stays usable for another open. */
lh_void
lh_sql_db_close(lh_sql_db_t *self);

/** Close @p self and free its error text. */
lh_void
lh_sql_db_deinit(lh_sql_db_t *self);

/* ── operations ──────────────────────────────────────────────────────────── */

/**
 * @brief Run every statement in @p sql (`;`-separated where the backend
 *        allows it), ignoring rows. For schema and one-off writes.
 *
 * @return ::lh_bool_false, with the error set, when @p self is closed or a
 *         statement fails.
 */
lh_bool_t
lh_sql_db_exec(lh_sql_db_t *self, lh_str_view_t sql);

/** The row id of the last `INSERT` on @p self; `0` while closed. */
lh_sllong_t
lh_sql_db_get_last_insert_id(lh_sql_db_t *self);

/**
 * @brief Compile the statement @p sql into @p stmt, with `?` for the values
 *        to bind. @p stmt needs no init; finalize it even when this fails.
 */
lh_bool_t
lh_sql_db_prepare(lh_sql_db_t *self, struct lh_sql_stmt *stmt, lh_str_view_t sql);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_DB_H */
