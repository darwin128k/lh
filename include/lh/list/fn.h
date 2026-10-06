/**
 * @file fn.h
 * @brief Callback signatures for ::lh_list_t.
 */

#ifndef LH_LIST_FN_H
#define LH_LIST_FN_H

#include <lh/numeric/types.h>
#include <lh/ptr.h>

struct lh_list_node;

/**
 * @typedef lh_list_cmp_fn
 * @brief Order two elements for ::lh_list_sort / ::lh_list_insert_sorted.
 *
 * @param a       An element.
 * @param b       Another element.
 * @param context What the caller passed to the list API.
 * @return Negative if @p a goes first, positive if @p b does, `0` if they
 *         are equal (their current order is then kept: the ops are stable).
 */
typedef lh_int_t(lh_list_cmp_fn)(const struct lh_list_node *a, const struct lh_list_node *b,
                                 lh_ptr context);

#endif /* LH_LIST_FN_H */
