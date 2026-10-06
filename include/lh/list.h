/**
 * @file list.h
 * @brief Intrusive doubly linked list (::lh_list_t) of ::lh_list_node_t.
 *
 * The list owns nothing: it only links nodes that live inside the caller's
 * objects. Adding, removing (::lh_list_node_unlink) and taking from either
 * end are O(1) and allocate nothing; an element never moves, so a pointer
 * to it stays valid for as long as the caller keeps the object.
 *
 * Same shape as the Linux kernel's `list_head`: a ring through a head node
 * that is not an element. Walking stops at the head — the getters return
 * ::lh_null there — so a loop needs no macro:
 * @code{.c}
 * for (lh_list_node_t *n = lh_list_get_first(&list); n; n = lh_list_get_next(&list, n))
 * {
 *     item_t *item = lh_list_entry(item_t, node, n);
 * }
 * @endcode
 * To remove while walking, read the next node before unlinking the current one.
 */

#ifndef LH_LIST_H
#define LH_LIST_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/index.h>
#include <lh/index/limits.h>
#include <lh/list/cb.h>
#include <lh/list/fields.h>
#include <lh/list/node.h>
#include <lh/null.h>
#include <lh/size.h>
#include <lh/util/offset.h>
#include <lh/util/ptr.h>

/**
 * @def LH_LIST_INVALID
 * @brief What ::lh_list_index_of returns for a node that is not in the list.
 */
#define LH_LIST_INVALID LH_UINDEX_T_MAX

/**
 * @struct lh_list
 * @brief The head node. Fields via ::lh_list_fields.
 */
struct lh_list
{
    lh_list_fields(lh_list_node_t);
};
typedef struct lh_list lh_list_t;

/**
 * @def lh_list_entry(T, member, node)
 * @brief The `T` whose field @p member is @p node, or ::lh_null if @p node is.
 *
 * @param T      Type of the object the node lives in.
 * @param member Name of the ::lh_list_node_t field in `T`.
 * @param node   Node pointer (may be ::lh_null, e.g. from ::lh_list_get_first).
 *               Evaluated once, so `lh_list_entry(T, m, lh_list_pop_back(&l))`
 *               pops one element.
 */
#define lh_list_entry(T, member, node)                                                             \
    lh_ptr_cast(T, lh_list_node_get_entry(node, lh_offset_of(T, member)))

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty list.
 */
void
lh_list_init(lh_list_t *self);

/**
 * @brief True when @p self has no elements.
 */
lh_bool_t
lh_list_is_empty(const lh_list_t *self);

/**
 * @brief Number of elements. O(n): the list keeps no count.
 */
lh_usize_t
lh_list_get_size(const lh_list_t *self);

/**
 * @brief First element, or ::lh_null if @p self is empty.
 */
lh_list_node_t *
lh_list_get_first(const lh_list_t *self);

/**
 * @brief Last element, or ::lh_null if @p self is empty.
 */
lh_list_node_t *
lh_list_get_last(const lh_list_t *self);

/**
 * @brief Element number @p index from the front (0 is the first), or
 *        ::lh_null if @p self has no such element.
 *
 * O(@p index): a linked list has no random access, so this walks from the
 * front. For "the third one" that is fine; to visit every element use
 * ::lh_list_get_first / ::lh_list_get_next — a loop over `get_at(i)` is
 * O(n²).
 */
lh_list_node_t *
lh_list_get_at(const lh_list_t *self, lh_uindex_t index);

/**
 * @brief Element after @p node, or ::lh_null if @p node is the last.
 *
 * @param node An element of @p self.
 */
lh_list_node_t *
lh_list_get_next(const lh_list_t *self, const lh_list_node_t *node);

/**
 * @brief Element before @p node, or ::lh_null if @p node is the first.
 *
 * @param node An element of @p self.
 */
lh_list_node_t *
lh_list_get_prev(const lh_list_t *self, const lh_list_node_t *node);

/**
 * @brief Position of @p node from the front (0 is the first), or
 *        ::LH_LIST_INVALID if @p node is not an element of @p self. O(n).
 */
lh_uindex_t
lh_list_index_of(const lh_list_t *self, const lh_list_node_t *node);

/**
 * @brief True when @p node is an element of @p self. O(n).
 */
lh_bool_t
lh_list_contains(const lh_list_t *self, const lh_list_node_t *node);

/**
 * @brief Make unlinked @p node the first element.
 */
void
lh_list_push_front(lh_list_t *self, lh_list_node_t *node);

/**
 * @brief Make unlinked @p node the last element.
 */
void
lh_list_push_back(lh_list_t *self, lh_list_node_t *node);

/**
 * @brief Insert unlinked @p node keeping @p self ordered by @p cmp.
 *
 * Same order as ::lh_list_sort: negative means @p node goes before an
 * existing element. Equal keys stay stable — @p node is placed after them.
 * O(n).
 *
 * @param cmp     Order of two elements.
 * @param context Passed to every @p cmp call.
 */
void
lh_list_insert_sorted(lh_list_t *self, lh_list_node_t *node, lh_list_cmp_cb cmp, lh_ptr context);

/**
 * @brief Move every element of @p other, in order, to the end of @p self.
 *
 * O(1): the two rings are joined, no element is visited. @p other ends up
 * empty. @p other must not be @p self.
 */
void
lh_list_splice_back(lh_list_t *self, lh_list_t *other);

/**
 * @brief Move every element of @p other, in order, to the front of @p self.
 *        O(1); see ::lh_list_splice_back.
 */
void
lh_list_splice_front(lh_list_t *self, lh_list_t *other);

/**
 * @brief Unlink every element. Each node is left unlinked; the objects
 *        themselves are untouched. O(n).
 */
void
lh_list_clear(lh_list_t *self);

/**
 * @brief Sort the elements by @p cmp: stable, O(n log n), allocates nothing.
 *
 * Bottom-up merge sort over the links themselves. Pointers to elements stay
 * valid; only their order changes.
 *
 * @param cmp     Order of two elements.
 * @param context Passed to every @p cmp call.
 */
void
lh_list_sort(lh_list_t *self, lh_list_cmp_cb cmp, lh_ptr context);

/**
 * @brief Unlink and return the first element, or ::lh_null if empty.
 */
lh_list_node_t *
lh_list_pop_front(lh_list_t *self);

/**
 * @brief Unlink and return the last element, or ::lh_null if empty.
 */
lh_list_node_t *
lh_list_pop_back(lh_list_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_LIST_H */
