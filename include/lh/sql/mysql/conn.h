/**
 * @file conn.h
 * @brief What the handle of an ::lh_sql_db_t opened on MySQL points at
 *        (::lh_sql_mysql_conn_t): the client, its `MYSQL *`, and the
 *        address it was opened with.
 *
 * Every call has to go through the client the connection was made with, so
 * the two travel together. The address (password included) stays for as
 * long as the connection: the server drops idle connections, and the
 * backend opens a new one to the same place (::lh_sql_mysql_reconnect).
 */

#ifndef LH_SQL_MYSQL_CONN_H
#define LH_SQL_MYSQL_CONN_H

#include <lh/sql/mysql/api.h>
#include <lh/sql/mysql/conn/fields.h>
#include <lh/str.h>

/**
 * @struct lh_sql_mysql_conn
 * @brief Client, connection and address. Fields via ::lh_sql_mysql_conn_fields.
 */
struct lh_sql_mysql_conn
{
    lh_bool_t broken;
    lh_sql_mysql_conn_fields(const lh_sql_mysql_api_t *, lh_ptr, lh_str_t, lh_uint_t);
};
/** @copybrief lh_sql_mysql_conn */
typedef struct lh_sql_mysql_conn lh_sql_mysql_conn_t;

LH_COMPILER_EXTERN_C_BEGIN

/** The client @p self was connected with. */
const lh_sql_mysql_api_t *
lh_sql_mysql_conn_get_api(const lh_sql_mysql_conn_t *self);

/** Make @p api the client @p self goes through. */
lh_void
lh_sql_mysql_conn_set_api(lh_sql_mysql_conn_t *self, const lh_sql_mysql_api_t *api);

/** The client's `MYSQL *`. */
lh_ptr
lh_sql_mysql_conn_get_native(const lh_sql_mysql_conn_t *self);

/** Make @p native the client's `MYSQL *`. */
lh_void
lh_sql_mysql_conn_set_native(lh_sql_mysql_conn_t *self, lh_ptr native);

/** The server's host. */
lh_str_view_t
lh_sql_mysql_conn_get_host(const lh_sql_mysql_conn_t *self);

/** The user @p self logged in as. */
lh_str_view_t
lh_sql_mysql_conn_get_user(const lh_sql_mysql_conn_t *self);

/** The user's password. */
lh_str_view_t
lh_sql_mysql_conn_get_password(const lh_sql_mysql_conn_t *self);

/** The database @p self uses. */
lh_str_view_t
lh_sql_mysql_conn_get_name(const lh_sql_mysql_conn_t *self);

/** The server's port. */
lh_uint_t
lh_sql_mysql_conn_get_port(const lh_sql_mysql_conn_t *self);

/**
 * @brief A new connection record for @p native on @p api, made to database
 *        @p name on @p host:@p port as @p user (all copied), from the
 *        runtime allocator.
 */
lh_sql_mysql_conn_t *
lh_sql_mysql_conn_create(const lh_sql_mysql_api_t *api, lh_ptr native, lh_str_view_t host,
                         lh_str_view_t user, lh_str_view_t password, lh_str_view_t name,
                         lh_uint_t port);

/** Close the connection with its client and free the record. */
lh_void
lh_sql_mysql_conn_destroy(lh_sql_mysql_conn_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_MYSQL_CONN_H */
