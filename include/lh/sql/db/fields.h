/**
 * @file fields.h
 * @brief Member fields of ::lh_sql_db_t.
 */

#ifndef LH_SQL_DB_FIELDS_H
#define LH_SQL_DB_FIELDS_H

/**
 * @def lh_sql_db_fields(driver_type, handle_type, error_type)
 * @brief Which backend, its connection, and what went wrong last.
 *
 * `driver` is the backend's function table (::lh_null while closed),
 * `handle` the backend's own connection (an `sqlite3 *`, a `MYSQL *`),
 * `error` the message of the last failure.
 *
 * @param driver_type Type of `driver` (pointer to const ::lh_sql_driver_t).
 * @param handle_type Type of `handle` (::lh_ptr).
 * @param error_type  Type of `error` (::lh_str_t).
 */
#define lh_sql_db_fields(driver_type, handle_type, error_type)                                     \
    driver_type driver;                                                                            \
    handle_type handle;                                                                            \
    error_type error

#endif /* LH_SQL_DB_FIELDS_H */
