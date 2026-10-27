/**
 * @file stmt.h
 * @brief A prepared statement (::lh_sql_stmt_t): bind values, step through
 *        rows, read columns.
 *
 * Made by ::lh_sql_db_prepare, run on the backend of its database. Values
 * are bound to `?` parameters, counted from 1; columns are read from 0.
 * Text read from a column is valid until the next step, reset or finalize.
 *
 * Example usage:
 * @code{.c}
 * lh_sql_stmt_t stmt;
 *
 * if (lh_sql_db_prepare(&db, &stmt, lh_str_view_lit("SELECT name FROM bans WHERE id = ?"))) {
 *     lh_sql_stmt_bind_str(&stmt, 1, steam_id);
 *     while (lh_sql_stmt_step(&stmt) == lh_sql_step_row) {
 *         lh_str_view_t name = lh_sql_stmt_get_column_str(&stmt, 0);
 *     }
 * }
 * lh_sql_stmt_finalize(&stmt);
 * @endcode
 */

#ifndef LH_SQL_STMT_H
#define LH_SQL_STMT_H

#include <lh/sql/db.h>
#include <lh/sql/step.h>
#include <lh/sql/stmt/fields.h>

/**
 * @struct lh_sql_stmt
 * @brief Database plus backend statement. Fields via ::lh_sql_stmt_fields.
 */
struct lh_sql_stmt
{
    /** Members: ::lh_sql_stmt_fields. */
    lh_sql_stmt_fields(lh_sql_db_t *, lh_ptr);
};
/** @copybrief lh_sql_stmt */
typedef struct lh_sql_stmt lh_sql_stmt_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── accessors ───────────────────────────────────────────────────────────── */

/** The database @p self runs on, to change. */
lh_sql_db_t *
lh_sql_stmt_get_db_mut(lh_sql_stmt_t *self);

/** The database @p self runs on. */
const lh_sql_db_t *
lh_sql_stmt_get_db(const lh_sql_stmt_t *self);

/** Make @p db the database @p self runs on. */
lh_void
lh_sql_stmt_set_db(lh_sql_stmt_t *self, lh_sql_db_t *db);

/** The backend's own statement; ::lh_null when not prepared. */
lh_ptr
lh_sql_stmt_get_handle(const lh_sql_stmt_t *self);

/** Make @p handle the backend's own statement. For backends. */
lh_void
lh_sql_stmt_set_handle(lh_sql_stmt_t *self, lh_ptr handle);

/** The driver of the database @p self runs on. */
const lh_sql_driver_t *
lh_sql_stmt_get_driver(const lh_sql_stmt_t *self);

/** True when @p self holds a prepared statement. */
lh_bool_t
lh_sql_stmt_is_prepared(const lh_sql_stmt_t *self);

/* ── init / finalize ─────────────────────────────────────────────────────── */

/** Set @p self to run on @p db, not prepared yet. */
lh_void
lh_sql_stmt_init(lh_sql_stmt_t *self, lh_sql_db_t *db);

/** Free the statement in the backend; @p self is not prepared afterwards. */
lh_void
lh_sql_stmt_finalize(lh_sql_stmt_t *self);

/* ── bind ────────────────────────────────────────────────────────────────── */

/** Bind `NULL` to parameter @p index (from 1). */
lh_bool_t
lh_sql_stmt_bind_null(lh_sql_stmt_t *self, lh_usize_t index);

/** Bind the integer @p value to parameter @p index (from 1). */
lh_bool_t
lh_sql_stmt_bind_sint(lh_sql_stmt_t *self, lh_usize_t index, lh_sllong_t value);

/** Bind a copy of the text @p value to parameter @p index (from 1). */
lh_bool_t
lh_sql_stmt_bind_str(lh_sql_stmt_t *self, lh_usize_t index, lh_str_view_t value);

/* ── run ─────────────────────────────────────────────────────────────────── */

/**
 * @brief Run @p self to its next row (::lh_sql_step_row) or its end
 *        (::lh_sql_step_done); ::lh_sql_step_error, with the database's
 *        error set, on a failure or when @p self is not prepared.
 */
lh_sql_step_t
lh_sql_stmt_step(lh_sql_stmt_t *self);

/** Rewind @p self to run again; the bound values stay. */
lh_bool_t
lh_sql_stmt_reset(lh_sql_stmt_t *self);

/* ── read the current row ────────────────────────────────────────────────── */

/** How many columns a row of @p self has. */
lh_usize_t
lh_sql_stmt_get_column_count(const lh_sql_stmt_t *self);

/** True when column @p index (from 0) of the current row is `NULL`. */
lh_bool_t
lh_sql_stmt_is_column_null(const lh_sql_stmt_t *self, lh_usize_t index);

/** Column @p index (from 0) of the current row as an integer; `0` for `NULL`. */
lh_sllong_t
lh_sql_stmt_get_column_sint(const lh_sql_stmt_t *self, lh_usize_t index);

/**
 * @brief Column @p index (from 0) of the current row as text; empty for
 *        `NULL`. Valid until the next step, reset or finalize.
 */
lh_str_view_t
lh_sql_stmt_get_column_str(const lh_sql_stmt_t *self, lh_usize_t index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_STMT_H */
