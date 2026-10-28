/**
 * @file api.h
 * @brief The MySQL / MariaDB C client, loaded at run time
 *        (::lh_sql_mysql_api_t).
 *
 * lh does not link the client: the host's own `libmariadb` / `libmysqlclient`
 * is opened with ::lh_os_shared_t and the dozen `mysql_*` functions lh/sql
 * needs are looked up in it. A server without the library still runs, it
 * only cannot open a MySQL database. The handles (`MYSQL *`, `MYSQL_RES *`)
 * stay opaque pointers: no client header is needed to build lh.
 *
 * The table can also be filled by hand — a client linked statically, a
 * test double — and given to ::lh_sql_db_open_mysql_with.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_SQL_MYSQL_API_H
#define LH_SQL_MYSQL_API_H

#include <lh/os/shared.h>
#include <lh/sql/mysql/api/fields.h>

/* ── the C API, as the client exports it ─────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** `MYSQL *mysql_init(MYSQL *)`. */
typedef lh_ptr(lh_sql_mysql_init_fn)(lh_ptr mysql);

/** `MYSQL *mysql_real_connect(MYSQL *, host, user, passwd, db, port, unix_socket, flags)`. */
typedef lh_ptr(lh_sql_mysql_real_connect_fn)(lh_ptr mysql, const lh_char_t *host,
                                             const lh_char_t *user, const lh_char_t *password,
                                             const lh_char_t *db, lh_uint_t port,
                                             const lh_char_t *unix_socket, lh_ulong_t flags);

/** `void mysql_close(MYSQL *)`. */
typedef lh_void(lh_sql_mysql_close_fn)(lh_ptr mysql);

/** `const char *mysql_error(MYSQL *)`. */
typedef const lh_char_t *(lh_sql_mysql_error_fn)(lh_ptr mysql);

/** `unsigned int mysql_errno(MYSQL *)`: the number of the last failure, `0` after a success. */
typedef lh_uint_t(lh_sql_mysql_error_code_fn)(lh_ptr mysql);

/** `int mysql_real_query(MYSQL *, const char *, unsigned long)`. */
typedef lh_sint_t(lh_sql_mysql_real_query_fn)(lh_ptr mysql, const lh_char_t *query,
                                              lh_ulong_t length);

/** `MYSQL_RES *mysql_store_result(MYSQL *)`. */
typedef lh_ptr(lh_sql_mysql_store_result_fn)(lh_ptr mysql);

/** `void mysql_free_result(MYSQL_RES *)`. */
typedef lh_void(lh_sql_mysql_free_result_fn)(lh_ptr result);

/** `MYSQL_ROW mysql_fetch_row(MYSQL_RES *)`. */
typedef lh_char_t **(lh_sql_mysql_fetch_row_fn)(lh_ptr result);

/** `unsigned long *mysql_fetch_lengths(MYSQL_RES *)`. */
typedef lh_ulong_t *(lh_sql_mysql_fetch_lengths_fn)(lh_ptr result);

/** `unsigned int mysql_num_fields(MYSQL_RES *)`. */
typedef lh_uint_t(lh_sql_mysql_num_fields_fn)(lh_ptr result);

/** `unsigned int mysql_field_count(MYSQL *)`: columns the last statement should return. */
typedef lh_uint_t(lh_sql_mysql_field_count_fn)(lh_ptr mysql);

/** `my_ulonglong mysql_insert_id(MYSQL *)`. */
typedef lh_ullong_t(lh_sql_mysql_insert_id_fn)(lh_ptr mysql);

/** `unsigned long mysql_real_escape_string(MYSQL *, char *to, const char *from, unsigned long)`. */
typedef lh_ulong_t(lh_sql_mysql_real_escape_string_fn)(lh_ptr mysql, lh_char_t *to,
                                                       const lh_char_t *from, lh_ulong_t length);

/** `int mysql_set_character_set(MYSQL *, const char *)`. */
typedef lh_sint_t(lh_sql_mysql_set_character_set_fn)(lh_ptr mysql, const lh_char_t *name);

/** `int mysql_options(MYSQL *, int option, const void *)`. */
typedef lh_sint_t(lh_sql_mysql_options_fn)(lh_ptr mysql, lh_sint_t option, const lh_void *value);

/** `void mysql_thread_end(void)`. */
typedef lh_void(lh_sql_mysql_thread_end_fn)(lh_void);

LH_COMPILER_EXTERN_C_END

/**
 * @struct lh_sql_mysql_api
 * @brief The client image and its functions. Fields via
 *        ::lh_sql_mysql_api_fields.
 */
struct lh_sql_mysql_api
{
    /** Optional for injected clients; mysql_options on real client libraries. */
    lh_sql_mysql_options_fn *options;
    /** Optional current-thread cleanup; absent in some injected clients. */
    lh_sql_mysql_thread_end_fn *thread_end;
    /** Members: ::lh_sql_mysql_api_fields. */
    lh_sql_mysql_api_fields(lh_os_shared_t, lh_sql_mysql_init_fn, lh_sql_mysql_real_connect_fn,
                            lh_sql_mysql_close_fn, lh_sql_mysql_error_fn,
                            lh_sql_mysql_error_code_fn, lh_sql_mysql_real_query_fn,
                            lh_sql_mysql_store_result_fn, lh_sql_mysql_free_result_fn,
                            lh_sql_mysql_fetch_row_fn, lh_sql_mysql_fetch_lengths_fn,
                            lh_sql_mysql_num_fields_fn, lh_sql_mysql_field_count_fn,
                            lh_sql_mysql_insert_id_fn, lh_sql_mysql_real_escape_string_fn,
                            lh_sql_mysql_set_character_set_fn);
};
/** @copybrief lh_sql_mysql_api */
typedef struct lh_sql_mysql_api lh_sql_mysql_api_t;

/* ── load ────────────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The library image of @p self, to change. */
lh_os_shared_t *
lh_sql_mysql_api_get_image_mut(lh_sql_mysql_api_t *self);

/**
 * @brief `mysql_init` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_init_fn *
lh_sql_mysql_api_get_init(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_init` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_init(lh_sql_mysql_api_t *self, lh_sql_mysql_init_fn *init);

/**
 * @brief `mysql_real_connect` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_real_connect_fn *
lh_sql_mysql_api_get_real_connect(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_real_connect` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_real_connect(lh_sql_mysql_api_t *self, lh_sql_mysql_real_connect_fn *real_connect);

/**
 * @brief `mysql_close` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_close_fn *
lh_sql_mysql_api_get_close(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_close` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_close(lh_sql_mysql_api_t *self, lh_sql_mysql_close_fn *close);

/**
 * @brief `mysql_error` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_error_fn *
lh_sql_mysql_api_get_error(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_error` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_error(lh_sql_mysql_api_t *self, lh_sql_mysql_error_fn *error);

/**
 * @brief `mysql_error_code` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_error_code_fn *
lh_sql_mysql_api_get_error_code(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_error_code` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_error_code(lh_sql_mysql_api_t *self, lh_sql_mysql_error_code_fn *error_code);

/**
 * @brief `mysql_real_query` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_real_query_fn *
lh_sql_mysql_api_get_real_query(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_real_query` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_real_query(lh_sql_mysql_api_t *self, lh_sql_mysql_real_query_fn *real_query);

/**
 * @brief `mysql_store_result` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_store_result_fn *
lh_sql_mysql_api_get_store_result(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_store_result` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_store_result(lh_sql_mysql_api_t *self, lh_sql_mysql_store_result_fn *store_result);

/**
 * @brief `mysql_free_result` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_free_result_fn *
lh_sql_mysql_api_get_free_result(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_free_result` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_free_result(lh_sql_mysql_api_t *self, lh_sql_mysql_free_result_fn *free_result);

/**
 * @brief `mysql_fetch_row` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_fetch_row_fn *
lh_sql_mysql_api_get_fetch_row(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_fetch_row` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_fetch_row(lh_sql_mysql_api_t *self, lh_sql_mysql_fetch_row_fn *fetch_row);

/**
 * @brief `mysql_fetch_lengths` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_fetch_lengths_fn *
lh_sql_mysql_api_get_fetch_lengths(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_fetch_lengths` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_fetch_lengths(lh_sql_mysql_api_t *self, lh_sql_mysql_fetch_lengths_fn *fetch_lengths);

/**
 * @brief `mysql_num_fields` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_num_fields_fn *
lh_sql_mysql_api_get_num_fields(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_num_fields` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_num_fields(lh_sql_mysql_api_t *self, lh_sql_mysql_num_fields_fn *num_fields);

/**
 * @brief `mysql_field_count` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_field_count_fn *
lh_sql_mysql_api_get_field_count(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_field_count` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_field_count(lh_sql_mysql_api_t *self, lh_sql_mysql_field_count_fn *field_count);

/**
 * @brief `mysql_insert_id` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_insert_id_fn *
lh_sql_mysql_api_get_insert_id(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_insert_id` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_insert_id(lh_sql_mysql_api_t *self, lh_sql_mysql_insert_id_fn *insert_id);

/**
 * @brief `mysql_real_escape_string` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_real_escape_string_fn *
lh_sql_mysql_api_get_real_escape_string(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_real_escape_string` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_real_escape_string(lh_sql_mysql_api_t *self, lh_sql_mysql_real_escape_string_fn *real_escape_string);

/**
 * @brief `mysql_set_character_set` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_set_character_set_fn *
lh_sql_mysql_api_get_set_character_set(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_set_character_set` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_set_character_set(lh_sql_mysql_api_t *self, lh_sql_mysql_set_character_set_fn *set_character_set);

/**
 * @brief `mysql_options` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_options_fn *
lh_sql_mysql_api_get_options(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_options` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_options(lh_sql_mysql_api_t *self, lh_sql_mysql_options_fn *options);

/**
 * @brief `mysql_thread_end` as looked up in the client library, or ::lh_null
 *        when the library lacks it (level 1).
 */
lh_sql_mysql_thread_end_fn *
lh_sql_mysql_api_get_thread_end(const lh_sql_mysql_api_t *self);

/** @brief Store `mysql_thread_end` (::lh_null for none). Level-1 setter. */
lh_void
lh_sql_mysql_api_set_thread_end(lh_sql_mysql_api_t *self, lh_sql_mysql_thread_end_fn *thread_end);

/** Set @p self to no library and no functions. */
lh_void
lh_sql_mysql_api_init(lh_sql_mysql_api_t *self);

/** True when every function lh/sql needs is in @p self. */
lh_bool_t
lh_sql_mysql_api_is_loaded(const lh_sql_mysql_api_t *self);

/* ── calls: the one place each slot is read ──────────────────────────────── */

/** `mysql_init(NULL)`. */
lh_ptr
lh_sql_mysql_api_init_native(const lh_sql_mysql_api_t *self);

/** `mysql_real_connect` with NUL-terminated strings; ::lh_null on failure. */
lh_ptr
lh_sql_mysql_api_real_connect(const lh_sql_mysql_api_t *self, lh_ptr mysql, const lh_char_t *host,
                              const lh_char_t *user, const lh_char_t *password, const lh_char_t *db,
                              lh_uint_t port);

/** `mysql_close`. */
lh_void
lh_sql_mysql_api_close(const lh_sql_mysql_api_t *self, lh_ptr mysql);

/**
 * @brief Release the client's state for the calling thread, when supported.
 *
 * Close all connections and results used by this thread before calling.
 * The caller retains ownership of @p self and must keep its image loaded
 * until this call returns. An absent optional callback is a no-op.
 */
lh_void
lh_sql_mysql_api_thread_end(const lh_sql_mysql_api_t *self);

/** `mysql_error`, as a view; empty when the client has none. */
lh_str_view_t
lh_sql_mysql_api_error(const lh_sql_mysql_api_t *self, lh_ptr mysql);

/** `mysql_errno`: the number of the last failure, `0` when there is none. */
lh_uint_t
lh_sql_mysql_api_error_code(const lh_sql_mysql_api_t *self, lh_ptr mysql);

/** `mysql_real_query`; ::lh_bool_true on success. */
lh_bool_t
lh_sql_mysql_api_real_query(const lh_sql_mysql_api_t *self, lh_ptr mysql, lh_str_view_t query);

/** `mysql_store_result`; ::lh_null when the statement returned no rows or it failed. */
lh_ptr
lh_sql_mysql_api_store_result(const lh_sql_mysql_api_t *self, lh_ptr mysql);

/** `mysql_free_result`. */
lh_void
lh_sql_mysql_api_free_result(const lh_sql_mysql_api_t *self, lh_ptr result);

/** `mysql_fetch_row`; ::lh_null after the last row. */
lh_char_t **
lh_sql_mysql_api_fetch_row(const lh_sql_mysql_api_t *self, lh_ptr result);

/** `mysql_fetch_lengths` of the current row. */
lh_ulong_t *
lh_sql_mysql_api_fetch_lengths(const lh_sql_mysql_api_t *self, lh_ptr result);

/** `mysql_num_fields`. */
lh_usize_t
lh_sql_mysql_api_num_fields(const lh_sql_mysql_api_t *self, lh_ptr result);

/** `mysql_field_count`. */
lh_usize_t
lh_sql_mysql_api_field_count(const lh_sql_mysql_api_t *self, lh_ptr mysql);

/** `mysql_insert_id`. */
lh_ullong_t
lh_sql_mysql_api_insert_id(const lh_sql_mysql_api_t *self, lh_ptr mysql);

/**
 * @brief `mysql_real_escape_string` of @p text, appended to @p out — the
 *        escapes the connection's character set and SQL mode want.
 */
lh_void
lh_sql_mysql_api_escape(const lh_sql_mysql_api_t *self, lh_ptr mysql, lh_str_view_t text,
                        lh_str_t *out);

/** `mysql_set_character_set`; ::lh_bool_true on success. */
lh_bool_t
lh_sql_mysql_api_set_character_set(const lh_sql_mysql_api_t *self, lh_ptr mysql,
                                   const lh_char_t *name);

/** Close the library @p self loaded and forget its functions. */
lh_void
lh_sql_mysql_api_deinit(lh_sql_mysql_api_t *self);

/**
 * @brief Look every function up in the image of @p self, which must be open.
 *
 * @return ::lh_bool_false, with the functions cleared, when one is missing:
 *         the image is not a MySQL client.
 */
lh_bool_t
lh_sql_mysql_api_bind(lh_sql_mysql_api_t *self);

/**
 * @brief Open the library @p name (a file name the OS searches for, or a
 *        path) into @p self and bind its functions.
 */
lh_bool_t
lh_sql_mysql_api_load(lh_sql_mysql_api_t *self, lh_str_view_t name);

/**
 * @brief Try the usual client names in turn — MariaDB's, then MySQL's, as
 *        Windows or Linux names them — until one loads.
 */
lh_bool_t
lh_sql_mysql_api_load_default(lh_sql_mysql_api_t *self);

/**
 * @brief The process's client, loaded by ::lh_sql_mysql_api_load_default on
 *        the first call; ::lh_null when there is none. Later calls do not
 *        try again. Not thread safe on that first call.
 */
const lh_sql_mysql_api_t *
lh_sql_mysql_api_get_default(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_MYSQL_API_H */
