/**
 * @file node.h
 * @brief One link of an intrusive doubly linked list (::lh_list_node_t).
 *
 * Intrusive: the node lives inside the object it links (a field of that
 * struct), not in memory of its own — linking allocates nothing and an
 * object leaves its list in O(1) from the node alone. The object is found
 * back from its node with ::lh_list_entry (`lh/list.h`).
 *
 * A node that is in no list links to itself (::lh_list_node_init), so
 * "is it linked" is one comparison and unlinking an unlinked node is a
 * no-op.
 */

#ifndef LH_LIST_NODE_H
#define LH_LIST_NODE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/list/node/fields.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @struct lh_list_node
 * @brief `next` and `prev`. Fields via ::lh_list_node_fields.
 */
struct lh_list_node
{
    lh_list_node_fields(struct lh_list_node *);
};
typedef struct lh_list_node lh_list_node_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Unlinked node: links to itself both ways.
 */
void
lh_list_node_init(lh_list_node_t *self);

/**
 * @brief Next node in the ring (the list head after the last element).
 *
 * Raw link. To walk a list's elements, use ::lh_list_get_next, which stops
 * at the head instead of returning it.
 */
lh_list_node_t *
lh_list_node_get_next(const lh_list_node_t *self);

/**
 * @brief Previous node in the ring. Raw link, see ::lh_list_node_get_next.
 */
lh_list_node_t *
lh_list_node_get_prev(const lh_list_node_t *self);

/**
 * @brief The object @p self is embedded in, @p offset bytes before it, or
 *        ::lh_null if @p self is ::lh_null.
 *
 * The function behind ::lh_list_entry (which supplies @p offset and the
 * type); a function so the node expression is evaluated exactly once.
 */
lh_ptr
lh_list_node_get_entry(const lh_list_node_t *self, lh_usize_t offset);

/**
 * @brief True when @p self is in a list.
 */
lh_bool_t
lh_list_node_is_linked(const lh_list_node_t *self);

/**
 * @brief Link unlinked @p self right after @p pos (an element or a head).
 */
void
lh_list_node_insert_after(lh_list_node_t *self, lh_list_node_t *pos);

/**
 * @brief Link unlinked @p self right before @p pos (an element or a head).
 */
void
lh_list_node_insert_before(lh_list_node_t *self, lh_list_node_t *pos);

/**
 * @brief Move @p self (linked or not) to right after @p pos, in O(1).
 *
 * @p pos may be in another list, or be a list head (after the head =
 * first). Moving a node next to itself is a no-op.
 */
void
lh_list_node_move_after(lh_list_node_t *self, lh_list_node_t *pos);

/**
 * @brief Move @p self (linked or not) to right before @p pos, in O(1).
 *
 * Before a list head = last. See ::lh_list_node_move_after.
 */
void
lh_list_node_move_before(lh_list_node_t *self, lh_list_node_t *pos);

/**
 * @brief Take @p self out of its list in O(1) and leave it unlinked.
 *
 * No-op on an unlinked node.
 */
void
lh_list_node_unlink(lh_list_node_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_LIST_NODE_H */
