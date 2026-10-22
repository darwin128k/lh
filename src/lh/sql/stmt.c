#include <lh/sql/stmt.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_sql_db_t *
lh_sql_stmt_get_db_mut(lh_sql_stmt_t *self)
{
    lh_assert_runtime_ref(self);
    return self->db;
}

const lh_sql_db_t *
lh_sql_stmt_get_db(const lh_sql_stmt_t *self)
{
    lh_assert_runtime_ref(self);
    return self->db;
}

lh_void
lh_sql_stmt_set_db(lh_sql_stmt_t *self, lh_sql_db_t *db)
{
    lh_assert_runtime_ref(self);
    self->db = db;
}

lh_ptr
lh_sql_stmt_get_handle(const lh_sql_stmt_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_sql_stmt_set_handle(lh_sql_stmt_t *self, lh_ptr handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

const lh_sql_driver_t *
lh_sql_stmt_get_driver(const lh_sql_stmt_t *self)
{
    return lh_sql_db_get_driver(lh_sql_stmt_get_db(self));
}

lh_bool_t
lh_sql_stmt_is_prepared(const lh_sql_stmt_t *self)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(lh_sql_stmt_get_handle(self)) &&
                                         lh_null_ne(lh_sql_stmt_get_db(self)) &&
                                         lh_sql_db_is_open(lh_sql_stmt_get_db(self)));
}

/* ── init / finalize ─────────────────────────────────────────────────────── */

lh_void
lh_sql_stmt_init(lh_sql_stmt_t *self, lh_sql_db_t *db)
{
    lh_sql_stmt_set_db(self, db);
    lh_sql_stmt_set_handle(self, lh_null);
}

lh_void
lh_sql_stmt_finalize(lh_sql_stmt_t *self)
{
    if (lh_sql_stmt_is_prepared(self))
    {
        lh_sql_driver_finalize(lh_sql_stmt_get_driver(self), self);
    }
    lh_sql_stmt_set_handle(self, lh_null);
}

/* ── bind ────────────────────────────────────────────────────────────────── */

lh_bool_t
lh_sql_stmt_bind_null(lh_sql_stmt_t *self, lh_usize_t index)
{
    if (!lh_sql_stmt_is_prepared(self))
    {
        return lh_bool_false;
    }
    return lh_sql_driver_bind_null(lh_sql_stmt_get_driver(self), self, index);
}

lh_bool_t
lh_sql_stmt_bind_sint(lh_sql_stmt_t *self, lh_usize_t index, lh_sllong_t value)
{
    if (!lh_sql_stmt_is_prepared(self))
    {
        return lh_bool_false;
    }
    return lh_sql_driver_bind_sint(lh_sql_stmt_get_driver(self), self, index, value);
}

lh_bool_t
lh_sql_stmt_bind_str(lh_sql_stmt_t *self, lh_usize_t index, lh_str_view_t value)
{
    if (!lh_sql_stmt_is_prepared(self))
    {
        return lh_bool_false;
    }
    return lh_sql_driver_bind_str(lh_sql_stmt_get_driver(self), self, index, value);
}

/* ── run ─────────────────────────────────────────────────────────────────── */

lh_sql_step_t
lh_sql_stmt_step(lh_sql_stmt_t *self)
{
    if (!lh_sql_stmt_is_prepared(self))
    {
        if (lh_null_ne(lh_sql_stmt_get_db(self)))
        {
            lh_sql_db_set_error(lh_sql_stmt_get_db_mut(self),
                                lh_str_view_lit("statement is not prepared"));
        }
        return lh_sql_step_error;
    }
    return lh_sql_driver_step(lh_sql_stmt_get_driver(self), self);
}

lh_bool_t
lh_sql_stmt_reset(lh_sql_stmt_t *self)
{
    if (!lh_sql_stmt_is_prepared(self))
    {
        return lh_bool_false;
    }
    return lh_sql_driver_reset(lh_sql_stmt_get_driver(self), self);
}

/* ── read the current row ────────────────────────────────────────────────── */

lh_usize_t
lh_sql_stmt_get_column_count(const lh_sql_stmt_t *self)
{
    if (!lh_sql_stmt_is_prepared(self))
    {
        return 0U;
    }
    return lh_sql_driver_get_column_count(lh_sql_stmt_get_driver(self), self);
}

lh_bool_t
lh_sql_stmt_is_column_null(const lh_sql_stmt_t *self, lh_usize_t index)
{
    if (!lh_sql_stmt_is_prepared(self))
    {
        return lh_bool_true;
    }
    return lh_sql_driver_is_column_null(lh_sql_stmt_get_driver(self), self, index);
}

lh_sllong_t
lh_sql_stmt_get_column_sint(const lh_sql_stmt_t *self, lh_usize_t index)
{
    if (!lh_sql_stmt_is_prepared(self))
    {
        return 0;
    }
    return lh_sql_driver_get_column_sint(lh_sql_stmt_get_driver(self), self, index);
}

lh_str_view_t
lh_sql_stmt_get_column_str(const lh_sql_stmt_t *self, lh_usize_t index)
{
    if (!lh_sql_stmt_is_prepared(self))
    {
        return lh_str_view_empty();
    }
    return lh_sql_driver_get_column_str(lh_sql_stmt_get_driver(self), self, index);
}
