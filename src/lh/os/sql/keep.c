#include <lh/os/sql/keep.h>
#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/os/sql/mysql.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

const lh_sql_db_t *
lh_os_sql_keep_get_db(const lh_os_sql_keep_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->db);
}

lh_sql_db_t *
lh_os_sql_keep_get_db_mut(lh_os_sql_keep_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->db);
}

const lh_sql_address_t *
lh_os_sql_keep_get_address(const lh_os_sql_keep_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->address);
}

lh_sql_address_t *
lh_os_sql_keep_get_address_mut(lh_os_sql_keep_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->address);
}

lh_bool_t
lh_os_sql_keep_needs_release(const lh_os_sql_keep_t *self)
{
    lh_assert_runtime_ref(self);
    return self->needs_release;
}

lh_void
lh_os_sql_keep_set_needs_release(lh_os_sql_keep_t *self, lh_bool_t needs_release)
{
    lh_assert_runtime_ref(self);
    self->needs_release = needs_release;
}

/* ── lifecycle ───────────────────────────────────────────────────────────── */

lh_void
lh_os_sql_keep_init(lh_os_sql_keep_t *self)
{
    lh_sql_db_init(lh_os_sql_keep_get_db_mut(self));
    lh_sql_address_init(lh_os_sql_keep_get_address_mut(self));
    lh_os_sql_keep_set_needs_release(self, lh_bool_false);
}

lh_void
lh_os_sql_keep_close(lh_os_sql_keep_t *self)
{
    lh_sql_db_close(lh_os_sql_keep_get_db_mut(self));
    lh_sql_address_clear(lh_os_sql_keep_get_address_mut(self));
}

lh_sql_db_t *
lh_os_sql_keep_try_get_db(lh_os_sql_keep_t *self, const lh_sql_address_t *address)
{
    lh_sql_db_t *db = lh_os_sql_keep_get_db_mut(self);

    if (lh_sql_address_is_set(lh_os_sql_keep_get_address(self)) && lh_sql_db_is_open(db) &&
        lh_sql_address_is_equal(lh_os_sql_keep_get_address(self), address))
    {
        lh_sql_db_clear_error(db);
        return db;
    }
    lh_os_sql_keep_close(self);
    if (lh_math_eq(lh_sql_address_get_driver(address), lh_os_sql_mysql_get_driver()))
    {
        lh_os_sql_keep_set_needs_release(self, lh_bool_true);
    }
    if (!lh_os_sql_address_try_open(address, db))
    {
        return lh_null;
    }
    lh_sql_address_assign(lh_os_sql_keep_get_address_mut(self), address);
    return db;
}

lh_void
lh_os_sql_keep_deinit(lh_os_sql_keep_t *self)
{
    lh_os_sql_keep_close(self);
    lh_sql_db_deinit(lh_os_sql_keep_get_db_mut(self));
    lh_sql_address_deinit(lh_os_sql_keep_get_address_mut(self));
    if (lh_os_sql_keep_needs_release(self))
    {
        /* No client was ever loaded: there is nothing to let go. */
        const lh_os_sql_mysql_api_t *api = lh_os_sql_mysql_api_get_default();

        if (lh_null_ne(api))
        {
            lh_os_sql_mysql_api_thread_end(api);
        }
        lh_os_sql_keep_set_needs_release(self, lh_bool_false);
    }
}
