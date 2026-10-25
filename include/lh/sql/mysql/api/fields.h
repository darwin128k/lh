/**
 * @file fields.h
 * @brief Member fields of ::lh_sql_mysql_api_t.
 */

#ifndef LH_SQL_MYSQL_API_FIELDS_H
#define LH_SQL_MYSQL_API_FIELDS_H

/**
 * @def lh_sql_mysql_api_fields(image_type, init_fn, real_connect_fn, close_fn, error_fn,
 *                              error_code_fn, real_query_fn, store_result_fn, free_result_fn,
 *                              fetch_row_fn, fetch_lengths_fn, num_fields_fn, field_count_fn,
 *                              insert_id_fn, real_escape_string_fn, set_character_set_fn)
 * @brief The client library and the C API functions found in it, one slot
 *        per `mysql_*` function lh/sql uses.
 */
#define lh_sql_mysql_api_fields(image_type, init_fn, real_connect_fn, close_fn, error_fn,          \
                                error_code_fn, real_query_fn, store_result_fn, free_result_fn,     \
                                fetch_row_fn, fetch_lengths_fn, num_fields_fn, field_count_fn,     \
                                insert_id_fn, real_escape_string_fn, set_character_set_fn)         \
    image_type image;                                                                              \
    init_fn *init;                                                                                 \
    real_connect_fn *real_connect;                                                                 \
    close_fn *close;                                                                               \
    error_fn *error;                                                                               \
    error_code_fn *error_code;                                                                     \
    real_query_fn *real_query;                                                                  \
    store_result_fn *store_result;                                                                 \
    free_result_fn *free_result;                                                                   \
    fetch_row_fn *fetch_row;                                                                       \
    fetch_lengths_fn *fetch_lengths;                                                               \
    num_fields_fn *num_fields;                                                                     \
    field_count_fn *field_count;                                                                   \
    insert_id_fn *insert_id;                                                                       \
    real_escape_string_fn *real_escape_string;                                                     \
    set_character_set_fn *set_character_set

#endif /* LH_SQL_MYSQL_API_FIELDS_H */
