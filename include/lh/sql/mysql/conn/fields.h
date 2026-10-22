/**
 * @file fields.h
 * @brief Member fields of ::lh_sql_mysql_conn_t.
 */

#ifndef LH_SQL_MYSQL_CONN_FIELDS_H
#define LH_SQL_MYSQL_CONN_FIELDS_H

/**
 * @def lh_sql_mysql_conn_fields(api_type, native_type)
 * @brief The client a connection was made with, and the connection.
 *
 * @param api_type    Type of `api` (pointer to const ::lh_sql_mysql_api_t).
 * @param native_type Type of `native`, the client's `MYSQL *` (::lh_ptr).
 */
#define lh_sql_mysql_conn_fields(api_type, native_type)                                            \
    api_type api;                                                                                  \
    native_type native

#endif /* LH_SQL_MYSQL_CONN_FIELDS_H */
