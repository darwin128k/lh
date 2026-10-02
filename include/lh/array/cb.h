/**
 * @file cb.h
 * @brief Function-pointer aliases for ::lh_array_fn.
 */

#ifndef LH_ARRAY_CB_H
#define LH_ARRAY_CB_H

#include <lh/array/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_array_cmp_cb
 * @brief Pointer to ::lh_array_cmp_fn.
 */
#define lh_array_cmp_cb lh_ptr_of(lh_array_cmp_fn)

/**
 * @def lh_array_pred_cb
 * @brief Pointer to ::lh_array_pred_fn.
 */
#define lh_array_pred_cb lh_ptr_of(lh_array_pred_fn)

#endif /* LH_ARRAY_CB_H */
