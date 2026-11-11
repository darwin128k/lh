/**
 * @file fields.h
 * @brief Member fields of ::lh_sql_address_t.
 */

#ifndef LH_SQL_ADDRESS_FIELDS_H
#define LH_SQL_ADDRESS_FIELDS_H

/**
 * @def lh_sql_address_fields(driver_type, text_type, port_type)
 * @brief Which backend, and where and as whom it connects.
 *
 * `driver` is the backend's function table (::lh_null: no address), `path`
 * the database file of a file backend, `host`, `user`, `password`, `database`
 * and `port` the server address of a server backend.
 *
 * @param driver_type Type of `driver` (pointer to const ::lh_sql_driver_t).
 * @param text_type   Type of the text fields (::lh_str_t).
 * @param port_type   Type of `port` (::lh_uint_t).
 */
#define lh_sql_address_fields(driver_type, text_type, port_type)                                   \
    driver_type driver;                                                                            \
    text_type path;                                                                                \
    text_type host;                                                                                \
    text_type user;                                                                                \
    text_type password;                                                                            \
    text_type database;                                                                            \
    port_type port

#endif /* LH_SQL_ADDRESS_FIELDS_H */
