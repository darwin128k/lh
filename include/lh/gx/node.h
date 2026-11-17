/**
 * @file node.h
 * @brief One place in the tree of the screen: ::lh_gx_node_t.
 *
 * The tree of gx is **intrusive**: it owns the order, never the bytes. A node
 * is the three links an entity carries inside itself — its owner, the list of
 * the nodes it owns, and its own place in its owner's list — and the entity
 * lives wherever the application put it: a field of the application's struct,
 * a static array, an `lh_array_t`. Attaching is linking, detaching is
 * unlinking; nothing here allocates, frees or moves memory, so nothing here can
 * fragment it. The links are LH's own list (::lh_list_t), so adding at the end
 * and taking a node out are constant time.
 *
 * Ownership: a node does not own the memory of its children, only their order.
 * The application keeps every node alive while it is attached, and detaches it
 * (::lh_gx_node_detach) before its memory goes — the rule of every intrusive
 * list. A node that is detached takes its own subtree with it: its children
 * stay its children.
 *
 * The order of the children is the order they are drawn in: the first is at
 * the back, the last is in front.
 *
 * The fast forms do not check; ::lh_gx_node_attach_v does, and is the one to
 * use where the tree's shape comes from data rather than from the code.
 */

#ifndef LH_GX_NODE_H
#define LH_GX_NODE_H

#include <lh/core/list.h>
#include <lh/gx/node/fields.h>

/**
 * @struct lh_gx_node
 * @brief The owner, the children, and the node's own link.
 */
struct lh_gx_node
{
    /** Members: ::lh_gx_node_fields. */
    lh_gx_node_fields(struct lh_gx_node, lh_list_t, lh_list_node_t);
};
/** @copybrief lh_gx_node */
typedef struct lh_gx_node lh_gx_node_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The owner of @p self, or ::lh_null when it is not attached (level 1).
 *
 * @param self The node; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_node_t *
lh_gx_node_get_owner(const lh_gx_node_t *self);

/**
 * @brief The owner of @p self, to change in place, or ::lh_null (level 1).
 *
 * @param self The node; checked by ::lh_assert_runtime_ref.
 */
lh_gx_node_t *
lh_gx_node_get_owner_mut(lh_gx_node_t *self);

/**
 * @brief Store the owner of @p self (level 1).
 *
 * A level-1 setter: it does not link @p self into the owner's children —
 * ::lh_gx_node_attach does both.
 *
 * @param self  The node; checked by ::lh_assert_runtime_ref.
 * @param owner The new owner; ::lh_null for none.
 */
lh_void
lh_gx_node_set_owner(lh_gx_node_t *self, lh_gx_node_t *owner);

/**
 * @brief The list of the nodes @p self owns (level 1).
 *
 * @param self The node; checked by ::lh_assert_runtime_ref.
 */
const lh_list_t *
lh_gx_node_get_children(const lh_gx_node_t *self);

/**
 * @brief The list of the nodes @p self owns, to change in place (level 1).
 *
 * @param self The node; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_list_t *
lh_gx_node_get_children_mut(lh_gx_node_t *self);

/**
 * @brief The link of @p self in its owner's children (level 1).
 *
 * @param self The node; checked by ::lh_assert_runtime_ref.
 */
const lh_list_node_t *
lh_gx_node_get_link(const lh_gx_node_t *self);

/**
 * @brief The link of @p self in its owner's children, to change in place (level 1).
 *
 * @param self The node; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_list_node_t *
lh_gx_node_get_link_mut(lh_gx_node_t *self);

/**
 * @brief The node whose link is @p entry.
 *
 * The way back from an entry of a list of children to its node.
 *
 * @param entry A link of a node, or ::lh_null.
 * @return The node, or ::lh_null when @p entry is ::lh_null.
 */
lh_gx_node_t *
lh_gx_node_from_link(const lh_list_node_t *entry);

/**
 * @brief A node with no owner and no children.
 *
 * @param self The node; must not be null. Its old links are forgotten, not
 *             undone: a node that was attached is detached first.
 */
lh_void
lh_gx_node_init(lh_gx_node_t *self);

/**
 * @brief True when @p self has an owner.
 *
 * @param self The node; must not be null.
 */
lh_bool_t
lh_gx_node_is_attached(const lh_gx_node_t *self);

/**
 * @brief True when @p self owns at least one node.
 *
 * @param self The node; must not be null.
 */
lh_bool_t
lh_gx_node_has_children(const lh_gx_node_t *self);

/**
 * @brief The first node @p self owns — the one at the back — or ::lh_null.
 *
 * @param self The node; must not be null.
 */
lh_gx_node_t *
lh_gx_node_get_first_child(const lh_gx_node_t *self);

/**
 * @brief The last node @p self owns — the one in front — or ::lh_null.
 *
 * @param self The node; must not be null.
 */
lh_gx_node_t *
lh_gx_node_get_last_child(const lh_gx_node_t *self);

/**
 * @brief The node after @p self in its owner's children, or ::lh_null when it is
 *        the last one or has no owner.
 *
 * @param self The node; must not be null.
 */
lh_gx_node_t *
lh_gx_node_get_next_sibling(const lh_gx_node_t *self);

/**
 * @brief The node before @p self in its owner's children, or ::lh_null when it is
 *        the first one or has no owner.
 *
 * @param self The node; must not be null.
 */
lh_gx_node_t *
lh_gx_node_get_prev_sibling(const lh_gx_node_t *self);

/**
 * @brief True when @p self is an owner of @p other, directly or through other
 *        owners. A node is not its own ancestor.
 *
 * @param self  The node; must not be null.
 * @param other The node to look up from; must not be null.
 */
lh_bool_t
lh_gx_node_is_ancestor_of(const lh_gx_node_t *self, const lh_gx_node_t *other);

/**
 * @brief Make @p owner the owner of @p child, in front of its other children.
 *
 * The fast form: it does not check. @p child must not be attached, and must not
 * be @p owner or an ancestor of it — otherwise the tree is broken. See
 * ::lh_gx_node_attach_v.
 *
 * @param owner The new owner; must not be null.
 * @param child The node to attach; must not be null.
 */
lh_void
lh_gx_node_attach(lh_gx_node_t *owner, lh_gx_node_t *child);

/**
 * @brief The node at the top of @p self's tree: @p self itself when it has no
 *        owner.
 *
 * @param self The node; must not be null.
 */
const lh_gx_node_t *
lh_gx_node_get_root(const lh_gx_node_t *self);

/**
 * @brief ::lh_gx_node_attach, after checking that it keeps the tree a tree.
 *
 * @p child must not be attached already, must not be @p owner, and must not be
 * an ancestor of @p owner — a node cannot own itself, directly or around a
 * loop. A violation is a checked failure (::lh_assert_runtime_if).
 *
 * @param owner The new owner; must not be null.
 * @param child The node to attach; must not be null.
 */
lh_void
lh_gx_node_attach_v(lh_gx_node_t *owner, lh_gx_node_t *child);

/**
 * @brief Take @p self out of its owner's children.
 *
 * @p self keeps its own children: a subtree is detached whole. A node that is
 * not attached is left as it is.
 *
 * @param self The node; must not be null.
 */
lh_void
lh_gx_node_detach(lh_gx_node_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_NODE_H */
