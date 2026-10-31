#include <lh/sql/mysql/prepared.h>
#include <lh/assert.h>
#include <lh/runtime/allocator.h>
#include <lh/sql/mysql/query.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_str_t *
lh_sql_mysql_prepared_get_sql_mut(lh_sql_mysql_prepared_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sql);
}

const lh_str_t *
lh_sql_mysql_prepared_get_sql(const lh_sql_mysql_prepared_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sql);
}

lh_array_t *
lh_sql_mysql_prepared_get_literals_mut(lh_sql_mysql_prepared_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->literals);
}

const lh_array_t *
lh_sql_mysql_prepared_get_literals(const lh_sql_mysql_prepared_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->literals);
}

lh_ptr
lh_sql_mysql_prepared_get_result(const lh_sql_mysql_prepared_t *self)
{
    lh_assert_runtime_ref(self);
    return self->result;
}

lh_void
lh_sql_mysql_prepared_set_result(lh_sql_mysql_prepared_t *self, lh_ptr result)
{
    lh_assert_runtime_ref(self);
    self->result = result;
}

lh_char_t **
lh_sql_mysql_prepared_get_row(const lh_sql_mysql_prepared_t *self)
{
    lh_assert_runtime_ref(self);
    return self->row;
}

lh_void
lh_sql_mysql_prepared_set_row(lh_sql_mysql_prepared_t *self, lh_char_t **row)
{
    lh_assert_runtime_ref(self);
    self->row = row;
}

lh_ulong_t *
lh_sql_mysql_prepared_get_lengths(const lh_sql_mysql_prepared_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lengths;
}

lh_void
lh_sql_mysql_prepared_set_lengths(lh_sql_mysql_prepared_t *self, lh_ulong_t *lengths)
{
    lh_assert_runtime_ref(self);
    self->lengths = lengths;
}

lh_bool_t
lh_sql_mysql_prepared_is_executed(const lh_sql_mysql_prepared_t *self)
{
    lh_assert_runtime_ref(self);
    return self->executed;
}

lh_void
lh_sql_mysql_prepared_set_executed(lh_sql_mysql_prepared_t *self, lh_bool_t executed)
{
    lh_assert_runtime_ref(self);
    self->executed = executed;
}

/* ── build ───────────────────────────────────────────────────────────────── */

lh_sql_mysql_prepared_t *
lh_sql_mysql_prepared_create(lh_str_view_t sql)
{
    lh_sql_mysql_prepared_t *self = lh_ptr_cast(
        lh_sql_mysql_prepared_t, lh_runtime_allocator_alloc(sizeof(lh_sql_mysql_prepared_t)));

    lh_str_init(lh_sql_mysql_prepared_get_sql_mut(self));
    lh_str_append_view(lh_sql_mysql_prepared_get_sql_mut(self), sql);
    lh_array_init(lh_sql_mysql_prepared_get_literals_mut(self), sizeof(lh_str_t));
    lh_sql_mysql_prepared_set_result(self, lh_null);
    lh_sql_mysql_prepared_set_row(self, lh_null);
    lh_sql_mysql_prepared_set_lengths(self, lh_null);
    lh_sql_mysql_prepared_set_executed(self, lh_bool_false);
    return self;
}

lh_str_t *
lh_sql_mysql_prepared_get_literal_mut(lh_sql_mysql_prepared_t *self, lh_usize_t index)
{
    lh_array_t *literals = lh_sql_mysql_prepared_get_literals_mut(self);

    if (lh_math_is_zero(index))
    {
        return lh_null;
    }
    /* Grown one initialized string at a time: resize would leave them raw. */
    while (lh_math_lt(lh_array_get_size(literals), index))
    {
        lh_str_t empty;

        lh_str_init(lh_addr_of(empty));
        lh_array_push_back(literals, lh_addr_of(empty));
    }
    return lh_ptr_cast(lh_str_t, lh_array_get_ptr(literals, lh_math_sub_one(index)));
}

lh_bool_t
lh_sql_mysql_prepared_build(const lh_sql_mysql_prepared_t *self, lh_str_t *out)
{
    const lh_array_t *literals = lh_sql_mysql_prepared_get_literals(self);
    lh_str_view_t sql;

    lh_str_as_view(lh_sql_mysql_prepared_get_sql(self), lh_addr_of(sql));
    return lh_sql_mysql_query_substitute(sql,
                                         lh_ptr_cast(const lh_str_t, lh_array_get_data(literals)),
                                         lh_array_get_size(literals), out);
}

lh_void
lh_sql_mysql_prepared_rewind(lh_sql_mysql_prepared_t *self, const lh_sql_mysql_api_t *api)
{
    if (lh_null_ne(lh_sql_mysql_prepared_get_result(self)))
    {
        lh_sql_mysql_api_free_result(api, lh_sql_mysql_prepared_get_result(self));
    }
    lh_sql_mysql_prepared_set_result(self, lh_null);
    lh_sql_mysql_prepared_set_row(self, lh_null);
    lh_sql_mysql_prepared_set_lengths(self, lh_null);
    lh_sql_mysql_prepared_set_executed(self, lh_bool_false);
}

lh_void
lh_sql_mysql_prepared_destroy(lh_sql_mysql_prepared_t *self, const lh_sql_mysql_api_t *api)
{
    lh_array_t *literals = lh_sql_mysql_prepared_get_literals_mut(self);

    lh_sql_mysql_prepared_rewind(self, api);
    for (lh_usize_t i = 0U; lh_math_lt(i, lh_array_get_size(literals)); i = lh_math_add_one(i))
    {
        lh_str_deinit(lh_ptr_cast(lh_str_t, lh_array_get_ptr(literals, i)));
    }
    lh_array_deinit(literals);
    lh_str_deinit(lh_sql_mysql_prepared_get_sql_mut(self));
    lh_runtime_allocator_free(self);
}
