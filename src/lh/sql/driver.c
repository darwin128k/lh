#include <lh/sql/driver.h>
#include <lh/assert.h>

lh_str_cptr
lh_sql_driver_get_name(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->name;
}

lh_sql_close_fn *
lh_sql_driver_get_close_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->close;
}

lh_void
lh_sql_driver_set_close_fn(lh_sql_driver_t *self, lh_sql_close_fn * close)
{
    lh_assert_runtime_ref(self);
    self->close = close;
}

lh_sql_exec_fn *
lh_sql_driver_get_exec_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->exec;
}

lh_void
lh_sql_driver_set_exec_fn(lh_sql_driver_t *self, lh_sql_exec_fn * exec)
{
    lh_assert_runtime_ref(self);
    self->exec = exec;
}

lh_sql_last_insert_id_fn *
lh_sql_driver_get_last_insert_id_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_insert_id;
}

lh_void
lh_sql_driver_set_last_insert_id_fn(lh_sql_driver_t *self, lh_sql_last_insert_id_fn * last_insert_id)
{
    lh_assert_runtime_ref(self);
    self->last_insert_id = last_insert_id;
}

lh_sql_prepare_fn *
lh_sql_driver_get_prepare_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->prepare;
}

lh_void
lh_sql_driver_set_prepare_fn(lh_sql_driver_t *self, lh_sql_prepare_fn * prepare)
{
    lh_assert_runtime_ref(self);
    self->prepare = prepare;
}

lh_sql_finalize_fn *
lh_sql_driver_get_finalize_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->finalize;
}

lh_void
lh_sql_driver_set_finalize_fn(lh_sql_driver_t *self, lh_sql_finalize_fn * finalize)
{
    lh_assert_runtime_ref(self);
    self->finalize = finalize;
}

lh_sql_bind_null_fn *
lh_sql_driver_get_bind_null_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bind_null;
}

lh_void
lh_sql_driver_set_bind_null_fn(lh_sql_driver_t *self, lh_sql_bind_null_fn * bind_null)
{
    lh_assert_runtime_ref(self);
    self->bind_null = bind_null;
}

lh_sql_bind_sint_fn *
lh_sql_driver_get_bind_sint_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bind_sint;
}

lh_void
lh_sql_driver_set_bind_sint_fn(lh_sql_driver_t *self, lh_sql_bind_sint_fn * bind_sint)
{
    lh_assert_runtime_ref(self);
    self->bind_sint = bind_sint;
}

lh_sql_bind_str_fn *
lh_sql_driver_get_bind_str_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bind_str;
}

lh_void
lh_sql_driver_set_bind_str_fn(lh_sql_driver_t *self, lh_sql_bind_str_fn * bind_str)
{
    lh_assert_runtime_ref(self);
    self->bind_str = bind_str;
}

lh_sql_step_fn *
lh_sql_driver_get_step_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

lh_void
lh_sql_driver_set_step_fn(lh_sql_driver_t *self, lh_sql_step_fn * step)
{
    lh_assert_runtime_ref(self);
    self->step = step;
}

lh_sql_reset_fn *
lh_sql_driver_get_reset_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->reset;
}

lh_void
lh_sql_driver_set_reset_fn(lh_sql_driver_t *self, lh_sql_reset_fn * reset)
{
    lh_assert_runtime_ref(self);
    self->reset = reset;
}

lh_sql_column_count_fn *
lh_sql_driver_get_column_count_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->column_count;
}

lh_void
lh_sql_driver_set_column_count_fn(lh_sql_driver_t *self, lh_sql_column_count_fn * column_count)
{
    lh_assert_runtime_ref(self);
    self->column_count = column_count;
}

lh_sql_column_is_null_fn *
lh_sql_driver_get_column_is_null_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->column_is_null;
}

lh_void
lh_sql_driver_set_column_is_null_fn(lh_sql_driver_t *self, lh_sql_column_is_null_fn * column_is_null)
{
    lh_assert_runtime_ref(self);
    self->column_is_null = column_is_null;
}

lh_sql_column_sint_fn *
lh_sql_driver_get_column_sint_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->column_sint;
}

lh_void
lh_sql_driver_set_column_sint_fn(lh_sql_driver_t *self, lh_sql_column_sint_fn * column_sint)
{
    lh_assert_runtime_ref(self);
    self->column_sint = column_sint;
}

lh_sql_column_str_fn *
lh_sql_driver_get_column_str_fn(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->column_str;
}

lh_void
lh_sql_driver_set_column_str_fn(lh_sql_driver_t *self, lh_sql_column_str_fn * column_str)
{
    lh_assert_runtime_ref(self);
    self->column_str = column_str;
}

lh_void
lh_sql_driver_close(const lh_sql_driver_t *self, struct lh_sql_db *db)
{
    lh_sql_driver_get_close_fn(self)(db);
}

lh_bool_t
lh_sql_driver_exec(const lh_sql_driver_t *self, struct lh_sql_db *db, lh_str_view_t sql)
{
    return lh_sql_driver_get_exec_fn(self)(db, sql);
}

lh_sllong_t
lh_sql_driver_get_last_insert_id(const lh_sql_driver_t *self, struct lh_sql_db *db)
{
    return lh_sql_driver_get_last_insert_id_fn(self)(db);
}

lh_bool_t
lh_sql_driver_prepare(const lh_sql_driver_t *self, struct lh_sql_db *db, struct lh_sql_stmt *stmt,
                      lh_str_view_t sql)
{
    return lh_sql_driver_get_prepare_fn(self)(db, stmt, sql);
}

lh_void
lh_sql_driver_finalize(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt)
{
    lh_sql_driver_get_finalize_fn(self)(stmt);
}

lh_bool_t
lh_sql_driver_bind_null(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt, lh_usize_t index)
{
    return lh_sql_driver_get_bind_null_fn(self)(stmt, index);
}

lh_bool_t
lh_sql_driver_bind_sint(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt, lh_usize_t index,
                        lh_sllong_t value)
{
    return lh_sql_driver_get_bind_sint_fn(self)(stmt, index, value);
}

lh_bool_t
lh_sql_driver_bind_str(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt, lh_usize_t index,
                       lh_str_view_t value)
{
    return lh_sql_driver_get_bind_str_fn(self)(stmt, index, value);
}

lh_sql_step_t
lh_sql_driver_step(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt)
{
    return lh_sql_driver_get_step_fn(self)(stmt);
}

lh_bool_t
lh_sql_driver_reset(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt)
{
    return lh_sql_driver_get_reset_fn(self)(stmt);
}

lh_usize_t
lh_sql_driver_get_column_count(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt)
{
    return lh_sql_driver_get_column_count_fn(self)(stmt);
}

lh_bool_t
lh_sql_driver_is_column_null(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt,
                             lh_usize_t index)
{
    return lh_sql_driver_get_column_is_null_fn(self)(stmt, index);
}

lh_sllong_t
lh_sql_driver_get_column_sint(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt,
                              lh_usize_t index)
{
    return lh_sql_driver_get_column_sint_fn(self)(stmt, index);
}

lh_str_view_t
lh_sql_driver_get_column_str(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt,
                             lh_usize_t index)
{
    return lh_sql_driver_get_column_str_fn(self)(stmt, index);
}
