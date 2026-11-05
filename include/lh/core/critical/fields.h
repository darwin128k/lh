/**
 * @file fields.h
 * @brief Member fields of ::lh_critical_t.
 */

#ifndef LH_CRITICAL_FIELDS_H
#define LH_CRITICAL_FIELDS_H

/**
 * @def lh_critical_fields(enter_fn_type, leave_fn_type)
 * @brief The two calls of a critical section.
 *
 * @param enter_fn_type ::lh_critical_enter_fn; the member is `enter_cb`.
 * @param leave_fn_type ::lh_critical_leave_fn; the member is `leave_cb`.
 */
#define lh_critical_fields(enter_fn_type, leave_fn_type)                                           \
    enter_fn_type *enter_cb;                                                                       \
    leave_fn_type *leave_cb

#endif /* LH_CRITICAL_FIELDS_H */
