/**
 * @file fields.h
 * @brief Member fields of ::lh_sql_mysql_conn_t.
 */

#ifndef LH_SQL_MYSQL_CONN_FIELDS_H
#define LH_SQL_MYSQL_CONN_FIELDS_H

/**
 * @def lh_sql_mysql_conn_fields(api_type, native_type, text_type, port_type)
 * @brief The client a connection was made with, the connection, and where
 *        it goes — kept to connect again when the server drops it.
 *
 * @param api_type    Type of `api` (pointer to const ::lh_sql_mysql_api_t).
 * @param native_type Type of `native`, the client's `MYSQL *` (::lh_ptr).
 * @param text_type   Type of `host`, `user`, `password` and `name` (::lh_str_t).
 * @param port_type   Type of `port` (::lh_uint_t).
 */
#define lh_sql_mysql_conn_fields(api_type, native_type, text_type, port_type)                      \
    api_type api;                                                                                  \
    native_type native;                                                                            \
    text_type host;                                                                                \
    text_type user;                                                                                \
    text_type password;                                                                            \
    text_type name;                                                                                \
    port_type port

#endif /* LH_SQL_MYSQL_CONN_FIELDS_H */
