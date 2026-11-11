/**
 * @file address.h
 * @brief Open a connection by its ::lh_sql_address_t, whichever backend it names.
 *
 * The address's backend picks the open: ::lh_os_sql_sqlite_open with its path
 * (only with ::LH_LIBRARY_OPTION_SQL_SQLITE), ::lh_os_sql_mysql_open with its
 * host, user, password, database and port. An address whose backend neither
 * of them hands out cannot be opened here.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SQL_ADDRESS_H
#define LH_OS_SQL_ADDRESS_H

#include <lh/config.h>
#include <lh/core/sql/address.h>
#include <lh/core/sql/db.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/sql/address.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Open @p db on the database @p address names, closing whatever @p db
 *        had open.
 *
 * @param address Where to connect; checked by its accessors.
 * @param db      The connection to open; ::lh_sql_db_init'ed.
 * @return ::lh_bool_false, with the error set in @p db and @p db closed, when
 *         the backend refuses, or @p address names no backend this build can
 *         open.
 */
lh_bool_t
lh_os_sql_address_try_open(const lh_sql_address_t *address, lh_sql_db_t *db);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SQL_ADDRESS_H */
