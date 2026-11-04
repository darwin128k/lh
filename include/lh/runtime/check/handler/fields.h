/**
 * @file fields.h
 * @brief Member fields of ::lh_runtime_check_handler_t.
 */

#ifndef LH_RUNTIME_CHECK_HANDLER_FIELDS_H
#define LH_RUNTIME_CHECK_HANDLER_FIELDS_H

#include <lh/ptr.h>

/**
 * @def lh_runtime_check_handler_fields(fail_fn_type)
 * @brief The report call and the context it is handed.
 *
 * @param fail_fn_type Function type of the call (::lh_runtime_check_fail_fn);
 *                     the member is `fail_fn_type *fail_cb`.
 */
#define lh_runtime_check_handler_fields(fail_fn_type)                                              \
    fail_fn_type *fail_cb;                                                                         \
    lh_ptr context

#endif /* LH_RUNTIME_CHECK_HANDLER_FIELDS_H */
