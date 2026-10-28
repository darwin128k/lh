/**
 * @file prepared.h
 * @brief What the handle of an ::lh_sql_stmt_t prepared on MySQL points at
 *        (::lh_sql_mysql_prepared_t): the statement as text and the
 *        literals bound to it (see lh/sql/mysql/query.h).
 */

#ifndef LH_SQL_MYSQL_PREPARED_H
#define LH_SQL_MYSQL_PREPARED_H

#include <lh/sql/mysql/api.h>
#include <lh/sql/mysql/prepared/fields.h>

/**
 * @struct lh_sql_mysql_prepared
 * @brief Text, literals and the current result. Fields via
 *        ::lh_sql_mysql_prepared_fields.
 */
struct lh_sql_mysql_prepared
{
    /** Members: ::lh_sql_mysql_prepared_fields. */
    lh_sql_mysql_prepared_fields(lh_str_t, lh_array_t, lh_ptr, lh_char_t **, lh_ulong_t *,
                                 lh_bool_t);
};
/** @copybrief lh_sql_mysql_prepared */
typedef struct lh_sql_mysql_prepared lh_sql_mysql_prepared_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The statement text, to change. */
lh_str_t *
lh_sql_mysql_prepared_get_sql_mut(lh_sql_mysql_prepared_t *self);

/** The statement text, placeholders and all. */
const lh_str_t *
lh_sql_mysql_prepared_get_sql(const lh_sql_mysql_prepared_t *self);

/** The literals bound so far, to change. */
lh_array_t *
lh_sql_mysql_prepared_get_literals_mut(lh_sql_mysql_prepared_t *self);

/** The literals bound so far: one ::lh_str_t per parameter, empty when unbound. */
const lh_array_t *
lh_sql_mysql_prepared_get_literals(const lh_sql_mysql_prepared_t *self);

/** The client's `MYSQL_RES *`; ::lh_null before the run or without rows. */
lh_ptr
lh_sql_mysql_prepared_get_result(const lh_sql_mysql_prepared_t *self);

/** Make @p result the client's `MYSQL_RES *`. */
lh_void
lh_sql_mysql_prepared_set_result(lh_sql_mysql_prepared_t *self, lh_ptr result);

/** The current row (`MYSQL_ROW`); ::lh_null before the first and after the last. */
lh_char_t **
lh_sql_mysql_prepared_get_row(const lh_sql_mysql_prepared_t *self);

/** Make @p row the current row. */
lh_void
lh_sql_mysql_prepared_set_row(lh_sql_mysql_prepared_t *self, lh_char_t **row);

/** The byte length of each column of the current row. */
lh_ulong_t *
lh_sql_mysql_prepared_get_lengths(const lh_sql_mysql_prepared_t *self);

/** Make @p lengths the byte lengths of the current row. */
lh_void
lh_sql_mysql_prepared_set_lengths(lh_sql_mysql_prepared_t *self, lh_ulong_t *lengths);

/** True once the text was sent to the server. */
lh_bool_t
lh_sql_mysql_prepared_is_executed(const lh_sql_mysql_prepared_t *self);

/** Make @p executed whether the text was sent. */
lh_void
lh_sql_mysql_prepared_set_executed(lh_sql_mysql_prepared_t *self, lh_bool_t executed);

/* ── build ───────────────────────────────────────────────────────────────── */

/** A new statement holding a copy of @p sql, nothing bound, from the runtime allocator. */
lh_sql_mysql_prepared_t *
lh_sql_mysql_prepared_create(lh_str_view_t sql);

/**
 * @brief The literal of parameter @p index (from 1), to write; the list
 *        grows to hold it. ::lh_null for index `0`.
 */
lh_str_t *
lh_sql_mysql_prepared_get_literal_mut(lh_sql_mysql_prepared_t *self, lh_usize_t index);

/**
 * @brief Append the statement text with every placeholder replaced by its
 *        literal to @p out (::lh_sql_mysql_query_substitute).
 */
lh_bool_t
lh_sql_mysql_prepared_build(const lh_sql_mysql_prepared_t *self, lh_str_t *out);

/** Free the current result with @p api and forget the run, keeping the bindings. */
lh_void
lh_sql_mysql_prepared_rewind(lh_sql_mysql_prepared_t *self, const lh_sql_mysql_api_t *api);

/** Rewind with @p api, free the text and the literals, and the record itself. */
lh_void
lh_sql_mysql_prepared_destroy(lh_sql_mysql_prepared_t *self, const lh_sql_mysql_api_t *api);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_MYSQL_PREPARED_H */
