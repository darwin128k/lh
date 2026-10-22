/**
 * @file fields.h
 * @brief Member fields of ::lh_sql_mysql_prepared_t.
 */

#ifndef LH_SQL_MYSQL_PREPARED_FIELDS_H
#define LH_SQL_MYSQL_PREPARED_FIELDS_H

/**
 * @def lh_sql_mysql_prepared_fields(text_type, list_type, result_type, row_type, lengths_type,
 *                                   flag_type)
 * @brief A statement run as text, and where its run stands.
 *
 * `sql` is the statement with its placeholders, `literals` the SQL literal
 * bound to each (an ::lh_str_t per parameter, empty when unbound), `result`
 * the client's `MYSQL_RES *` once run, `row` and `lengths` the current row,
 * `executed` whether the text was sent.
 *
 * @param text_type    Type of `sql` (::lh_str_t).
 * @param list_type    Type of `literals` (::lh_array_t of ::lh_str_t).
 * @param result_type  Type of `result` (::lh_ptr).
 * @param row_type     Type of `row` (`lh_char_t **`, a `MYSQL_ROW`).
 * @param lengths_type Type of `lengths` (`lh_ulong_t *`).
 * @param flag_type    Type of `executed` (::lh_bool_t).
 */
#define lh_sql_mysql_prepared_fields(text_type, list_type, result_type, row_type, lengths_type,    \
                                     flag_type)                                                    \
    text_type sql;                                                                                 \
    list_type literals;                                                                            \
    result_type result;                                                                            \
    row_type row;                                                                                  \
    lengths_type lengths;                                                                          \
    flag_type executed

#endif /* LH_SQL_MYSQL_PREPARED_FIELDS_H */
