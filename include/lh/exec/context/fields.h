/**
 * @file fields.h
 * @brief Member fields of ::lh_exec_context_t.
 */

#ifndef LH_EXEC_CONTEXT_FIELDS_H
#define LH_EXEC_CONTEXT_FIELDS_H

/**
 * @def lh_exec_context_fields(open_fn_type, close_fn_type, user_type)
 * @brief The open and close calls of a thread's state, and their pointer.
 *
 * @param open_fn_type  ::lh_exec_context_open_fn; the member is `open_cb`.
 * @param close_fn_type ::lh_exec_context_close_fn; the member is `close_cb`.
 * @param user_type     ::lh_ptr: `user`, handed to both calls.
 */
#define lh_exec_context_fields(open_fn_type, close_fn_type, user_type)                             \
    open_fn_type *open_cb;                                                                         \
    close_fn_type *close_cb;                                                                       \
    user_type user

#endif /* LH_EXEC_CONTEXT_FIELDS_H */
