/**
 * @file fields.h
 * @brief Member fields of ::lh_rail_executor_t.
 */

#ifndef LH_RAIL_EXECUTOR_FIELDS_H
#define LH_RAIL_EXECUTOR_FIELDS_H

/**
 * @def lh_rail_executor_fields(accept_fn_type, context_type)
 * @brief The executor's one call and its state.
 *
 * @param accept_fn_type ::lh_rail_executor_accept_fn; the member is `accept_cb`.
 * @param context_type   ::lh_ptr: `context`, the state `accept_cb` receives (a
 *                       thread executor's record; ::lh_null for the inline one).
 */
#define lh_rail_executor_fields(accept_fn_type, context_type)                                      \
    accept_fn_type *accept_cb;                                                                     \
    context_type context

#endif /* LH_RAIL_EXECUTOR_FIELDS_H */
