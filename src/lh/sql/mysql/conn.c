#include <lh/sql/mysql/conn.h>
#include <lh/assert.h>
#include <lh/runtime/allocator.h>

const lh_sql_mysql_api_t *
lh_sql_mysql_conn_get_api(const lh_sql_mysql_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->api;
}

lh_void
lh_sql_mysql_conn_set_api(lh_sql_mysql_conn_t *self, const lh_sql_mysql_api_t *api)
{
    lh_assert_runtime_ref(self);
    self->api = api;
}

lh_ptr
lh_sql_mysql_conn_get_native(const lh_sql_mysql_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->native;
}

lh_void
lh_sql_mysql_conn_set_native(lh_sql_mysql_conn_t *self, lh_ptr native)
{
    lh_assert_runtime_ref(self);
    self->native = native;
}

lh_str_view_t
lh_sql_mysql_conn_get_host(const lh_sql_mysql_conn_t *self)
{
    lh_str_view_t view;

    lh_assert_runtime_ref(self);
    lh_str_as_view(lh_addr_of(self->host), lh_addr_of(view));
    return view;
}

lh_str_view_t
lh_sql_mysql_conn_get_user(const lh_sql_mysql_conn_t *self)
{
    lh_str_view_t view;

    lh_assert_runtime_ref(self);
    lh_str_as_view(lh_addr_of(self->user), lh_addr_of(view));
    return view;
}

lh_str_view_t
lh_sql_mysql_conn_get_password(const lh_sql_mysql_conn_t *self)
{
    lh_str_view_t view;

    lh_assert_runtime_ref(self);
    lh_str_as_view(lh_addr_of(self->password), lh_addr_of(view));
    return view;
}

lh_str_view_t
lh_sql_mysql_conn_get_name(const lh_sql_mysql_conn_t *self)
{
    lh_str_view_t view;

    lh_assert_runtime_ref(self);
    lh_str_as_view(lh_addr_of(self->name), lh_addr_of(view));
    return view;
}

lh_uint_t
lh_sql_mysql_conn_get_port(const lh_sql_mysql_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->port;
}

lh_sql_mysql_conn_t *
lh_sql_mysql_conn_create(const lh_sql_mysql_api_t *api, lh_ptr native, lh_str_view_t host,
                         lh_str_view_t user, lh_str_view_t password, lh_str_view_t name,
                         lh_uint_t port)
{
    lh_sql_mysql_conn_t *self =
        lh_ptr_cast(lh_sql_mysql_conn_t, lh_runtime_allocator_alloc(sizeof(lh_sql_mysql_conn_t)));

    lh_sql_mysql_conn_set_api(self, api);
    self->broken = lh_bool_false;
    lh_sql_mysql_conn_set_native(self, native);
    lh_str_init_by_view(lh_addr_of(self->host), host);
    lh_str_init_by_view(lh_addr_of(self->user), user);
    lh_str_init_by_view(lh_addr_of(self->password), password);
    lh_str_init_by_view(lh_addr_of(self->name), name);
    self->port = port;
    return self;
}

lh_void
lh_sql_mysql_conn_destroy(lh_sql_mysql_conn_t *self)
{
    lh_sql_mysql_api_close(lh_sql_mysql_conn_get_api(self), lh_sql_mysql_conn_get_native(self));
    lh_str_deinit(lh_addr_of(self->host));
    lh_str_deinit(lh_addr_of(self->user));
    lh_str_deinit(lh_addr_of(self->password));
    lh_str_deinit(lh_addr_of(self->name));
    lh_runtime_allocator_free(self);
}
