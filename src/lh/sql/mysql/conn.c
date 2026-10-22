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

lh_sql_mysql_conn_t *
lh_sql_mysql_conn_create(const lh_sql_mysql_api_t *api, lh_ptr native)
{
    lh_sql_mysql_conn_t *self =
        lh_ptr_cast(lh_sql_mysql_conn_t, lh_runtime_allocator_alloc(sizeof(lh_sql_mysql_conn_t)));

    lh_sql_mysql_conn_set_api(self, api);
    lh_sql_mysql_conn_set_native(self, native);
    return self;
}

lh_void
lh_sql_mysql_conn_destroy(lh_sql_mysql_conn_t *self)
{
    lh_sql_mysql_api_close(lh_sql_mysql_conn_get_api(self), lh_sql_mysql_conn_get_native(self));
    lh_runtime_allocator_free(self);
}
