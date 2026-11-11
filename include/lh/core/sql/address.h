/**
 * @file address.h
 * @brief Where a database is (::lh_sql_address_t): the backend and its address.
 *
 * Everything needed to open a connection, as owned text: the backend
 * (::lh_sql_driver_t, as its backend hands it out — ::lh_os_sql_sqlite_get_driver,
 * ::lh_os_sql_mysql_get_driver), the database file of a file backend, or the
 * host, user, password, database and port of a server backend. The fields a
 * backend does not use stay empty.
 *
 * It is data only: opening a connection by it is the backends'
 * (::lh_os_sql_address_try_open). Two addresses are equal when every field is
 * (::lh_sql_address_is_equal), which tells a kept connection whether it still
 * fits (::lh_os_sql_keep_t). The password is wiped when the address is cleared
 * or released.
 *
 * Example usage:
 * @code{.c}
 * lh_sql_address_t address;
 *
 * lh_sql_address_init(&address);
 * lh_sql_address_set_driver(&address, lh_os_sql_sqlite_get_driver());
 * lh_str_assign_view(lh_sql_address_get_path_mut(&address), lh_str_view_lit("bans.db"));
 * // ...
 * lh_sql_address_deinit(&address);
 * @endcode
 */

#ifndef LH_SQL_ADDRESS_H
#define LH_SQL_ADDRESS_H

#include <lh/core/sql/address/fields.h>
#include <lh/core/sql/driver.h>
#include <lh/core/str.h>

/**
 * @struct lh_sql_address
 * @brief Backend, file, server address. Fields via ::lh_sql_address_fields.
 */
struct lh_sql_address
{
    /** Members: ::lh_sql_address_fields. */
    lh_sql_address_fields(const lh_sql_driver_t *, lh_str_t, lh_uint_t);
};
/** @copybrief lh_sql_address */
typedef struct lh_sql_address lh_sql_address_t;

LH_COMPILER_EXTERN_C_BEGIN

/** The backend (level 1); ::lh_null when @p self names no database. */
const lh_sql_driver_t *
lh_sql_address_get_driver(const lh_sql_address_t *self);

/** Store the backend (level 1); not validated. */
lh_void
lh_sql_address_set_driver(lh_sql_address_t *self, const lh_sql_driver_t *driver);

/** The database file of a file backend (level 1). */
const lh_str_t *
lh_sql_address_get_path(const lh_sql_address_t *self);

/** The database file of a file backend, for change (level 1). */
lh_str_t *
lh_sql_address_get_path_mut(lh_sql_address_t *self);

/** The server host (level 1). */
const lh_str_t *
lh_sql_address_get_host(const lh_sql_address_t *self);

/** The server host, for change (level 1). */
lh_str_t *
lh_sql_address_get_host_mut(lh_sql_address_t *self);

/** The user name on the server (level 1). */
const lh_str_t *
lh_sql_address_get_user(const lh_sql_address_t *self);

/** The user name on the server, for change (level 1). */
lh_str_t *
lh_sql_address_get_user_mut(lh_sql_address_t *self);

/** The password on the server (level 1). */
const lh_str_t *
lh_sql_address_get_password(const lh_sql_address_t *self);

/** The password on the server, for change (level 1). */
lh_str_t *
lh_sql_address_get_password_mut(lh_sql_address_t *self);

/** The database on the server (level 1). */
const lh_str_t *
lh_sql_address_get_database(const lh_sql_address_t *self);

/** The database on the server, for change (level 1). */
lh_str_t *
lh_sql_address_get_database_mut(lh_sql_address_t *self);

/** The server port (level 1); 0 is the backend's default. */
lh_uint_t
lh_sql_address_get_port(const lh_sql_address_t *self);

/** Store the server port (level 1); not validated. */
lh_void
lh_sql_address_set_port(lh_sql_address_t *self, lh_uint_t port);

/**
 * @brief No backend, every text empty, port 0 (level 2). Allocates nothing.
 *
 * @param self The address; checked by its accessors.
 */
lh_void
lh_sql_address_init(lh_sql_address_t *self);

/**
 * @brief Whether @p self names a database: it has a backend (level 2).
 */
lh_bool_t
lh_sql_address_is_set(const lh_sql_address_t *self);

/**
 * @brief Wipe the password, then make @p self empty as ::lh_sql_address_init
 *        does, keeping its storage for the next use (level 2).
 */
lh_void
lh_sql_address_clear(lh_sql_address_t *self);

/**
 * @brief Whether @p self and @p other name the same database as the same user:
 *        the same backend and every text and the port equal (level 2).
 *
 * Text is compared byte for byte (a host name in another case is another
 * address).
 */
lh_bool_t
lh_sql_address_is_equal(const lh_sql_address_t *self, const lh_sql_address_t *other);

/**
 * @brief Wipe the password and release every text (level 3). @p self may be
 *        ::lh_sql_address_init'ed again.
 */
lh_void
lh_sql_address_deinit(lh_sql_address_t *self);

/**
 * @brief Make @p self a copy of @p other (level 3). The old password is wiped
 *        first.
 *
 * @param self  The address to change.
 * @param other The address to copy; checked by its accessors.
 */
lh_void
lh_sql_address_assign(lh_sql_address_t *self, const lh_sql_address_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_ADDRESS_H */
