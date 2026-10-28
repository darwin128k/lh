/**
 * @file driver.h
 * @brief One database backend: ::lh_sql_driver_t, a function table with no
 *        state.
 *
 * SQLite (lh/sql/sqlite.h) and MySQL each ship one of these; an
 * ::lh_sql_db_t opened by a backend points at its table and dispatches every
 * operation through it, so the rest of lh/sql never names a backend. A host
 * can bring its own (a test double, PostgreSQL) the same way.
 *
 * Indices follow SQL: bind parameters count from 1 (`?1` is the first),
 * columns from 0. Errors are reported by a `false` / ::lh_sql_step_error and
 * a message left in the database (::lh_sql_db_set_error).
 */

#ifndef LH_SQL_DRIVER_H
#define LH_SQL_DRIVER_H

#include <lh/sql/driver/fields.h>
#include <lh/sql/step.h>
#include <lh/str/view.h>

struct lh_sql_db;
struct lh_sql_stmt;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Close the backend handle of @p db and forget it. */
typedef lh_void(lh_sql_close_fn)(struct lh_sql_db *db);

/** @brief Run every statement of @p sql, discarding any rows. */
typedef lh_bool_t(lh_sql_exec_fn)(struct lh_sql_db *db, lh_str_view_t sql);

/** @brief The row id the last `INSERT` on @p db produced. */
typedef lh_sllong_t(lh_sql_last_insert_id_fn)(struct lh_sql_db *db);

/** @brief Compile the first statement of @p sql into @p stmt. */
typedef lh_bool_t(lh_sql_prepare_fn)(struct lh_sql_db *db, struct lh_sql_stmt *stmt,
                                     lh_str_view_t sql);

/** @brief Free what @p stmt holds in the backend. */
typedef lh_void(lh_sql_finalize_fn)(struct lh_sql_stmt *stmt);

/** @brief Bind `NULL` to parameter @p index. */
typedef lh_bool_t(lh_sql_bind_null_fn)(struct lh_sql_stmt *stmt, lh_usize_t index);

/** @brief Bind the integer @p value to parameter @p index. */
typedef lh_bool_t(lh_sql_bind_sint_fn)(struct lh_sql_stmt *stmt, lh_usize_t index,
                                       lh_sllong_t value);

/** @brief Bind a copy of the text @p value to parameter @p index. */
typedef lh_bool_t(lh_sql_bind_str_fn)(struct lh_sql_stmt *stmt, lh_usize_t index,
                                      lh_str_view_t value);

/** @brief Run @p stmt to its next row or its end. */
typedef lh_sql_step_t(lh_sql_step_fn)(struct lh_sql_stmt *stmt);

/** @brief Rewind @p stmt to run again, keeping its bindings. */
typedef lh_bool_t(lh_sql_reset_fn)(struct lh_sql_stmt *stmt);

/** @brief How many columns a row of @p stmt has. */
typedef lh_usize_t(lh_sql_column_count_fn)(const struct lh_sql_stmt *stmt);

/** @brief True when column @p index of the current row is `NULL`. */
typedef lh_bool_t(lh_sql_column_is_null_fn)(const struct lh_sql_stmt *stmt, lh_usize_t index);

/** @brief Column @p index of the current row as an integer (`0` for `NULL`). */
typedef lh_sllong_t(lh_sql_column_sint_fn)(const struct lh_sql_stmt *stmt, lh_usize_t index);

/**
 * @brief Column @p index of the current row as text (empty for `NULL`),
 *        valid until the next step, reset or finalize.
 */
typedef lh_str_view_t(lh_sql_column_str_fn)(const struct lh_sql_stmt *stmt, lh_usize_t index);

LH_COMPILER_EXTERN_C_END

/**
 * @struct lh_sql_driver
 * @brief Function table for one backend. Usually a static const; not owned
 *        by the databases using it. Fields via ::lh_sql_driver_fields.
 */
struct lh_sql_driver
{
    /** Members: ::lh_sql_driver_fields. */
    lh_sql_driver_fields(lh_str_cptr, lh_sql_close_fn, lh_sql_exec_fn, lh_sql_last_insert_id_fn,
                         lh_sql_prepare_fn, lh_sql_finalize_fn, lh_sql_bind_null_fn,
                         lh_sql_bind_sint_fn, lh_sql_bind_str_fn, lh_sql_step_fn, lh_sql_reset_fn,
                         lh_sql_column_count_fn, lh_sql_column_is_null_fn, lh_sql_column_sint_fn,
                         lh_sql_column_str_fn);
};
/** @copybrief lh_sql_driver */
typedef struct lh_sql_driver lh_sql_driver_t;

/* ── dispatch: the one place each slot is read ───────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The name of the backend @p self is (`"sqlite"`, `"mysql"`). */
lh_str_cptr
lh_sql_driver_get_name(const lh_sql_driver_t *self);

/**
 * @brief The backend function behind ::lh_sql_driver_close: the driver's
 *        `close` entry (level 1).
 */
lh_sql_close_fn *
lh_sql_driver_get_close_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `close` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_close_fn(lh_sql_driver_t *self, lh_sql_close_fn * close);

/**
 * @brief The backend function behind ::lh_sql_driver_exec: the driver's
 *        `exec` entry (level 1).
 */
lh_sql_exec_fn *
lh_sql_driver_get_exec_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `exec` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_exec_fn(lh_sql_driver_t *self, lh_sql_exec_fn * exec);

/**
 * @brief The backend function behind ::lh_sql_driver_last_insert_id: the driver's
 *        `last_insert_id` entry (level 1).
 */
lh_sql_last_insert_id_fn *
lh_sql_driver_get_last_insert_id_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `last_insert_id` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_last_insert_id_fn(lh_sql_driver_t *self, lh_sql_last_insert_id_fn * last_insert_id);

/**
 * @brief The backend function behind ::lh_sql_driver_prepare: the driver's
 *        `prepare` entry (level 1).
 */
lh_sql_prepare_fn *
lh_sql_driver_get_prepare_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `prepare` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_prepare_fn(lh_sql_driver_t *self, lh_sql_prepare_fn * prepare);

/**
 * @brief The backend function behind ::lh_sql_driver_finalize: the driver's
 *        `finalize` entry (level 1).
 */
lh_sql_finalize_fn *
lh_sql_driver_get_finalize_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `finalize` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_finalize_fn(lh_sql_driver_t *self, lh_sql_finalize_fn * finalize);

/**
 * @brief The backend function behind ::lh_sql_driver_bind_null: the driver's
 *        `bind_null` entry (level 1).
 */
lh_sql_bind_null_fn *
lh_sql_driver_get_bind_null_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `bind_null` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_bind_null_fn(lh_sql_driver_t *self, lh_sql_bind_null_fn * bind_null);

/**
 * @brief The backend function behind ::lh_sql_driver_bind_sint: the driver's
 *        `bind_sint` entry (level 1).
 */
lh_sql_bind_sint_fn *
lh_sql_driver_get_bind_sint_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `bind_sint` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_bind_sint_fn(lh_sql_driver_t *self, lh_sql_bind_sint_fn * bind_sint);

/**
 * @brief The backend function behind ::lh_sql_driver_bind_str: the driver's
 *        `bind_str` entry (level 1).
 */
lh_sql_bind_str_fn *
lh_sql_driver_get_bind_str_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `bind_str` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_bind_str_fn(lh_sql_driver_t *self, lh_sql_bind_str_fn * bind_str);

/**
 * @brief The backend function behind ::lh_sql_driver_step: the driver's
 *        `step` entry (level 1).
 */
lh_sql_step_fn *
lh_sql_driver_get_step_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `step` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_step_fn(lh_sql_driver_t *self, lh_sql_step_fn * step);

/**
 * @brief The backend function behind ::lh_sql_driver_reset: the driver's
 *        `reset` entry (level 1).
 */
lh_sql_reset_fn *
lh_sql_driver_get_reset_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `reset` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_reset_fn(lh_sql_driver_t *self, lh_sql_reset_fn * reset);

/**
 * @brief The backend function behind ::lh_sql_driver_column_count: the driver's
 *        `column_count` entry (level 1).
 */
lh_sql_column_count_fn *
lh_sql_driver_get_column_count_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `column_count` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_column_count_fn(lh_sql_driver_t *self, lh_sql_column_count_fn * column_count);

/**
 * @brief The backend function behind ::lh_sql_driver_column_is_null: the driver's
 *        `column_is_null` entry (level 1).
 */
lh_sql_column_is_null_fn *
lh_sql_driver_get_column_is_null_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `column_is_null` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_column_is_null_fn(lh_sql_driver_t *self, lh_sql_column_is_null_fn * column_is_null);

/**
 * @brief The backend function behind ::lh_sql_driver_column_sint: the driver's
 *        `column_sint` entry (level 1).
 */
lh_sql_column_sint_fn *
lh_sql_driver_get_column_sint_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `column_sint` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_column_sint_fn(lh_sql_driver_t *self, lh_sql_column_sint_fn * column_sint);

/**
 * @brief The backend function behind ::lh_sql_driver_column_str: the driver's
 *        `column_str` entry (level 1).
 */
lh_sql_column_str_fn *
lh_sql_driver_get_column_str_fn(const lh_sql_driver_t *self);

/** @brief Install the driver's `column_str` entry. Level-1 setter: not checked. */
lh_void
lh_sql_driver_set_column_str_fn(lh_sql_driver_t *self, lh_sql_column_str_fn * column_str);

/** Call the `close` slot of @p self. */
lh_void
lh_sql_driver_close(const lh_sql_driver_t *self, struct lh_sql_db *db);

/** Call the `exec` slot of @p self. */
lh_bool_t
lh_sql_driver_exec(const lh_sql_driver_t *self, struct lh_sql_db *db, lh_str_view_t sql);

/** Call the `last_insert_id` slot of @p self. */
lh_sllong_t
lh_sql_driver_get_last_insert_id(const lh_sql_driver_t *self, struct lh_sql_db *db);

/** Call the `prepare` slot of @p self. */
lh_bool_t
lh_sql_driver_prepare(const lh_sql_driver_t *self, struct lh_sql_db *db, struct lh_sql_stmt *stmt,
                      lh_str_view_t sql);

/** Call the `finalize` slot of @p self. */
lh_void
lh_sql_driver_finalize(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt);

/** Call the `bind_null` slot of @p self. */
lh_bool_t
lh_sql_driver_bind_null(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt, lh_usize_t index);

/** Call the `bind_sint` slot of @p self. */
lh_bool_t
lh_sql_driver_bind_sint(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt, lh_usize_t index,
                        lh_sllong_t value);

/** Call the `bind_str` slot of @p self. */
lh_bool_t
lh_sql_driver_bind_str(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt, lh_usize_t index,
                       lh_str_view_t value);

/** Call the `step` slot of @p self. */
lh_sql_step_t
lh_sql_driver_step(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt);

/** Call the `reset` slot of @p self. */
lh_bool_t
lh_sql_driver_reset(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt);

/** Call the `column_count` slot of @p self. */
lh_usize_t
lh_sql_driver_get_column_count(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt);

/** Call the `column_is_null` slot of @p self. */
lh_bool_t
lh_sql_driver_is_column_null(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt,
                             lh_usize_t index);

/** Call the `column_sint` slot of @p self. */
lh_sllong_t
lh_sql_driver_get_column_sint(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt,
                              lh_usize_t index);

/** Call the `column_str` slot of @p self. */
lh_str_view_t
lh_sql_driver_get_column_str(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt,
                             lh_usize_t index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_DRIVER_H */
