/**
 * @file cb.h
 * @brief Function-pointer aliases for ::lh_list_fn.
 */

#ifndef LH_LIST_CB_H
#define LH_LIST_CB_H

#include <lh/list/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_list_cmp_cb
 * @brief Pointer to ::lh_list_cmp_fn.
 */
#define lh_list_cmp_cb lh_ptr_of(lh_list_cmp_fn)

#endif /* LH_LIST_CB_H */
