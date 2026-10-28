#include <lh/sql/db.h>
#include <lh/assert.h>
#include <lh/sql/stmt.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

const lh_sql_driver_t *
lh_sql_db_get_driver(const lh_sql_db_t *self)
{
    lh_assert_runtime_ref(self);
    return self->driver;
}

lh_void
lh_sql_db_set_driver(lh_sql_db_t *self, const lh_sql_driver_t *driver)
{
    lh_assert_runtime_ref(self);
    self->driver = driver;
}

lh_ptr
lh_sql_db_get_handle(const lh_sql_db_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_sql_db_set_handle(lh_sql_db_t *self, lh_ptr handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

lh_str_t *
lh_sql_db_get_error_mut(lh_sql_db_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->error);
}

lh_str_view_t
lh_sql_db_get_error(const lh_sql_db_t *self)
{
    lh_str_view_t view;

    lh_assert_runtime_ref(self);
    lh_str_as_view(lh_addr_of(self->error), lh_addr_of(view));
    return view;
}

lh_void
lh_sql_db_set_error(lh_sql_db_t *self, lh_str_view_t message)
{
    lh_str_assign_view(lh_sql_db_get_error_mut(self), message);
}

lh_void
lh_sql_db_clear_error(lh_sql_db_t *self)
{
    lh_str_clear(lh_sql_db_get_error_mut(self));
}

lh_bool_t
lh_sql_db_fail(lh_sql_db_t *self, lh_str_view_t message)
{
    lh_sql_db_set_error(self, message);
    return lh_bool_false;
}

lh_bool_t
lh_sql_db_is_open(const lh_sql_db_t *self)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(lh_sql_db_get_driver(self)) &&
                                         lh_null_ne(lh_sql_db_get_handle(self)));
}

/* ── init / deinit ───────────────────────────────────────────────────────── */

lh_void
lh_sql_db_init(lh_sql_db_t *self)
{
    lh_sql_db_set_driver(self, lh_null);
    lh_sql_db_set_handle(self, lh_null);
    lh_str_init(lh_sql_db_get_error_mut(self));
}

lh_void
lh_sql_db_close(lh_sql_db_t *self)
{
    if (lh_sql_db_is_open(self))
    {
        lh_sql_driver_close(lh_sql_db_get_driver(self), self);
    }
    lh_sql_db_set_driver(self, lh_null);
    lh_sql_db_set_handle(self, lh_null);
}

lh_void
lh_sql_db_deinit(lh_sql_db_t *self)
{
    lh_sql_db_close(self);
    lh_str_deinit(lh_sql_db_get_error_mut(self));
}

/* ── operations ──────────────────────────────────────────────────────────── */

lh_bool_t
lh_sql_db_exec(lh_sql_db_t *self, lh_str_view_t sql)
{
    if (!lh_sql_db_is_open(self))
    {
        return lh_sql_db_fail(self, lh_str_view_lit("database is not open"));
    }
    lh_sql_db_clear_error(self);
    return lh_sql_driver_exec(lh_sql_db_get_driver(self), self, sql);
}

lh_sllong_t
lh_sql_db_get_last_insert_id(lh_sql_db_t *self)
{
    if (!lh_sql_db_is_open(self))
    {
        return 0;
    }
    return lh_sql_driver_get_last_insert_id(lh_sql_db_get_driver(self), self);
}

lh_bool_t
lh_sql_db_prepare(lh_sql_db_t *self, struct lh_sql_stmt *stmt, lh_str_view_t sql)
{
    lh_sql_stmt_init(stmt, self);
    if (!lh_sql_db_is_open(self))
    {
        return lh_sql_db_fail(self, lh_str_view_lit("database is not open"));
    }
    lh_sql_db_clear_error(self);
    return lh_sql_driver_prepare(lh_sql_db_get_driver(self), self, stmt, sql);
}
