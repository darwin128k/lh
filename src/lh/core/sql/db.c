#include <lh/core/sql/db.h>
#include <lh/core/assert.h>
#include <lh/core/sql/stmt.h>

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

lh_sql_error_kind_t
lh_sql_db_get_error_kind(const lh_sql_db_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error_kind;
}

lh_void
lh_sql_db_set_error_kind(lh_sql_db_t *self, lh_sql_error_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->error_kind = kind;
}

lh_sint_t
lh_sql_db_get_error_code(const lh_sql_db_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error_code;
}

lh_void
lh_sql_db_set_error_code(lh_sql_db_t *self, lh_sint_t code)
{
    lh_assert_runtime_ref(self);
    self->error_code = code;
}

lh_void
lh_sql_db_set_error_as(lh_sql_db_t *self, lh_sql_error_kind_t kind, lh_sint_t code,
                       lh_str_view_t message)
{
    lh_str_assign_view(lh_sql_db_get_error_mut(self), message);
    lh_sql_db_set_error_kind(self, kind);
    lh_sql_db_set_error_code(self, code);
}

lh_void
lh_sql_db_clear_error(lh_sql_db_t *self)
{
    lh_str_clear(lh_sql_db_get_error_mut(self));
    lh_sql_db_set_error_kind(self, lh_sql_error_kind_none);
    lh_sql_db_set_error_code(self, 0);
}

lh_bool_t
lh_sql_db_is_open(const lh_sql_db_t *self)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(lh_sql_db_get_driver(self)) &&
                                         lh_null_ne(lh_sql_db_get_handle(self)));
}

lh_void
lh_sql_db_init(lh_sql_db_t *self)
{
    lh_sql_db_set_driver(self, lh_null);
    lh_sql_db_set_handle(self, lh_null);
    lh_str_init(lh_sql_db_get_error_mut(self));
    lh_sql_db_set_error_kind(self, lh_sql_error_kind_none);
    lh_sql_db_set_error_code(self, 0);
}

lh_void
lh_sql_db_set_error(lh_sql_db_t *self, lh_str_view_t message)
{
    lh_sql_db_set_error_as(self, lh_sql_error_kind_other, 0, message);
}

lh_bool_t
lh_sql_db_fail_as(lh_sql_db_t *self, lh_sql_error_kind_t kind, lh_sint_t code,
                  lh_str_view_t message)
{
    lh_sql_db_set_error_as(self, kind, code, message);
    return lh_bool_false;
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
lh_sql_db_fail(lh_sql_db_t *self, lh_str_view_t message)
{
    return lh_sql_db_fail_as(self, lh_sql_error_kind_other, 0, message);
}

lh_void
lh_sql_db_deinit(lh_sql_db_t *self)
{
    lh_sql_db_close(self);
    lh_str_deinit(lh_sql_db_get_error_mut(self));
}

lh_bool_t
lh_sql_db_exec(lh_sql_db_t *self, lh_str_view_t sql)
{
    if (!lh_sql_db_is_open(self))
    {
        return lh_sql_db_fail_as(self, lh_sql_error_kind_usage, 0,
                                 lh_str_view_lit("database is not open"));
    }
    lh_sql_db_clear_error(self);
    return lh_sql_driver_exec(lh_sql_db_get_driver(self), self, sql);
}

lh_bool_t
lh_sql_db_prepare(lh_sql_db_t *self, struct lh_sql_stmt *stmt, lh_str_view_t sql)
{
    lh_sql_stmt_init(stmt, self);
    if (!lh_sql_db_is_open(self))
    {
        return lh_sql_db_fail_as(self, lh_sql_error_kind_usage, 0,
                                 lh_str_view_lit("database is not open"));
    }
    lh_sql_db_clear_error(self);
    return lh_sql_driver_prepare(lh_sql_db_get_driver(self), self, stmt, sql);
}
