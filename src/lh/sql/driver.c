#include <lh/sql/driver.h>
#include <lh/assert.h>

lh_str_cptr
lh_sql_driver_get_name(const lh_sql_driver_t *self)
{
    lh_assert_runtime_ref(self);
    return self->name;
}

lh_void
lh_sql_driver_close(const lh_sql_driver_t *self, struct lh_sql_db *db)
{
    lh_assert_runtime_ref(self);
    self->close(db);
}

lh_bool_t
lh_sql_driver_exec(const lh_sql_driver_t *self, struct lh_sql_db *db, lh_str_view_t sql)
{
    lh_assert_runtime_ref(self);
    return self->exec(db, sql);
}

lh_sllong_t
lh_sql_driver_get_last_insert_id(const lh_sql_driver_t *self, struct lh_sql_db *db)
{
    lh_assert_runtime_ref(self);
    return self->last_insert_id(db);
}

lh_bool_t
lh_sql_driver_prepare(const lh_sql_driver_t *self, struct lh_sql_db *db, struct lh_sql_stmt *stmt,
                      lh_str_view_t sql)
{
    lh_assert_runtime_ref(self);
    return self->prepare(db, stmt, sql);
}

lh_void
lh_sql_driver_finalize(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt)
{
    lh_assert_runtime_ref(self);
    self->finalize(stmt);
}

lh_bool_t
lh_sql_driver_bind_null(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt, lh_usize_t index)
{
    lh_assert_runtime_ref(self);
    return self->bind_null(stmt, index);
}

lh_bool_t
lh_sql_driver_bind_sint(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt, lh_usize_t index,
                        lh_sllong_t value)
{
    lh_assert_runtime_ref(self);
    return self->bind_sint(stmt, index, value);
}

lh_bool_t
lh_sql_driver_bind_str(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt, lh_usize_t index,
                       lh_str_view_t value)
{
    lh_assert_runtime_ref(self);
    return self->bind_str(stmt, index, value);
}

lh_sql_step_t
lh_sql_driver_step(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt)
{
    lh_assert_runtime_ref(self);
    return self->step(stmt);
}

lh_bool_t
lh_sql_driver_reset(const lh_sql_driver_t *self, struct lh_sql_stmt *stmt)
{
    lh_assert_runtime_ref(self);
    return self->reset(stmt);
}

lh_usize_t
lh_sql_driver_get_column_count(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt)
{
    lh_assert_runtime_ref(self);
    return self->column_count(stmt);
}

lh_bool_t
lh_sql_driver_is_column_null(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt,
                             lh_usize_t index)
{
    lh_assert_runtime_ref(self);
    return self->column_is_null(stmt, index);
}

lh_sllong_t
lh_sql_driver_get_column_sint(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt,
                              lh_usize_t index)
{
    lh_assert_runtime_ref(self);
    return self->column_sint(stmt, index);
}

lh_str_view_t
lh_sql_driver_get_column_str(const lh_sql_driver_t *self, const struct lh_sql_stmt *stmt,
                             lh_usize_t index)
{
    lh_assert_runtime_ref(self);
    return self->column_str(stmt, index);
}
