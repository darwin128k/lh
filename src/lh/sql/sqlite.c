#include <lh/sql/sqlite/native.h>
#include <lh/assert.h>

/* ── native handles ──────────────────────────────────────────────────────── */

sqlite3 *
lh_sql_sqlite_get_native_db(const lh_sql_db_t *db)
{
    return lh_ptr_cast(sqlite3, lh_sql_db_get_handle(db));
}

sqlite3_stmt *
lh_sql_sqlite_get_native_stmt(const lh_sql_stmt_t *stmt)
{
    return lh_ptr_cast(sqlite3_stmt, lh_sql_stmt_get_handle(stmt));
}

lh_bool_t
lh_sql_sqlite_to_int(lh_usize_t index, int *out)
{
    lh_assert_runtime_ref(out);
    if (lh_math_gt(index, lh_cast_static(lh_usize_t, lh_numeric_limit_max(int))))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_cast_static(int, index);
    return lh_bool_true;
}

/* ── driver callbacks ────────────────────────────────────────────────────── */

lh_bool_t
lh_sql_sqlite_fail(lh_sql_db_t *db)
{
    lh_str_view_t message;

    /* sqlite3_errmsg is never null: without a connection it says "out of memory". */
    lh_str_view_init(lh_addr_of(message), sqlite3_errmsg(lh_sql_sqlite_get_native_db(db)));
    return lh_sql_db_fail(db, message);
}

lh_void
lh_sql_sqlite_close(lh_sql_db_t *db)
{
    sqlite3_close_v2(lh_sql_sqlite_get_native_db(db));
    lh_sql_db_set_handle(db, lh_null);
}

lh_bool_t
lh_sql_sqlite_exec(lh_sql_db_t *db, lh_str_view_t sql)
{
    const lh_str_cptr end = lh_str_view_get_end(lh_addr_of(sql));
    lh_str_cptr at = lh_str_view_get_begin(lh_addr_of(sql));
    int size;

    /* One statement at a time, like sqlite3_exec, but on a view: the text
       need not end in a NUL. */
    while (lh_math_lt(at, end))
    {
        sqlite3_stmt *stmt = lh_null;
        const char *tail = lh_null;
        int rc;

        if (!lh_sql_sqlite_to_int(lh_cast_static(lh_usize_t, end - at), lh_addr_of(size)))
        {
            return lh_sql_db_fail(db, lh_str_view_lit("SQL text too long"));
        }
        if (lh_math_ne(sqlite3_prepare_v2(lh_sql_sqlite_get_native_db(db), at, size,
                                          lh_addr_of(stmt), lh_addr_of(tail)),
                       SQLITE_OK))
        {
            return lh_sql_sqlite_fail(db);
        }
        /* Only whitespace or a comment left: nothing was compiled. */
        if (lh_null_eq(stmt))
        {
            break;
        }
        do
        {
            rc = sqlite3_step(stmt);
        } while (lh_math_eq(rc, SQLITE_ROW));
        sqlite3_finalize(stmt);
        if (lh_math_ne(rc, SQLITE_DONE))
        {
            return lh_sql_sqlite_fail(db);
        }
        at = tail;
    }
    return lh_bool_true;
}

lh_sllong_t
lh_sql_sqlite_get_last_insert_id(lh_sql_db_t *db)
{
    return lh_cast_static(lh_sllong_t, sqlite3_last_insert_rowid(lh_sql_sqlite_get_native_db(db)));
}

lh_bool_t
lh_sql_sqlite_prepare(lh_sql_db_t *db, lh_sql_stmt_t *stmt, lh_str_view_t sql)
{
    sqlite3_stmt *native = lh_null;
    int size;

    if (!lh_sql_sqlite_to_int(lh_str_view_get_size(lh_addr_of(sql)), lh_addr_of(size)))
    {
        return lh_sql_db_fail(db, lh_str_view_lit("SQL text too long"));
    }
    if (lh_math_ne(sqlite3_prepare_v2(lh_sql_sqlite_get_native_db(db),
                                      lh_str_view_get_begin(lh_addr_of(sql)), size,
                                      lh_addr_of(native), lh_null),
                   SQLITE_OK))
    {
        return lh_sql_sqlite_fail(db);
    }
    if (lh_null_eq(native))
    {
        return lh_sql_db_fail(db, lh_str_view_lit("no statement in the SQL text"));
    }
    lh_sql_stmt_set_handle(stmt, native);
    return lh_bool_true;
}

lh_void
lh_sql_sqlite_finalize(lh_sql_stmt_t *stmt)
{
    sqlite3_finalize(lh_sql_sqlite_get_native_stmt(stmt));
    lh_sql_stmt_set_handle(stmt, lh_null);
}

lh_bool_t
lh_sql_sqlite_bind_null(lh_sql_stmt_t *stmt, lh_usize_t index)
{
    int at;

    return lh_sql_sqlite_to_int(index, lh_addr_of(at)) &&
           lh_math_eq(sqlite3_bind_null(lh_sql_sqlite_get_native_stmt(stmt), at), SQLITE_OK);
}

lh_bool_t
lh_sql_sqlite_bind_sint(lh_sql_stmt_t *stmt, lh_usize_t index, lh_sllong_t value)
{
    int at;

    return lh_sql_sqlite_to_int(index, lh_addr_of(at)) &&
           lh_math_eq(sqlite3_bind_int64(lh_sql_sqlite_get_native_stmt(stmt), at, value),
                      SQLITE_OK);
}

lh_bool_t
lh_sql_sqlite_bind_str(lh_sql_stmt_t *stmt, lh_usize_t index, lh_str_view_t value)
{
    /* An empty view may have no data pointer; SQLite takes "" for empty text,
       a null pointer for NULL. */
    const lh_str_cptr text =
        lh_str_view_is_empty(lh_addr_of(value)) ? "" : lh_str_view_get_begin(lh_addr_of(value));
    int at;

    if (!lh_sql_sqlite_to_int(index, lh_addr_of(at)))
    {
        return lh_bool_false;
    }
    int size;

    if (!lh_sql_sqlite_to_int(lh_str_view_get_size(lh_addr_of(value)), lh_addr_of(size)))
    {
        return lh_bool_false;
    }
    return lh_math_eq(
        sqlite3_bind_text(lh_sql_sqlite_get_native_stmt(stmt), at, text, size, SQLITE_TRANSIENT),
        SQLITE_OK);
}

lh_sql_step_t
lh_sql_sqlite_step(lh_sql_stmt_t *stmt)
{
    const int rc = sqlite3_step(lh_sql_sqlite_get_native_stmt(stmt));

    if (lh_math_eq(rc, SQLITE_ROW))
    {
        return lh_sql_step_row;
    }
    if (lh_math_eq(rc, SQLITE_DONE))
    {
        return lh_sql_step_done;
    }
    lh_sql_sqlite_fail(lh_sql_stmt_get_db_mut(stmt));
    return lh_sql_step_error;
}

lh_bool_t
lh_sql_sqlite_reset(lh_sql_stmt_t *stmt)
{
    return lh_cast_static(
        lh_bool_t, lh_math_eq(sqlite3_reset(lh_sql_sqlite_get_native_stmt(stmt)), SQLITE_OK));
}

lh_usize_t
lh_sql_sqlite_get_column_count(const lh_sql_stmt_t *stmt)
{
    return lh_cast_static(lh_usize_t, sqlite3_column_count(lh_sql_sqlite_get_native_stmt(stmt)));
}

lh_bool_t
lh_sql_sqlite_is_column_null(const lh_sql_stmt_t *stmt, lh_usize_t index)
{
    int at;

    return lh_cast_static(
        lh_bool_t,
        !lh_sql_sqlite_to_int(index, lh_addr_of(at)) ||
            lh_math_eq(sqlite3_column_type(lh_sql_sqlite_get_native_stmt(stmt), at), SQLITE_NULL));
}

lh_sllong_t
lh_sql_sqlite_get_column_sint(const lh_sql_stmt_t *stmt, lh_usize_t index)
{
    int at;

    if (!lh_sql_sqlite_to_int(index, lh_addr_of(at)))
    {
        return 0;
    }
    return lh_cast_static(lh_sllong_t,
                          sqlite3_column_int64(lh_sql_sqlite_get_native_stmt(stmt), at));
}

lh_str_view_t
lh_sql_sqlite_get_column_str(const lh_sql_stmt_t *stmt, lh_usize_t index)
{
    sqlite3_stmt *native = lh_sql_sqlite_get_native_stmt(stmt);
    int at;

    if (!lh_sql_sqlite_to_int(index, lh_addr_of(at)))
    {
        return lh_str_view_empty();
    }
    /* Text first, then its length: the documented order, as reading the text
       may convert the value. */
    const unsigned char *text = sqlite3_column_text(native, at);
    const int size = sqlite3_column_bytes(native, at);

    if (lh_null_eq(text) || lh_math_le(size, 0))
    {
        return lh_str_view_empty();
    }
    lh_str_view_t view;

    lh_str_view_init_by_size(lh_addr_of(view), lh_ptr_cast(const lh_char_t, text),
                        lh_cast_static(lh_usize_t, size));
    return view;
}

/* ── open ────────────────────────────────────────────────────────────────── */

static const lh_sql_driver_t m_sql_sqlite_driver = {
    "sqlite",
    lh_sql_sqlite_close,
    lh_sql_sqlite_exec,
    lh_sql_sqlite_get_last_insert_id,
    lh_sql_sqlite_prepare,
    lh_sql_sqlite_finalize,
    lh_sql_sqlite_bind_null,
    lh_sql_sqlite_bind_sint,
    lh_sql_sqlite_bind_str,
    lh_sql_sqlite_step,
    lh_sql_sqlite_reset,
    lh_sql_sqlite_get_column_count,
    lh_sql_sqlite_is_column_null,
    lh_sql_sqlite_get_column_sint,
    lh_sql_sqlite_get_column_str,
};

const lh_sql_driver_t *
lh_sql_sqlite_get_driver(lh_void)
{
    return lh_addr_of(m_sql_sqlite_driver);
}

lh_bool_t
lh_sql_db_open_sqlite(lh_sql_db_t *db, lh_str_view_t path)
{
    sqlite3 *native = lh_null;
    lh_str_t name;

    lh_sql_db_close(db);
    lh_sql_db_clear_error(db);
    if (lh_str_view_is_empty(lh_addr_of(path)))
    {
        return lh_sql_db_fail(db, lh_str_view_lit("empty database path"));
    }

    /* SQLite wants a NUL-terminated name; the view need not have one. */
    lh_str_init_by_view(lh_addr_of(name), path);
    const int rc = sqlite3_open_v2(lh_str_get_data(lh_addr_of(name)), lh_addr_of(native),
                                   SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, lh_null);

    lh_str_deinit(lh_addr_of(name));

    lh_sql_db_set_handle(db, native);
    if (lh_math_ne(rc, SQLITE_OK))
    {
        lh_sql_sqlite_fail(db);
        sqlite3_close_v2(native);
        lh_sql_db_set_handle(db, lh_null);
        return lh_bool_false;
    }
    lh_sql_db_set_driver(db, lh_sql_sqlite_get_driver());
    return lh_bool_true;
}
