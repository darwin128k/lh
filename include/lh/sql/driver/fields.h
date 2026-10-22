/**
 * @file fields.h
 * @brief Member fields of ::lh_sql_driver_t.
 */

#ifndef LH_SQL_DRIVER_FIELDS_H
#define LH_SQL_DRIVER_FIELDS_H

/**
 * @def lh_sql_driver_fields(name_type, close_fn, exec_fn, last_insert_id_fn, prepare_fn,
 *                           finalize_fn, bind_null_fn, bind_sint_fn, bind_str_fn, step_fn,
 *                           reset_fn, column_count_fn, column_is_null_fn, column_sint_fn,
 *                           column_str_fn)
 * @brief A backend's name and the function behind each database operation.
 *
 * Every slot is required: lh/sql calls them without checking.
 */
#define lh_sql_driver_fields(name_type, close_fn, exec_fn, last_insert_id_fn, prepare_fn,          \
                             finalize_fn, bind_null_fn, bind_sint_fn, bind_str_fn, step_fn,        \
                             reset_fn, column_count_fn, column_is_null_fn, column_sint_fn,         \
                             column_str_fn)                                                        \
    name_type name;                                                                                \
    close_fn *close;                                                                               \
    exec_fn *exec;                                                                                 \
    last_insert_id_fn *last_insert_id;                                                             \
    prepare_fn *prepare;                                                                           \
    finalize_fn *finalize;                                                                         \
    bind_null_fn *bind_null;                                                                       \
    bind_sint_fn *bind_sint;                                                                       \
    bind_str_fn *bind_str;                                                                         \
    step_fn *step;                                                                                 \
    reset_fn *reset;                                                                               \
    column_count_fn *column_count;                                                                 \
    column_is_null_fn *column_is_null;                                                             \
    column_sint_fn *column_sint;                                                                   \
    column_str_fn *column_str

#endif /* LH_SQL_DRIVER_FIELDS_H */
