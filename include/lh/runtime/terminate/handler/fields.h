/**
 * @file fields.h
 * @brief Member fields of ::lh_runtime_terminate_handler_t.
 */

#ifndef LH_RUNTIME_TERMINATE_HANDLER_FIELDS_H
#define LH_RUNTIME_TERMINATE_HANDLER_FIELDS_H

#include <lh/ptr.h>

/**
 * @def lh_runtime_terminate_handler_fields(call_fn_type)
 * @brief The call and the context it is handed.
 *
 * @param call_fn_type Function type of the call (::lh_runtime_terminate_fn);
 *                     the member is `call_fn_type *call_cb`.
 */
#define lh_runtime_terminate_handler_fields(call_fn_type)                                          \
    call_fn_type *call_cb;                                                                         \
    lh_ptr context

#endif /* LH_RUNTIME_TERMINATE_HANDLER_FIELDS_H */
