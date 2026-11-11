/**
 * @file fields.h
 * @brief Member fields of ::lh_os_sql_keep_t.
 */

#ifndef LH_OS_SQL_KEEP_FIELDS_H
#define LH_OS_SQL_KEEP_FIELDS_H

/**
 * @def lh_os_sql_keep_fields(db_type, address_type, flag_type)
 * @brief The connection kept, what it was opened on, and whether the thread
 *        has to let a client library go at its end.
 *
 * `db` is the connection, `address` the address it was opened on (not set
 * while nothing is kept), `needs_release` whether the MySQL client was used on
 * this thread (`mysql_thread_end` at ::lh_os_sql_keep_deinit).
 *
 * @param db_type      Type of `db` (::lh_sql_db_t).
 * @param address_type Type of `address` (::lh_sql_address_t).
 * @param flag_type    Type of `needs_release` (::lh_bool_t).
 */
#define lh_os_sql_keep_fields(db_type, address_type, flag_type)                                    \
    db_type db;                                                                                    \
    address_type address;                                                                          \
    flag_type needs_release

#endif /* LH_OS_SQL_KEEP_FIELDS_H */
