/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_os_controller_t.
 */

#ifndef LH_OS_CONTROLLER_FIELDS_H
#define LH_OS_CONTROLLER_FIELDS_H

/**
 * @def lh_os_controller_fields(handle_type, update_type, cancel_type, context_type)
 * @brief Expands to the three handler pointers, the context handed to each of
 *        them, the gesture recognizer and the flags, in this order.
 *
 * @param handle_type  Function type of the event handler
 *                     (::lh_os_controller_handlers_handle_fn); the member is
 *                     `handle_type *handle_cb`.
 * @param update_type  Function type of the timer handler
 *                     (::lh_os_controller_handlers_update_fn); the member is
 *                     `update_type *update_cb`, null for none.
 * @param cancel_type  Function type of the cancel handler
 *                     (::lh_os_controller_handlers_cancel_fn); the member is
 *                     `cancel_type *cancel_cb`, null for none.
 * @param context_type Type of `context`, the object the handlers work on, handed
 *                     to each of them as `self` (::lh_self_ptr).
 *
 * `recognizer` is an ::lh_os_input_recognizer_t and `flags` an
 * ::lh_os_controller_flags_t; the file that expands the macro includes both.
 *
 * Example usage:
 * @code{.c}
 * struct lh_os_controller
 * {
 *     lh_os_controller_fields(lh_os_controller_handlers_handle_fn,
 *                             lh_os_controller_handlers_update_fn,
 *                             lh_os_controller_handlers_cancel_fn, lh_self_ptr);
 * };
 * @endcode
 *
 * @see lh_os_controller_t
 */
#define lh_os_controller_fields(handle_type, update_type, cancel_type, context_type)               \
    handle_type *handle_cb;                                                                        \
    update_type *update_cb;                                                                        \
    cancel_type *cancel_cb;                                                                        \
    context_type context;                                                                          \
    lh_os_input_recognizer_t recognizer;                                                           \
    lh_os_controller_flags_t flags

#endif /* LH_OS_CONTROLLER_FIELDS_H */
