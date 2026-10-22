/**
 * @file fields.h
 * @brief Member fields of ::lh_sql_stmt_t.
 */

#ifndef LH_SQL_STMT_FIELDS_H
#define LH_SQL_STMT_FIELDS_H

/**
 * @def lh_sql_stmt_fields(db_type, handle_type)
 * @brief The database a statement runs on and the backend's own statement.
 *
 * @param db_type     Type of `db` (pointer to ::lh_sql_db_t).
 * @param handle_type Type of `handle` (::lh_ptr; an `sqlite3_stmt *`, ...).
 */
#define lh_sql_stmt_fields(db_type, handle_type)                                                   \
    db_type db;                                                                                    \
    handle_type handle

#endif /* LH_SQL_STMT_FIELDS_H */
