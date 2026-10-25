/**
 * @file mysql.h
 * @brief The MySQL / MariaDB backend of lh/sql, on the host's client
 *        library loaded at run time (lh/sql/mysql/api.h).
 *
 * The connection speaks utf8mb4 (utf8 on servers older than 5.5.3), so
 * text goes in and out as UTF-8 and escaping is safe for every byte.
 * Statements run as text (lh/sql/mysql/query.h): a bound value becomes an
 * escaped literal at bind time and replaces its `?` when the statement
 * runs; all rows are fetched on the first step (`mysql_store_result`).
 * ::lh_sql_db_exec runs one statement: the client is connected without
 * `CLIENT_MULTI_STATEMENTS`, as `;`-joined input is how injection spreads.
 *
 * The server says nothing when it drops a connection — idle past its
 * `wait_timeout`, restarted — so the backend finds out from the statement
 * that hits it. When the client says that statement never reached a live
 * server (::LH_SQL_MYSQL_ERROR_SERVER_GONE, ::LH_SQL_MYSQL_ERROR_IDLE_TIMEOUT),
 * the backend connects again to the same address and sends it once more;
 * the caller sees one statement that ran. A connection lost while a
 * statement ran (::LH_SQL_MYSQL_ERROR_SERVER_LOST) is reported, not resent:
 * the statement may have taken effect. The next one finds the connection
 * gone and reconnects.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_SQL_MYSQL_H
#define LH_SQL_MYSQL_H

#include <lh/sql/mysql/conn.h>
#include <lh/sql/mysql/prepared.h>
#include <lh/sql/stmt.h>

/** @brief The port ::lh_sql_db_open_mysql uses for `0`. */
#define LH_SQL_MYSQL_PORT_DEFAULT 3306U

/** @brief Characters the longest 64-bit integer literal takes: a sign and 20 digits. */
#define LH_SQL_MYSQL_INTEGER_DIGITS_MAX 21U

/** @brief `CR_SERVER_GONE_ERROR`: the connection was gone before the statement was sent. */
#define LH_SQL_MYSQL_ERROR_SERVER_GONE 2006U

/** @brief `CR_SERVER_LOST`: the connection broke while a statement ran. */
#define LH_SQL_MYSQL_ERROR_SERVER_LOST 2013U

/**
 * @brief `ER_CLIENT_INTERACTION_TIMEOUT` (MySQL 8.0.24+): the server closed the
 *        connection as idle, and said so, before it read the statement.
 */
#define LH_SQL_MYSQL_ERROR_IDLE_TIMEOUT 4031U

LH_COMPILER_EXTERN_C_BEGIN

/* ── state behind the handles ────────────────────────────────────────────── */

/** The connection record of @p db, opened on MySQL. */
lh_sql_mysql_conn_t *
lh_sql_mysql_get_conn(const lh_sql_db_t *db);

/** The client @p db was opened with. */
const lh_sql_mysql_api_t *
lh_sql_mysql_get_api(const lh_sql_db_t *db);

/** The `MYSQL *` of @p db. */
lh_ptr
lh_sql_mysql_get_native(const lh_sql_db_t *db);

/** The text statement behind @p stmt, prepared on MySQL. */
lh_sql_mysql_prepared_t *
lh_sql_mysql_get_prepared(const lh_sql_stmt_t *stmt);

/** Copy the client's message for the last failure on @p db into its error; ::lh_bool_false. */
lh_bool_t
lh_sql_mysql_fail(lh_sql_db_t *db);

/** Write @p text as a quoted, escaped SQL string literal into @p out (replacing it). */
lh_void
lh_sql_mysql_quote(const lh_sql_db_t *db, lh_str_view_t text, lh_str_t *out);

/**
 * @brief Open a new connection of @p db to the address it was opened with
 *        and put it in place of the old one, which is closed. Statements
 *        prepared on @p db stay usable.
 *
 * @return ::lh_bool_false, with the error set, when the server refuses or
 *         cannot be reached; @p db then keeps the old connection, so the
 *         next statement finds it gone and tries again.
 */
lh_bool_t
lh_sql_mysql_reconnect(lh_sql_db_t *db);

/**
 * @brief Send @p sql (`mysql_real_query`); when it never reached a live
 *        server, reconnect and send it once more.
 *
 * @return ::lh_bool_false, with the error set, when it failed.
 */
lh_bool_t
lh_sql_mysql_send(lh_sql_db_t *db, lh_str_view_t sql);

/* ── driver callbacks ────────────────────────────────────────────────────── */

/** ::lh_sql_close_fn: close the connection and free its record. */
lh_void
lh_sql_mysql_close(lh_sql_db_t *db);

/** ::lh_sql_exec_fn: run one statement, dropping any rows it returns. */
lh_bool_t
lh_sql_mysql_exec(lh_sql_db_t *db, lh_str_view_t sql);

/** ::lh_sql_last_insert_id_fn: `mysql_insert_id`. */
lh_sllong_t
lh_sql_mysql_get_last_insert_id(lh_sql_db_t *db);

/** ::lh_sql_prepare_fn: keep a copy of @p sql; nothing is sent yet. */
lh_bool_t
lh_sql_mysql_prepare(lh_sql_db_t *db, lh_sql_stmt_t *stmt, lh_str_view_t sql);

/** ::lh_sql_finalize_fn. */
lh_void
lh_sql_mysql_finalize(lh_sql_stmt_t *stmt);

/** ::lh_sql_bind_null_fn: the literal `NULL`. */
lh_bool_t
lh_sql_mysql_bind_null(lh_sql_stmt_t *stmt, lh_usize_t index);

/** ::lh_sql_bind_sint_fn: the integer, in decimal. */
lh_bool_t
lh_sql_mysql_bind_sint(lh_sql_stmt_t *stmt, lh_usize_t index, lh_sllong_t value);

/** ::lh_sql_bind_str_fn: the text, quoted and escaped by the client. */
lh_bool_t
lh_sql_mysql_bind_str(lh_sql_stmt_t *stmt, lh_usize_t index, lh_str_view_t value);

/**
 * @brief Send the statement text, literals in, and keep the rows it
 *        returned (if any).
 */
lh_bool_t
lh_sql_mysql_execute(lh_sql_stmt_t *stmt);

/** ::lh_sql_step_fn: run on the first step, then hand out one row per step. */
lh_sql_step_t
lh_sql_mysql_step(lh_sql_stmt_t *stmt);

/** ::lh_sql_reset_fn: drop the rows; the next step runs the statement again. */
lh_bool_t
lh_sql_mysql_reset(lh_sql_stmt_t *stmt);

/** ::lh_sql_column_count_fn: `mysql_num_fields`, `0` before the first step. */
lh_usize_t
lh_sql_mysql_get_column_count(const lh_sql_stmt_t *stmt);

/** ::lh_sql_column_is_null_fn; ::lh_bool_true past the last column. */
lh_bool_t
lh_sql_mysql_is_column_null(const lh_sql_stmt_t *stmt, lh_usize_t index);

/** ::lh_sql_column_sint_fn: the column's text as a 64-bit integer; `0` if it is not one. */
lh_sllong_t
lh_sql_mysql_get_column_sint(const lh_sql_stmt_t *stmt, lh_usize_t index);

/** ::lh_sql_column_str_fn: the column's bytes, with their length. */
lh_str_view_t
lh_sql_mysql_get_column_str(const lh_sql_stmt_t *stmt, lh_usize_t index);

/* ── open ────────────────────────────────────────────────────────────────── */

/** The MySQL function table. */
const lh_sql_driver_t *
lh_sql_mysql_get_driver(void);

/**
 * @brief @p text as a NUL-terminated string the client can take, kept in
 *        @p storage (initialized here; deinit it after the call). `""` for
 *        empty text.
 */
const lh_char_t *
lh_sql_mysql_to_cstr(lh_str_t *storage, lh_str_view_t text);

/**
 * @brief A client connection to database @p name on @p host:@p port as
 *        @p user through @p api, speaking utf8mb4 (utf8 on old servers).
 *
 * @return ::lh_null, with the error set on @p db, when the server refuses
 *         or cannot be reached.
 */
lh_ptr
lh_sql_mysql_connect(lh_sql_db_t *db, const lh_sql_mysql_api_t *api, lh_str_view_t host,
                     lh_str_view_t user, lh_str_view_t password, lh_str_view_t name,
                     lh_uint_t port);

/**
 * @brief Connect @p db to database @p name on @p host:@p port as @p user
 *        through the client @p api, closing whatever @p db had open.
 *        @p port `0` is ::LH_SQL_MYSQL_PORT_DEFAULT.
 *
 * @return ::lh_bool_false, with the error set and @p db closed, when the
 *         server refuses or cannot be reached.
 */
lh_bool_t
lh_sql_db_open_mysql_with(lh_sql_db_t *db, const lh_sql_mysql_api_t *api, lh_str_view_t host,
                          lh_str_view_t user, lh_str_view_t password, lh_str_view_t name,
                          lh_uint_t port);

/**
 * @brief ::lh_sql_db_open_mysql_with on the process's client
 *        (::lh_sql_mysql_api_get_default).
 *
 * @return ::lh_bool_false, with the error set, also when no client library
 *         could be loaded.
 */
lh_bool_t
lh_sql_db_open_mysql(lh_sql_db_t *db, lh_str_view_t host, lh_str_view_t user,
                     lh_str_view_t password, lh_str_view_t name, lh_uint_t port);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_MYSQL_H */
