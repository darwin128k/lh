#include <lh/sql/mysql/api.h>
#include <lh/assert.h>
#include <lh/os/system/fs/path.h>
#include <lh/runtime/allocator.h>

/* What the client is called, most likely first: MariaDB's Connector/C, then
   Oracle's libmysqlclient, newest soname first. */
static const lh_char_t *const m_sql_mysql_api_names[] = {
#if defined(_WIN32)
    "libmariadb.dll",
    "libmysql.dll",
    "libmysqlclient.dll",
#else
    "libmariadb.so.3",      "libmariadb.so",        "libmysqlclient.so.21",
    "libmysqlclient.so.20", "libmysqlclient.so.18", "libmysqlclient.so",
#endif
};

static lh_sql_mysql_api_t m_sql_mysql_api_default;
static lh_bool_t m_sql_mysql_api_default_tried;
/* ── load ────────────────────────────────────────────────────────────────── */

lh_os_shared_t *
lh_sql_mysql_api_get_image_mut(lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->image);
}

lh_sql_mysql_init_fn *
lh_sql_mysql_api_get_init(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->init;
}

lh_void
lh_sql_mysql_api_set_init(lh_sql_mysql_api_t *self, lh_sql_mysql_init_fn *init)
{
    lh_assert_runtime_ref(self);
    self->init = init;
}

lh_sql_mysql_real_connect_fn *
lh_sql_mysql_api_get_real_connect(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->real_connect;
}

lh_void
lh_sql_mysql_api_set_real_connect(lh_sql_mysql_api_t *self, lh_sql_mysql_real_connect_fn *real_connect)
{
    lh_assert_runtime_ref(self);
    self->real_connect = real_connect;
}

lh_sql_mysql_close_fn *
lh_sql_mysql_api_get_close(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->close;
}

lh_void
lh_sql_mysql_api_set_close(lh_sql_mysql_api_t *self, lh_sql_mysql_close_fn *close)
{
    lh_assert_runtime_ref(self);
    self->close = close;
}

lh_sql_mysql_error_fn *
lh_sql_mysql_api_get_error(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error;
}

lh_void
lh_sql_mysql_api_set_error(lh_sql_mysql_api_t *self, lh_sql_mysql_error_fn *error)
{
    lh_assert_runtime_ref(self);
    self->error = error;
}

lh_sql_mysql_error_code_fn *
lh_sql_mysql_api_get_error_code(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error_code;
}

lh_void
lh_sql_mysql_api_set_error_code(lh_sql_mysql_api_t *self, lh_sql_mysql_error_code_fn *error_code)
{
    lh_assert_runtime_ref(self);
    self->error_code = error_code;
}

lh_sql_mysql_real_query_fn *
lh_sql_mysql_api_get_real_query(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->real_query;
}

lh_void
lh_sql_mysql_api_set_real_query(lh_sql_mysql_api_t *self, lh_sql_mysql_real_query_fn *real_query)
{
    lh_assert_runtime_ref(self);
    self->real_query = real_query;
}

lh_sql_mysql_store_result_fn *
lh_sql_mysql_api_get_store_result(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->store_result;
}

lh_void
lh_sql_mysql_api_set_store_result(lh_sql_mysql_api_t *self, lh_sql_mysql_store_result_fn *store_result)
{
    lh_assert_runtime_ref(self);
    self->store_result = store_result;
}

lh_sql_mysql_free_result_fn *
lh_sql_mysql_api_get_free_result(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->free_result;
}

lh_void
lh_sql_mysql_api_set_free_result(lh_sql_mysql_api_t *self, lh_sql_mysql_free_result_fn *free_result)
{
    lh_assert_runtime_ref(self);
    self->free_result = free_result;
}

lh_sql_mysql_fetch_row_fn *
lh_sql_mysql_api_get_fetch_row(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fetch_row;
}

lh_void
lh_sql_mysql_api_set_fetch_row(lh_sql_mysql_api_t *self, lh_sql_mysql_fetch_row_fn *fetch_row)
{
    lh_assert_runtime_ref(self);
    self->fetch_row = fetch_row;
}

lh_sql_mysql_fetch_lengths_fn *
lh_sql_mysql_api_get_fetch_lengths(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fetch_lengths;
}

lh_void
lh_sql_mysql_api_set_fetch_lengths(lh_sql_mysql_api_t *self, lh_sql_mysql_fetch_lengths_fn *fetch_lengths)
{
    lh_assert_runtime_ref(self);
    self->fetch_lengths = fetch_lengths;
}

lh_sql_mysql_num_fields_fn *
lh_sql_mysql_api_get_num_fields(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->num_fields;
}

lh_void
lh_sql_mysql_api_set_num_fields(lh_sql_mysql_api_t *self, lh_sql_mysql_num_fields_fn *num_fields)
{
    lh_assert_runtime_ref(self);
    self->num_fields = num_fields;
}

lh_sql_mysql_field_count_fn *
lh_sql_mysql_api_get_field_count(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->field_count;
}

lh_void
lh_sql_mysql_api_set_field_count(lh_sql_mysql_api_t *self, lh_sql_mysql_field_count_fn *field_count)
{
    lh_assert_runtime_ref(self);
    self->field_count = field_count;
}

lh_sql_mysql_insert_id_fn *
lh_sql_mysql_api_get_insert_id(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->insert_id;
}

lh_void
lh_sql_mysql_api_set_insert_id(lh_sql_mysql_api_t *self, lh_sql_mysql_insert_id_fn *insert_id)
{
    lh_assert_runtime_ref(self);
    self->insert_id = insert_id;
}

lh_sql_mysql_real_escape_string_fn *
lh_sql_mysql_api_get_real_escape_string(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->real_escape_string;
}

lh_void
lh_sql_mysql_api_set_real_escape_string(lh_sql_mysql_api_t *self, lh_sql_mysql_real_escape_string_fn *real_escape_string)
{
    lh_assert_runtime_ref(self);
    self->real_escape_string = real_escape_string;
}

lh_sql_mysql_set_character_set_fn *
lh_sql_mysql_api_get_set_character_set(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->set_character_set;
}

lh_void
lh_sql_mysql_api_set_set_character_set(lh_sql_mysql_api_t *self, lh_sql_mysql_set_character_set_fn *set_character_set)
{
    lh_assert_runtime_ref(self);
    self->set_character_set = set_character_set;
}

lh_sql_mysql_options_fn *
lh_sql_mysql_api_get_options(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->options;
}

lh_void
lh_sql_mysql_api_set_options(lh_sql_mysql_api_t *self, lh_sql_mysql_options_fn *options)
{
    lh_assert_runtime_ref(self);
    self->options = options;
}

lh_sql_mysql_thread_end_fn *
lh_sql_mysql_api_get_thread_end(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thread_end;
}

lh_void
lh_sql_mysql_api_set_thread_end(lh_sql_mysql_api_t *self, lh_sql_mysql_thread_end_fn *thread_end)
{
    lh_assert_runtime_ref(self);
    self->thread_end = thread_end;
}

lh_void
lh_sql_mysql_api_init(lh_sql_mysql_api_t *self)
{
    lh_sql_mysql_api_set_options(self, lh_null);
    lh_sql_mysql_api_set_thread_end(self, lh_null);
    lh_os_shared_init(lh_sql_mysql_api_get_image_mut(self));
    lh_sql_mysql_api_set_init(self, lh_null);
    lh_sql_mysql_api_set_real_connect(self, lh_null);
    lh_sql_mysql_api_set_close(self, lh_null);
    lh_sql_mysql_api_set_error(self, lh_null);
    lh_sql_mysql_api_set_error_code(self, lh_null);
    lh_sql_mysql_api_set_real_query(self, lh_null);
    lh_sql_mysql_api_set_store_result(self, lh_null);
    lh_sql_mysql_api_set_free_result(self, lh_null);
    lh_sql_mysql_api_set_fetch_row(self, lh_null);
    lh_sql_mysql_api_set_fetch_lengths(self, lh_null);
    lh_sql_mysql_api_set_num_fields(self, lh_null);
    lh_sql_mysql_api_set_field_count(self, lh_null);
    lh_sql_mysql_api_set_insert_id(self, lh_null);
    lh_sql_mysql_api_set_real_escape_string(self, lh_null);
    lh_sql_mysql_api_set_set_character_set(self, lh_null);
}

lh_bool_t
lh_sql_mysql_api_is_loaded(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_bool_t,
                          lh_null_ne(lh_sql_mysql_api_get_init(self)) && lh_null_ne(lh_sql_mysql_api_get_real_connect(self)) &&
                              lh_null_ne(lh_sql_mysql_api_get_close(self)) && lh_null_ne(lh_sql_mysql_api_get_error(self)) &&
                              lh_null_ne(lh_sql_mysql_api_get_error_code(self)) && lh_null_ne(lh_sql_mysql_api_get_real_query(self)) &&
                              lh_null_ne(lh_sql_mysql_api_get_store_result(self)) && lh_null_ne(lh_sql_mysql_api_get_free_result(self)) &&
                              lh_null_ne(lh_sql_mysql_api_get_fetch_row(self)) && lh_null_ne(lh_sql_mysql_api_get_fetch_lengths(self)) &&
                              lh_null_ne(lh_sql_mysql_api_get_num_fields(self)) && lh_null_ne(lh_sql_mysql_api_get_field_count(self)) &&
                              lh_null_ne(lh_sql_mysql_api_get_insert_id(self)) && lh_null_ne(lh_sql_mysql_api_get_real_escape_string(self)) &&
                              lh_null_ne(lh_sql_mysql_api_get_set_character_set(self)));
}

/* ── calls ───────────────────────────────────────────────────────────────── */

lh_ptr
lh_sql_mysql_api_init_native(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_sql_mysql_api_get_init(self)(lh_null);
}

lh_ptr
lh_sql_mysql_api_real_connect(const lh_sql_mysql_api_t *self, lh_ptr mysql, const lh_char_t *host,
                              const lh_char_t *user, const lh_char_t *password, const lh_char_t *db,
                              lh_uint_t port)
{
    lh_assert_runtime_ref(self);
    return lh_sql_mysql_api_get_real_connect(self)(mysql, host, user, password, db, port, lh_null, 0UL);
}

lh_void
lh_sql_mysql_api_close(const lh_sql_mysql_api_t *self, lh_ptr mysql)
{
    lh_assert_runtime_ref(self);
    lh_sql_mysql_api_get_close(self)(mysql);
}

lh_str_view_t
lh_sql_mysql_api_error(const lh_sql_mysql_api_t *self, lh_ptr mysql)
{
    const lh_char_t *text;
    lh_str_view_t view;

    lh_assert_runtime_ref(self);
    text = lh_sql_mysql_api_get_error(self)(mysql);
    if (lh_null_eq(text))
    {
        return lh_str_view_empty();
    }
    lh_str_view_init(lh_addr_of(view), text);
    return view;
}

lh_uint_t
lh_sql_mysql_api_error_code(const lh_sql_mysql_api_t *self, lh_ptr mysql)
{
    lh_assert_runtime_ref(self);
    return lh_sql_mysql_api_get_error_code(self)(mysql);
}

lh_bool_t
lh_sql_mysql_api_real_query(const lh_sql_mysql_api_t *self, lh_ptr mysql, lh_str_view_t query)
{
    lh_assert_runtime_ref(self);
    /* A view may have no data when empty; the client wants a pointer anyway. */
    return lh_cast_static(
        lh_bool_t,
        lh_math_is_zero(lh_sql_mysql_api_get_real_query(self)(
            mysql,
            lh_str_view_is_empty(lh_addr_of(query)) ? "" : lh_str_view_get_begin(lh_addr_of(query)),
            lh_cast_static(lh_ulong_t, lh_str_view_get_size(lh_addr_of(query))))));
}

lh_ptr
lh_sql_mysql_api_store_result(const lh_sql_mysql_api_t *self, lh_ptr mysql)
{
    lh_assert_runtime_ref(self);
    return lh_sql_mysql_api_get_store_result(self)(mysql);
}

lh_void
lh_sql_mysql_api_free_result(const lh_sql_mysql_api_t *self, lh_ptr result)
{
    lh_assert_runtime_ref(self);
    lh_sql_mysql_api_get_free_result(self)(result);
}

lh_char_t **
lh_sql_mysql_api_fetch_row(const lh_sql_mysql_api_t *self, lh_ptr result)
{
    lh_assert_runtime_ref(self);
    return lh_sql_mysql_api_get_fetch_row(self)(result);
}

lh_ulong_t *
lh_sql_mysql_api_fetch_lengths(const lh_sql_mysql_api_t *self, lh_ptr result)
{
    lh_assert_runtime_ref(self);
    return lh_sql_mysql_api_get_fetch_lengths(self)(result);
}

lh_usize_t
lh_sql_mysql_api_num_fields(const lh_sql_mysql_api_t *self, lh_ptr result)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_usize_t, lh_sql_mysql_api_get_num_fields(self)(result));
}

lh_usize_t
lh_sql_mysql_api_field_count(const lh_sql_mysql_api_t *self, lh_ptr mysql)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_usize_t, lh_sql_mysql_api_get_field_count(self)(mysql));
}

lh_ullong_t
lh_sql_mysql_api_insert_id(const lh_sql_mysql_api_t *self, lh_ptr mysql)
{
    lh_assert_runtime_ref(self);
    return lh_sql_mysql_api_get_insert_id(self)(mysql);
}

lh_void
lh_sql_mysql_api_escape(const lh_sql_mysql_api_t *self, lh_ptr mysql, lh_str_view_t text,
                        lh_str_t *out)
{
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(text));
    /* The client's contract: room for every byte escaped, plus its NUL. */
    const lh_usize_t room = lh_math_add_one(lh_math_mul(size, 2U));
    lh_char_t *buffer = lh_ptr_cast(lh_char_t, lh_runtime_allocator_alloc(room));
    lh_ulong_t written;

    lh_assert_runtime_ref(self);
    written = lh_sql_mysql_api_get_real_escape_string(self)(
        mysql, buffer, lh_math_is_zero(size) ? "" : lh_str_view_get_begin(lh_addr_of(text)),
        lh_cast_static(lh_ulong_t, size));
    lh_str_append(out, buffer, lh_cast_static(lh_usize_t, written));
    lh_runtime_allocator_free(buffer);
}

lh_bool_t
lh_sql_mysql_api_set_character_set(const lh_sql_mysql_api_t *self, lh_ptr mysql,
                                   const lh_char_t *name)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_bool_t, lh_math_is_zero(lh_sql_mysql_api_get_set_character_set(self)(mysql, name)));
}

lh_void
lh_sql_mysql_api_thread_end(const lh_sql_mysql_api_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_null_ne(lh_sql_mysql_api_get_thread_end(self)))
    {
        lh_sql_mysql_api_get_thread_end(self)();
    }
}

lh_void
lh_sql_mysql_api_deinit(lh_sql_mysql_api_t *self)
{
    lh_os_shared_deinit(lh_sql_mysql_api_get_image_mut(self));
    lh_sql_mysql_api_init(self);
}

lh_bool_t
lh_sql_mysql_api_bind(lh_sql_mysql_api_t *self)
{
    const lh_os_shared_t *image = lh_sql_mysql_api_get_image_mut(self);
    lh_sql_mysql_api_set_options(self, lh_cast_static(lh_sql_mysql_options_fn *, lh_os_shared_get_fn(image, "mysql_options")));

    lh_sql_mysql_api_set_thread_end(self, lh_cast_static(lh_sql_mysql_thread_end_fn *, lh_os_shared_get_fn(image, "mysql_thread_end")));

    lh_sql_mysql_api_set_init(self, lh_cast_static(lh_sql_mysql_init_fn *, lh_os_shared_get_fn(image, "mysql_init")));
    lh_sql_mysql_api_set_real_connect(self, lh_cast_static(lh_sql_mysql_real_connect_fn *, lh_os_shared_get_fn(image, "mysql_real_connect")));
    lh_sql_mysql_api_set_close(self, lh_cast_static(lh_sql_mysql_close_fn *, lh_os_shared_get_fn(image, "mysql_close")));
    lh_sql_mysql_api_set_error(self, lh_cast_static(lh_sql_mysql_error_fn *, lh_os_shared_get_fn(image, "mysql_error")));
    lh_sql_mysql_api_set_error_code(self, lh_cast_static(lh_sql_mysql_error_code_fn *, lh_os_shared_get_fn(image, "mysql_errno")));
    lh_sql_mysql_api_set_real_query(self, lh_cast_static(lh_sql_mysql_real_query_fn *, lh_os_shared_get_fn(image, "mysql_real_query")));
    lh_sql_mysql_api_set_store_result(self, lh_cast_static(lh_sql_mysql_store_result_fn *, lh_os_shared_get_fn(image, "mysql_store_result")));
    lh_sql_mysql_api_set_free_result(self, lh_cast_static(lh_sql_mysql_free_result_fn *, lh_os_shared_get_fn(image, "mysql_free_result")));
    lh_sql_mysql_api_set_fetch_row(self, lh_cast_static(lh_sql_mysql_fetch_row_fn *, lh_os_shared_get_fn(image, "mysql_fetch_row")));
    lh_sql_mysql_api_set_fetch_lengths(self, lh_cast_static(lh_sql_mysql_fetch_lengths_fn *, lh_os_shared_get_fn(image, "mysql_fetch_lengths")));
    lh_sql_mysql_api_set_num_fields(self, lh_cast_static(lh_sql_mysql_num_fields_fn *, lh_os_shared_get_fn(image, "mysql_num_fields")));
    lh_sql_mysql_api_set_field_count(self, lh_cast_static(lh_sql_mysql_field_count_fn *, lh_os_shared_get_fn(image, "mysql_field_count")));
    lh_sql_mysql_api_set_insert_id(self, lh_cast_static(lh_sql_mysql_insert_id_fn *, lh_os_shared_get_fn(image, "mysql_insert_id")));
    lh_sql_mysql_api_set_real_escape_string(self, lh_cast_static(lh_sql_mysql_real_escape_string_fn *, lh_os_shared_get_fn(image, "mysql_real_escape_string")));
    lh_sql_mysql_api_set_set_character_set(self, lh_cast_static(lh_sql_mysql_set_character_set_fn *, lh_os_shared_get_fn(image, "mysql_set_character_set")));

    if (!lh_sql_mysql_api_is_loaded(self))
    {
        lh_sql_mysql_api_deinit(self);
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_sql_mysql_api_load(lh_sql_mysql_api_t *self, lh_str_view_t name)
{
    lh_fs_path_t path;
    lh_bool_t opened;

    lh_sql_mysql_api_deinit(self);
    lh_fs_path_init(lh_addr_of(path));
    lh_fs_path_set(lh_addr_of(path), name, lh_os_system_fs_path_style_native());
    opened = lh_os_shared_open(lh_sql_mysql_api_get_image_mut(self), lh_addr_of(path));
    lh_fs_path_deinit(lh_addr_of(path));
    return lh_cast_static(lh_bool_t, opened && lh_sql_mysql_api_bind(self));
}

lh_bool_t
lh_sql_mysql_api_load_default(lh_sql_mysql_api_t *self)
{
    for (lh_usize_t i = 0U; lh_math_lt(i, lh_array_ptr_get_size(m_sql_mysql_api_names));
         i = lh_math_add_one(i))
    {
        lh_str_view_t name;

        lh_str_view_init(lh_addr_of(name), m_sql_mysql_api_names[i]);
        if (lh_sql_mysql_api_load(self, name))
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

const lh_sql_mysql_api_t *
lh_sql_mysql_api_get_default(lh_void)
{
    if (!m_sql_mysql_api_default_tried)
    {
        m_sql_mysql_api_default_tried = lh_bool_true;
        lh_sql_mysql_api_init(lh_addr_of(m_sql_mysql_api_default));
        lh_sql_mysql_api_load_default(lh_addr_of(m_sql_mysql_api_default));
    }
    return lh_sql_mysql_api_is_loaded(lh_addr_of(m_sql_mysql_api_default))
               ? lh_addr_of(m_sql_mysql_api_default)
               : lh_null;
}
