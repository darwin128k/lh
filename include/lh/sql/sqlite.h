/**
 * @file sqlite.h
 * @brief The SQLite backend of lh/sql: SQLite 3.46.1, compiled into lh.
 *
 * Only with ::LH_LIBRARY_OPTION_SQL_SQLITE. The database is a file (created
 * when missing) or `:memory:`. ::lh_sql_db_exec runs every statement of a
 * script; ::lh_sql_db_prepare compiles the first. Integers are 64-bit,
 * text is bound and read as UTF-8 with its length, so it may hold any byte.
 *
 * The callbacks of the driver are public like any brick; a caller only
 * needs ::lh_sql_db_open_sqlite and then lh/sql/db.h and lh/sql/stmt.h.
 */

#ifndef LH_SQL_SQLITE_H
#define LH_SQL_SQLITE_H

#include <lh/config.h>

#if !LH_LIBRARY_OPTION_SQL_SQLITE
#    error "lh/sql/sqlite.h requires LH_LIBRARY_OPTION_SQL_SQLITE (CMake: -D...SQL_SQLITE=ON)"
#endif

#include <lh/sql/stmt.h>

LH_COMPILER_EXTERN_C_BEGIN

/* ── driver callbacks ────────────────────────────────────────────────────── */

/** Copy SQLite's message for the last failure on @p db into its error; ::lh_bool_false. */
lh_bool_t
lh_sql_sqlite_fail(lh_sql_db_t *db);

/** ::lh_sql_close_fn: close the SQLite connection of @p db. */
lh_void
lh_sql_sqlite_close(lh_sql_db_t *db);

/** ::lh_sql_exec_fn: run every statement of @p sql in turn, ignoring rows. */
lh_bool_t
lh_sql_sqlite_exec(lh_sql_db_t *db, lh_str_view_t sql);

/** ::lh_sql_last_insert_id_fn: `sqlite3_last_insert_rowid`. */
lh_sllong_t
lh_sql_sqlite_get_last_insert_id(lh_sql_db_t *db);

/** ::lh_sql_prepare_fn: compile the first statement of @p sql into @p stmt. */
lh_bool_t
lh_sql_sqlite_prepare(lh_sql_db_t *db, lh_sql_stmt_t *stmt, lh_str_view_t sql);

/** ::lh_sql_finalize_fn: `sqlite3_finalize`. */
lh_void
lh_sql_sqlite_finalize(lh_sql_stmt_t *stmt);

/** ::lh_sql_bind_null_fn. */
lh_bool_t
lh_sql_sqlite_bind_null(lh_sql_stmt_t *stmt, lh_usize_t index);

/** ::lh_sql_bind_sint_fn: a 64-bit integer. */
lh_bool_t
lh_sql_sqlite_bind_sint(lh_sql_stmt_t *stmt, lh_usize_t index, lh_sllong_t value);

/** ::lh_sql_bind_str_fn: text, copied by SQLite. */
lh_bool_t
lh_sql_sqlite_bind_str(lh_sql_stmt_t *stmt, lh_usize_t index, lh_str_view_t value);

/** ::lh_sql_step_fn: `sqlite3_step`. */
lh_sql_step_t
lh_sql_sqlite_step(lh_sql_stmt_t *stmt);

/** ::lh_sql_reset_fn: `sqlite3_reset`; the bindings stay. */
lh_bool_t
lh_sql_sqlite_reset(lh_sql_stmt_t *stmt);

/** ::lh_sql_column_count_fn. */
lh_usize_t
lh_sql_sqlite_get_column_count(const lh_sql_stmt_t *stmt);

/** ::lh_sql_column_is_null_fn. */
lh_bool_t
lh_sql_sqlite_is_column_null(const lh_sql_stmt_t *stmt, lh_usize_t index);

/** ::lh_sql_column_sint_fn: `sqlite3_column_int64`. */
lh_sllong_t
lh_sql_sqlite_get_column_sint(const lh_sql_stmt_t *stmt, lh_usize_t index);

/** ::lh_sql_column_str_fn: `sqlite3_column_text` with its length. */
lh_str_view_t
lh_sql_sqlite_get_column_str(const lh_sql_stmt_t *stmt, lh_usize_t index);

/* ── open ────────────────────────────────────────────────────────────────── */

/** The SQLite function table. */
const lh_sql_driver_t *
lh_sql_sqlite_get_driver(void);

/**
 * @brief Open (create when missing) the SQLite database at @p path —
 *        `:memory:` for one that lives in memory — into @p db, closing
 *        whatever @p db had open.
 *
 * @return ::lh_bool_false, with the error set and @p db closed, when SQLite
 *         cannot open it.
 */
lh_bool_t
lh_sql_db_open_sqlite(lh_sql_db_t *db, lh_str_view_t path);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_SQLITE_H */
