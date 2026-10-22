/**
 * @file native.h
 * @brief SQLite's own handles behind an ::lh_sql_db_t / ::lh_sql_stmt_t.
 *
 * Library-private: it needs `sqlite3.h`, which lh does not hand to its
 * dependents.
 */

#ifndef LH_SQL_SQLITE_NATIVE_H
#define LH_SQL_SQLITE_NATIVE_H

#include <lh/sql/sqlite.h>
#include <sqlite3.h>

LH_COMPILER_EXTERN_C_BEGIN

/** The `sqlite3 *` of @p db. */
sqlite3 *
lh_sql_sqlite_get_native_db(const lh_sql_db_t *db);

/** The `sqlite3_stmt *` of @p stmt. */
sqlite3_stmt *
lh_sql_sqlite_get_native_stmt(const lh_sql_stmt_t *stmt);

/**
 * @brief @p index as the `int` SQLite counts in, into @p out.
 *
 * @return ::lh_bool_false when it does not fit an `int`.
 */
lh_bool_t
lh_sql_sqlite_to_int(lh_usize_t index, int *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_SQLITE_NATIVE_H */
