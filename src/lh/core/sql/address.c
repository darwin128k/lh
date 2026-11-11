#include <lh/core/sql/address.h>
#include <lh/core/assert.h>
#include <lh/core/cast/const.h>
#include <lh/core/crypto/wipe.h>
#include <lh/core/math.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

const lh_sql_driver_t *
lh_sql_address_get_driver(const lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return self->driver;
}

lh_void
lh_sql_address_set_driver(lh_sql_address_t *self, const lh_sql_driver_t *driver)
{
    lh_assert_runtime_ref(self);
    self->driver = driver;
}

const lh_str_t *
lh_sql_address_get_path(const lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

lh_str_t *
lh_sql_address_get_path_mut(lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

const lh_str_t *
lh_sql_address_get_host(const lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->host);
}

lh_str_t *
lh_sql_address_get_host_mut(lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->host);
}

const lh_str_t *
lh_sql_address_get_user(const lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->user);
}

lh_str_t *
lh_sql_address_get_user_mut(lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->user);
}

const lh_str_t *
lh_sql_address_get_password(const lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->password);
}

lh_str_t *
lh_sql_address_get_password_mut(lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->password);
}

const lh_str_t *
lh_sql_address_get_database(const lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->database);
}

lh_str_t *
lh_sql_address_get_database_mut(lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->database);
}

lh_uint_t
lh_sql_address_get_port(const lh_sql_address_t *self)
{
    lh_assert_runtime_ref(self);
    return self->port;
}

lh_void
lh_sql_address_set_port(lh_sql_address_t *self, lh_uint_t port)
{
    lh_assert_runtime_ref(self);
    self->port = port;
}

/* ── built on the accessors ──────────────────────────────────────────────── */

lh_void
lh_sql_address_init(lh_sql_address_t *self)
{
    lh_sql_address_set_driver(self, lh_null);
    lh_str_init(lh_sql_address_get_path_mut(self));
    lh_str_init(lh_sql_address_get_host_mut(self));
    lh_str_init(lh_sql_address_get_user_mut(self));
    lh_str_init(lh_sql_address_get_password_mut(self));
    lh_str_init(lh_sql_address_get_database_mut(self));
    lh_sql_address_set_port(self, 0U);
}

lh_bool_t
lh_sql_address_is_set(const lh_sql_address_t *self)
{
    return lh_null_ne(lh_sql_address_get_driver(self));
}

lh_void
lh_sql_address_clear(lh_sql_address_t *self)
{
    lh_str_t *password = lh_sql_address_get_password_mut(self);

    lh_crypto_wipe(lh_cast_const(lh_ptr, lh_str_get_data(password)), lh_str_get_size(password));
    lh_str_clear(password);
    lh_sql_address_set_driver(self, lh_null);
    lh_str_clear(lh_sql_address_get_path_mut(self));
    lh_str_clear(lh_sql_address_get_host_mut(self));
    lh_str_clear(lh_sql_address_get_user_mut(self));
    lh_str_clear(lh_sql_address_get_database_mut(self));
    lh_sql_address_set_port(self, 0U);
}

lh_bool_t
lh_sql_address_is_equal(const lh_sql_address_t *self, const lh_sql_address_t *other)
{
    return lh_cast_static(
        lh_bool_t,
        lh_math_eq(lh_sql_address_get_driver(self), lh_sql_address_get_driver(other)) &&
            lh_math_eq(lh_sql_address_get_port(self), lh_sql_address_get_port(other)) &&
            lh_str_is_equal(lh_sql_address_get_path(self), lh_sql_address_get_path(other)) &&
            lh_str_is_equal(lh_sql_address_get_host(self), lh_sql_address_get_host(other)) &&
            lh_str_is_equal(lh_sql_address_get_user(self), lh_sql_address_get_user(other)) &&
            lh_str_is_equal(lh_sql_address_get_database(self),
                            lh_sql_address_get_database(other)) &&
            lh_str_is_equal(lh_sql_address_get_password(self), lh_sql_address_get_password(other)));
}

lh_void
lh_sql_address_deinit(lh_sql_address_t *self)
{
    lh_sql_address_clear(self);
    lh_str_deinit(lh_sql_address_get_path_mut(self));
    lh_str_deinit(lh_sql_address_get_host_mut(self));
    lh_str_deinit(lh_sql_address_get_user_mut(self));
    lh_str_deinit(lh_sql_address_get_password_mut(self));
    lh_str_deinit(lh_sql_address_get_database_mut(self));
}

lh_void
lh_sql_address_assign(lh_sql_address_t *self, const lh_sql_address_t *other)
{
    if (self == other)
    {
        return;
    }
    lh_sql_address_clear(self);
    lh_sql_address_set_driver(self, lh_sql_address_get_driver(other));
    lh_str_assign(lh_sql_address_get_path_mut(self), lh_sql_address_get_path(other));
    lh_str_assign(lh_sql_address_get_host_mut(self), lh_sql_address_get_host(other));
    lh_str_assign(lh_sql_address_get_user_mut(self), lh_sql_address_get_user(other));
    lh_str_assign(lh_sql_address_get_password_mut(self), lh_sql_address_get_password(other));
    lh_str_assign(lh_sql_address_get_database_mut(self), lh_sql_address_get_database(other));
    lh_sql_address_set_port(self, lh_sql_address_get_port(other));
}
