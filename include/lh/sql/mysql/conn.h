/**
 * @file conn.h
 * @brief What the handle of an ::lh_sql_db_t opened on MySQL points at
 *        (::lh_sql_mysql_conn_t): the client and its `MYSQL *`.
 *
 * Every call has to go through the client the connection was made with, so
 * the two travel together.
 */

#ifndef LH_SQL_MYSQL_CONN_H
#define LH_SQL_MYSQL_CONN_H

#include <lh/sql/mysql/api.h>
#include <lh/sql/mysql/conn/fields.h>

/**
 * @struct lh_sql_mysql_conn
 * @brief Client plus connection. Fields via ::lh_sql_mysql_conn_fields.
 */
struct lh_sql_mysql_conn
{
    lh_sql_mysql_conn_fields(const lh_sql_mysql_api_t *, lh_ptr);
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

/** A new connection record for @p native on @p api, from the runtime allocator. */
lh_sql_mysql_conn_t *
lh_sql_mysql_conn_create(const lh_sql_mysql_api_t *api, lh_ptr native);

/** Close the connection with its client and free the record. */
lh_void
lh_sql_mysql_conn_destroy(lh_sql_mysql_conn_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_MYSQL_CONN_H */
