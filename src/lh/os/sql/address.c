#include <lh/os/sql/address.h>
#include <lh/core/math.h>
#include <lh/os/sql/mysql.h>

#if LH_LIBRARY_OPTION_SQL_SQLITE
#    include <lh/os/sql/sqlite.h>
#endif

lh_bool_t
lh_os_sql_address_try_open(const lh_sql_address_t *address, lh_sql_db_t *db)
{
    const lh_sql_driver_t *driver = lh_sql_address_get_driver(address);

    lh_sql_db_close(db);
    if (lh_math_eq(driver, lh_os_sql_mysql_get_driver()))
    {
        lh_str_view_t host;

        lh_str_as_view(lh_sql_address_get_host(address), lh_addr_of(host));

        lh_str_view_t user;

        lh_str_as_view(lh_sql_address_get_user(address), lh_addr_of(user));

        lh_str_view_t password;

        lh_str_as_view(lh_sql_address_get_password(address), lh_addr_of(password));

        lh_str_view_t database;

        lh_str_as_view(lh_sql_address_get_database(address), lh_addr_of(database));
        return lh_os_sql_mysql_open(db, host, user, password, database,
                                    lh_sql_address_get_port(address));
    }
#if LH_LIBRARY_OPTION_SQL_SQLITE
    if (lh_math_eq(driver, lh_os_sql_sqlite_get_driver()))
    {
        lh_str_view_t path;

        lh_str_as_view(lh_sql_address_get_path(address), lh_addr_of(path));
        return lh_os_sql_sqlite_open(db, path);
    }
#endif
    return lh_sql_db_fail_as(db, lh_sql_error_kind_unsupported, 0,
                             lh_str_view_lit("the address names no backend this build opens"));
}
