#include <lh/sql/mysql.h>
#include <lh/assert.h>
#include <lh/char/map.h>
#include <lh/str/format/sllong.h>
#include <lh/str/parse/sllong.h>

/* ── state behind the handles ────────────────────────────────────────────── */

lh_sql_mysql_conn_t *
lh_sql_mysql_get_conn(const lh_sql_db_t *db)
{
    return lh_ptr_cast(lh_sql_mysql_conn_t, lh_sql_db_get_handle(db));
}

const lh_sql_mysql_api_t *
lh_sql_mysql_get_api(const lh_sql_db_t *db)
{
    return lh_sql_mysql_conn_get_api(lh_sql_mysql_get_conn(db));
}

lh_ptr
lh_sql_mysql_get_native(const lh_sql_db_t *db)
{
    return lh_sql_mysql_conn_get_native(lh_sql_mysql_get_conn(db));
}

lh_sql_mysql_prepared_t *
lh_sql_mysql_get_prepared(const lh_sql_stmt_t *stmt)
{
    return lh_ptr_cast(lh_sql_mysql_prepared_t, lh_sql_stmt_get_handle(stmt));
}

lh_bool_t
lh_sql_mysql_fail(lh_sql_db_t *db)
{
    const lh_str_view_t message =
        lh_sql_mysql_api_error(lh_sql_mysql_get_api(db), lh_sql_mysql_get_native(db));

    return lh_sql_db_fail(
        db, lh_str_view_is_empty(lh_addr_of(message)) ? lh_str_view_lit("MySQL error") : message);
}

lh_void
lh_sql_mysql_quote(const lh_sql_db_t *db, lh_str_view_t text, lh_str_t *out)
{
    lh_str_clear(out);
    lh_str_push_back(out, lh_char_map_squote);
    lh_sql_mysql_api_escape(lh_sql_mysql_get_api(db), lh_sql_mysql_get_native(db), text, out);
    lh_str_push_back(out, lh_char_map_squote);
}

lh_bool_t
lh_sql_mysql_reconnect(lh_sql_db_t *db)
{
    lh_sql_mysql_conn_t *conn = lh_sql_mysql_get_conn(db);
    const lh_sql_mysql_api_t *api = lh_sql_mysql_conn_get_api(conn);
    lh_ptr native = lh_sql_mysql_connect(
        db, api, lh_sql_mysql_conn_get_host(conn), lh_sql_mysql_conn_get_user(conn),
        lh_sql_mysql_conn_get_password(conn), lh_sql_mysql_conn_get_name(conn),
        lh_sql_mysql_conn_get_port(conn));

    if (lh_null_eq(native))
    {
        return lh_bool_false;
    }
    lh_sql_mysql_api_close(api, lh_sql_mysql_conn_get_native(conn));
    lh_sql_mysql_conn_set_native(conn, native);
    conn->broken = lh_bool_false;
    lh_sql_db_clear_error(db);
    return lh_bool_true;
}

lh_bool_t
lh_sql_mysql_send(lh_sql_db_t *db, lh_str_view_t sql)
{
    lh_uint_t code;
    lh_sql_mysql_conn_t *conn = lh_sql_mysql_get_conn(db);

    if (conn->broken && !lh_sql_mysql_reconnect(db)) return lh_bool_false;

    if (lh_sql_mysql_api_real_query(lh_sql_mysql_get_api(db), lh_sql_mysql_get_native(db), sql))
    {
        return lh_bool_true;
    }
    code = lh_sql_mysql_api_error_code(lh_sql_mysql_get_api(db), lh_sql_mysql_get_native(db));
    /* Unknown outcome: never replay this statement, but don't reuse the
     * broken connection for the NEXT independent statement either. */
    if (code == LH_SQL_MYSQL_ERROR_SERVER_LOST)
    {
        conn->broken = lh_bool_true;
        return lh_sql_mysql_fail(db);
    }
    /* Never reached a live server, so nothing ran: safe to send again. */
    if (lh_math_ne(code, LH_SQL_MYSQL_ERROR_SERVER_GONE) &&
        lh_math_ne(code, LH_SQL_MYSQL_ERROR_IDLE_TIMEOUT))
    {
        return lh_sql_mysql_fail(db);
    }
    if (!lh_sql_mysql_reconnect(db))
    {
        return lh_bool_false;
    }
    if (!lh_sql_mysql_api_real_query(lh_sql_mysql_get_api(db), lh_sql_mysql_get_native(db), sql))
    {
        return lh_sql_mysql_fail(db);
    }
    return lh_bool_true;
}

/* ── driver callbacks ────────────────────────────────────────────────────── */

lh_void
lh_sql_mysql_close(lh_sql_db_t *db)
{
    lh_sql_mysql_conn_destroy(lh_sql_mysql_get_conn(db));
    lh_sql_db_set_handle(db, lh_null);
}

lh_bool_t
lh_sql_mysql_exec(lh_sql_db_t *db, lh_str_view_t sql)
{
    const lh_sql_mysql_api_t *api = lh_sql_mysql_get_api(db);
    lh_ptr result;

    if (!lh_sql_mysql_send(db, sql))
    {
        return lh_bool_false;
    }
    /* Rows nobody asked for still have to be taken off the connection, or
       the next query is "out of sync". No result where one was due is an
       error of its own. */
    result = lh_sql_mysql_api_store_result(api, lh_sql_mysql_get_native(db));
    if (lh_null_ne(result))
    {
        lh_sql_mysql_api_free_result(api, result);
    }
    else if (!lh_math_is_zero(lh_sql_mysql_api_field_count(api, lh_sql_mysql_get_native(db))))
    {
        return lh_sql_mysql_fail(db);
    }
    return lh_bool_true;
}

lh_sllong_t
lh_sql_mysql_get_last_insert_id(lh_sql_db_t *db)
{
    return lh_cast_static(lh_sllong_t, lh_sql_mysql_api_insert_id(lh_sql_mysql_get_api(db),
                                                                  lh_sql_mysql_get_native(db)));
}

lh_bool_t
lh_sql_mysql_prepare(lh_sql_db_t *db, lh_sql_stmt_t *stmt, lh_str_view_t sql)
{
    if (lh_str_view_is_empty(lh_addr_of(sql)))
    {
        return lh_sql_db_fail(db, lh_str_view_lit("no statement in the SQL text"));
    }
    lh_sql_stmt_set_handle(stmt, lh_sql_mysql_prepared_create(sql));
    return lh_bool_true;
}

lh_void
lh_sql_mysql_finalize(lh_sql_stmt_t *stmt)
{
    lh_sql_mysql_prepared_destroy(lh_sql_mysql_get_prepared(stmt),
                                  lh_sql_mysql_get_api(lh_sql_stmt_get_db(stmt)));
    lh_sql_stmt_set_handle(stmt, lh_null);
}

lh_bool_t
lh_sql_mysql_bind_null(lh_sql_stmt_t *stmt, lh_usize_t index)
{
    lh_sql_mysql_prepared_t *prepared = lh_sql_mysql_get_prepared(stmt);
    lh_str_t *literal = lh_sql_mysql_prepared_get_literal_mut(prepared, index);

    if (lh_null_eq(literal))
    {
        return lh_bool_false;
    }
    /* Binding after a run starts the next one, as SQLite's reset would. */
    lh_sql_mysql_prepared_rewind(prepared, lh_sql_mysql_get_api(lh_sql_stmt_get_db(stmt)));
    lh_str_assign_view(literal, lh_str_view_lit("NULL"));
    return lh_bool_true;
}

lh_bool_t
lh_sql_mysql_bind_sint(lh_sql_stmt_t *stmt, lh_usize_t index, lh_sllong_t value)
{
    lh_sql_mysql_prepared_t *prepared = lh_sql_mysql_get_prepared(stmt);
    lh_str_t *literal = lh_sql_mysql_prepared_get_literal_mut(prepared, index);
    lh_char_t digits[LH_SQL_MYSQL_INTEGER_DIGITS_MAX];

    if (lh_null_eq(literal))
    {
        return lh_bool_false;
    }
    lh_sql_mysql_prepared_rewind(prepared, lh_sql_mysql_get_api(lh_sql_stmt_get_db(stmt)));
    lh_str_clear(literal);
    lh_str_append(literal, digits,
                  lh_str_ptr_format_sllong(value, digits, LH_SQL_MYSQL_INTEGER_DIGITS_MAX));
    return lh_bool_true;
}

lh_bool_t
lh_sql_mysql_bind_str(lh_sql_stmt_t *stmt, lh_usize_t index, lh_str_view_t value)
{
    lh_sql_mysql_prepared_t *prepared = lh_sql_mysql_get_prepared(stmt);
    lh_str_t *literal = lh_sql_mysql_prepared_get_literal_mut(prepared, index);

    if (lh_null_eq(literal))
    {
        return lh_bool_false;
    }
    lh_sql_mysql_prepared_rewind(prepared, lh_sql_mysql_get_api(lh_sql_stmt_get_db(stmt)));
    lh_sql_mysql_quote(lh_sql_stmt_get_db(stmt), value, literal);
    return lh_bool_true;
}

lh_bool_t
lh_sql_mysql_execute(lh_sql_stmt_t *stmt)
{
    lh_sql_db_t *db = lh_sql_stmt_get_db_mut(stmt);
    lh_sql_mysql_prepared_t *prepared = lh_sql_mysql_get_prepared(stmt);
    const lh_sql_mysql_api_t *api = lh_sql_mysql_get_api(db);
    lh_str_t query;
    lh_str_view_t text;
    lh_bool_t sent;
    lh_ptr result;

    lh_str_init(lh_addr_of(query));
    if (!lh_sql_mysql_prepared_build(prepared, lh_addr_of(query)))
    {
        lh_str_deinit(lh_addr_of(query));
        return lh_sql_db_fail(db, lh_str_view_lit("a parameter is not bound"));
    }
    lh_str_as_view(lh_addr_of(query), lh_addr_of(text));
    sent = lh_sql_mysql_send(db, text);
    lh_str_deinit(lh_addr_of(query));
    if (!sent)
    {
        return lh_bool_false;
    }

    /* The connection may be a new one now: ask the one the text went to. */
    result = lh_sql_mysql_api_store_result(api, lh_sql_mysql_get_native(db));
    if (lh_null_eq(result) &&
        !lh_math_is_zero(lh_sql_mysql_api_field_count(api, lh_sql_mysql_get_native(db))))
    {
        return lh_sql_mysql_fail(db);
    }
    lh_sql_mysql_prepared_set_result(prepared, result);
    lh_sql_mysql_prepared_set_executed(prepared, lh_bool_true);
    return lh_bool_true;
}

lh_sql_step_t
lh_sql_mysql_step(lh_sql_stmt_t *stmt)
{
    lh_sql_mysql_prepared_t *prepared = lh_sql_mysql_get_prepared(stmt);
    const lh_sql_mysql_api_t *api = lh_sql_mysql_get_api(lh_sql_stmt_get_db(stmt));
    lh_ptr result;
    lh_char_t **row;

    if (!lh_sql_mysql_prepared_is_executed(prepared) && !lh_sql_mysql_execute(stmt))
    {
        return lh_sql_step_error;
    }
    result = lh_sql_mysql_prepared_get_result(prepared);
    /* An INSERT or UPDATE: run, and nothing to hand out. */
    if (lh_null_eq(result))
    {
        return lh_sql_step_done;
    }
    row = lh_sql_mysql_api_fetch_row(api, result);
    lh_sql_mysql_prepared_set_row(prepared, row);
    if (lh_null_eq(row))
    {
        lh_sql_mysql_prepared_set_lengths(prepared, lh_null);
        return lh_sql_step_done;
    }
    lh_sql_mysql_prepared_set_lengths(prepared, lh_sql_mysql_api_fetch_lengths(api, result));
    return lh_sql_step_row;
}

lh_bool_t
lh_sql_mysql_reset(lh_sql_stmt_t *stmt)
{
    lh_sql_mysql_prepared_rewind(lh_sql_mysql_get_prepared(stmt),
                                 lh_sql_mysql_get_api(lh_sql_stmt_get_db(stmt)));
    return lh_bool_true;
}

lh_usize_t
lh_sql_mysql_get_column_count(const lh_sql_stmt_t *stmt)
{
    lh_ptr result = lh_sql_mysql_prepared_get_result(lh_sql_mysql_get_prepared(stmt));

    if (lh_null_eq(result))
    {
        return 0U;
    }
    return lh_sql_mysql_api_num_fields(lh_sql_mysql_get_api(lh_sql_stmt_get_db(stmt)), result);
}

lh_bool_t
lh_sql_mysql_is_column_null(const lh_sql_stmt_t *stmt, lh_usize_t index)
{
    lh_char_t **row = lh_sql_mysql_prepared_get_row(lh_sql_mysql_get_prepared(stmt));

    /* The row array has exactly num_fields entries: past it is not a column. */
    return lh_cast_static(lh_bool_t, lh_null_eq(row) ||
                                         lh_math_ge(index, lh_sql_mysql_get_column_count(stmt)) ||
                                         lh_null_eq(row[index]));
}

lh_sllong_t
lh_sql_mysql_get_column_sint(const lh_sql_stmt_t *stmt, lh_usize_t index)
{
    lh_str_view_t text = lh_sql_mysql_get_column_str(stmt, index);
    lh_sllong_t value;

    if (lh_str_view_is_empty(lh_addr_of(text)) ||
        !lh_str_ptr_parse_sllong(lh_str_view_get_begin(lh_addr_of(text)),
                                 lh_str_view_get_size(lh_addr_of(text)),
                                 lh_numeric_limit_min(lh_sllong_t),
                                 lh_numeric_limit_max(lh_sllong_t), lh_addr_of(value)))
    {
        return 0;
    }
    return value;
}

lh_str_view_t
lh_sql_mysql_get_column_str(const lh_sql_stmt_t *stmt, lh_usize_t index)
{
    const lh_sql_mysql_prepared_t *prepared = lh_sql_mysql_get_prepared(stmt);
    lh_str_view_t view;
    lh_usize_t size;

    if (lh_sql_mysql_is_column_null(stmt, index))
    {
        return lh_str_view_empty();
    }
    size = lh_cast_static(lh_usize_t, lh_sql_mysql_prepared_get_lengths(prepared)[index]);
    if (lh_math_is_zero(size))
    {
        return lh_str_view_empty();
    }
    lh_str_view_init_by_size(lh_addr_of(view), lh_sql_mysql_prepared_get_row(prepared)[index], size);
    return view;
}

/* ── open ────────────────────────────────────────────────────────────────── */

static const lh_sql_driver_t m_sql_mysql_driver = {
    "mysql",
    lh_sql_mysql_close,
    lh_sql_mysql_exec,
    lh_sql_mysql_get_last_insert_id,
    lh_sql_mysql_prepare,
    lh_sql_mysql_finalize,
    lh_sql_mysql_bind_null,
    lh_sql_mysql_bind_sint,
    lh_sql_mysql_bind_str,
    lh_sql_mysql_step,
    lh_sql_mysql_reset,
    lh_sql_mysql_get_column_count,
    lh_sql_mysql_is_column_null,
    lh_sql_mysql_get_column_sint,
    lh_sql_mysql_get_column_str,
};

const lh_sql_driver_t *
lh_sql_mysql_get_driver(void)
{
    return lh_addr_of(m_sql_mysql_driver);
}

const lh_char_t *
lh_sql_mysql_to_cstr(lh_str_t *storage, lh_str_view_t text)
{
    lh_str_init(storage);
    lh_str_append_view(storage, text);
    /* An empty string may own no buffer; the client wants "" all the same. */
    return lh_str_is_empty(storage) ? "" : lh_str_get_data(storage);
}

lh_ptr
lh_sql_mysql_connect(lh_sql_db_t *db, const lh_sql_mysql_api_t *api, lh_str_view_t host,
                     lh_str_view_t user, lh_str_view_t password, lh_str_view_t name,
                     lh_uint_t port)
{
    lh_str_t host_text;
    lh_str_t user_text;
    lh_str_t password_text;
    lh_str_t name_text;
    lh_ptr native;
    lh_ptr connected;
    lh_bool_t charset;

    native = lh_sql_mysql_api_init_native(api);
    if (lh_null_eq(native))
    {
        lh_sql_db_set_error(db, lh_str_view_lit("mysql_init failed"));
        return lh_null;
    }

    if (api->options)
    {
        const lh_uint_t seconds = 5U;
        /* Stable C client ABI: CONNECT_TIMEOUT, READ_TIMEOUT, WRITE_TIMEOUT.
         * Keep network failures bounded, including worker joins on unload. */
        if (api->options(native, 0, &seconds) || api->options(native, 11, &seconds) ||
            api->options(native, 12, &seconds))
        {
            lh_sql_db_set_error(db, lh_str_view_lit("cannot set MySQL I/O timeouts"));
            lh_sql_mysql_api_close(api, native);
            return lh_null;
        }
    }

    connected = lh_sql_mysql_api_real_connect(
        api, native, lh_sql_mysql_to_cstr(lh_addr_of(host_text), host),
        lh_sql_mysql_to_cstr(lh_addr_of(user_text), user),
        lh_sql_mysql_to_cstr(lh_addr_of(password_text), password),
        lh_sql_mysql_to_cstr(lh_addr_of(name_text), name), port);
    lh_str_deinit(lh_addr_of(host_text));
    lh_str_deinit(lh_addr_of(user_text));
    lh_str_deinit(lh_addr_of(password_text));
    lh_str_deinit(lh_addr_of(name_text));

    /* utf8mb4 is all of UTF-8; servers before 5.5.3 only know utf8. Escaping
       is only safe once the client knows the character set. */
    charset =
        lh_cast_static(lh_bool_t, lh_null_ne(connected) &&
                                      (lh_sql_mysql_api_set_character_set(api, native, "utf8mb4") ||
                                       lh_sql_mysql_api_set_character_set(api, native, "utf8")));
    if (!charset)
    {
        const lh_str_view_t message = lh_sql_mysql_api_error(api, native);

        lh_sql_db_set_error(db, lh_str_view_is_empty(lh_addr_of(message))
                                    ? lh_str_view_lit("cannot connect to MySQL")
                                    : message);
        lh_sql_mysql_api_close(api, native);
        return lh_null;
    }
    return native;
}

lh_bool_t
lh_sql_db_open_mysql_with(lh_sql_db_t *db, const lh_sql_mysql_api_t *api, lh_str_view_t host,
                          lh_str_view_t user, lh_str_view_t password, lh_str_view_t name,
                          lh_uint_t port)
{
    lh_ptr native;

    lh_sql_db_close(db);
    lh_sql_db_clear_error(db);
    if (lh_null_eq(api) || !lh_sql_mysql_api_is_loaded(api))
    {
        return lh_sql_db_fail(db, lh_str_view_lit("MySQL client library not found"));
    }
    if (lh_math_is_zero(port))
    {
        port = LH_SQL_MYSQL_PORT_DEFAULT;
    }
    native = lh_sql_mysql_connect(db, api, host, user, password, name, port);
    if (lh_null_eq(native))
    {
        return lh_bool_false;
    }

    lh_sql_db_set_handle(db,
                         lh_sql_mysql_conn_create(api, native, host, user, password, name, port));
    lh_sql_db_set_driver(db, lh_sql_mysql_get_driver());
    return lh_bool_true;
}

lh_bool_t
lh_sql_db_open_mysql(lh_sql_db_t *db, lh_str_view_t host, lh_str_view_t user,
                     lh_str_view_t password, lh_str_view_t name, lh_uint_t port)
{
    return lh_sql_db_open_mysql_with(db, lh_sql_mysql_api_get_default(), host, user, password, name,
                                     port);
}
