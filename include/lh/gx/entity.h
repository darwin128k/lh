/**
 * @file entity.h
 * @brief One thing on the screen: ::lh_gx_entity_t.
 *
 * gx has **one** entity type. A layer, a card, a label, a shadow are all this
 * struct; what tells them apart is the **class** — a constant table of slots
 * (how it paints, whether it covers what is under it, whether it takes input)
 * that every entity of a kind shares — and the **style**, a constant record of
 * a theme that says how it looks. A control is a composition of entities under
 * one owner: a button is a box that takes input, owning a text.
 *
 * The entity is many bricks in one record: the node (::lh_gx_node_t), the
 * place (::lh_gx_rect_t), the visibility, and three pointers it does not own —
 * the class, the style and the content (the class's own data, the
 * application's). Each brick has its own job; the entity embeds them by value,
 * so on a board one entity is one block with no pointer to follow to reach its
 * own parts.
 *
 * Ownership: gx never stores an entity. It lives wherever the application put
 * it — a field of the application's struct, a static array, an `lh_array_t`
 * reserved once — and the tree only links it (`docs/design/gx.md`, section 2.2).
 * The application detaches an entity (::lh_gx_entity_detach) before its memory
 * goes.
 *
 * **A change is reported, never remembered.** When something of an entity that
 * shows changes — it moves (::lh_gx_entity_move), it is shown or hidden
 * (::lh_gx_entity_set_shown), it is attached or detached, or the application
 * changed its look and says so (::lh_gx_entity_invalidate) — the entity reports
 * its rectangle on the screen to the root of its tree, and the root's class
 * hears it (`report` of ::lh_gx_class_t): on a renderer's screen, that is the
 * damage. Nothing in the entity is a "dirty" flag, and the entity does not know
 * what a renderer is — only the class contract.
 *
 * The level-1 setters (`set_place`, `set_visible`, …) store and report nothing;
 * they are the bricks the reporting forms are built from.
 *
 * The class and the style are declared here and defined by their own bricks;
 * the entity only carries the pointers.
 */

#ifndef LH_GX_ENTITY_H
#define LH_GX_ENTITY_H

#include <lh/gx/entity/fields.h>
#include <lh/gx/node.h>
#include <lh/gx/rect.h>

struct lh_gx_class;
struct lh_gx_style;

/**
 * @struct lh_gx_entity
 * @brief One thing on the screen: its node, place, visibility, class, style and
 *        content.
 */
struct lh_gx_entity
{
    /** Members: ::lh_gx_entity_fields. */
    lh_gx_entity_fields(lh_gx_node_t, lh_gx_rect_t, struct lh_gx_class, struct lh_gx_style);
};
/** @copybrief lh_gx_entity */
typedef struct lh_gx_entity lh_gx_entity_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The node of @p self: its place in the tree (level 1).
 *
 * @param self The entity; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_node_t *
lh_gx_entity_get_node(const lh_gx_entity_t *self);

/**
 * @brief The node of @p self, to change in place (level 1).
 *
 * @param self The entity; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_node_t *
lh_gx_entity_get_node_mut(lh_gx_entity_t *self);

/**
 * @brief Where @p self is and how big it is, relative to its owner (level 1).
 *
 * @param self The entity; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_rect_t *
lh_gx_entity_get_place(const lh_gx_entity_t *self);

/**
 * @brief Store where @p self is and how big it is, relative to its owner (level 1).
 *
 * @param self  The entity; checked by ::lh_assert_runtime_ref.
 * @param place The new place.
 */
lh_void
lh_gx_entity_set_place(lh_gx_entity_t *self, lh_gx_rect_t place);

/**
 * @brief True when @p self is drawn, hit and laid out (level 1).
 *
 * @param self The entity; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gx_entity_is_visible(const lh_gx_entity_t *self);

/**
 * @brief Store whether @p self is drawn, hit and laid out (level 1).
 *
 * @param self    The entity; checked by ::lh_assert_runtime_ref.
 * @param visible The new value.
 */
lh_void
lh_gx_entity_set_visible(lh_gx_entity_t *self, lh_bool_t visible);

/**
 * @brief The class of @p self: what it is and how it behaves (level 1).
 *
 * @param self The entity; checked by ::lh_assert_runtime_ref.
 * @return The class, or ::lh_null for an entity with none — it draws nothing and
 *         takes nothing, and only holds its children.
 */
const struct lh_gx_class *
lh_gx_entity_get_class(const lh_gx_entity_t *self);

/**
 * @brief Store the class of @p self (level 1).
 *
 * @param self  The entity; checked by ::lh_assert_runtime_ref.
 * @param klass The class; not owned, it must outlive the entity's use of it.
 */
lh_void
lh_gx_entity_set_class(lh_gx_entity_t *self, const struct lh_gx_class *klass);

/**
 * @brief The style of @p self: how it looks (level 1).
 *
 * @param self The entity; checked by ::lh_assert_runtime_ref.
 * @return The style, or ::lh_null.
 */
const struct lh_gx_style *
lh_gx_entity_get_style(const lh_gx_entity_t *self);

/**
 * @brief Store the style of @p self (level 1).
 *
 * @param self  The entity; checked by ::lh_assert_runtime_ref.
 * @param style The style; not owned, usually a record of a theme in flash.
 */
lh_void
lh_gx_entity_set_style(lh_gx_entity_t *self, const struct lh_gx_style *style);

/**
 * @brief The class's own data of @p self (level 1).
 *
 * @param self The entity; checked by ::lh_assert_runtime_ref.
 * @return What the application gave; its type is the class's to know.
 */
lh_ptr
lh_gx_entity_get_content(const lh_gx_entity_t *self);

/**
 * @brief Store the class's own data of @p self (level 1).
 *
 * @param self    The entity; checked by ::lh_assert_runtime_ref.
 * @param content The application's data; not owned.
 */
lh_void
lh_gx_entity_set_content(lh_gx_entity_t *self, lh_ptr content);

/**
 * @brief The entity whose node is @p node.
 *
 * Every node in a tree of gx is the node of an entity, so this is the way back
 * from a link to the thing on the screen.
 *
 * @param node A node, or ::lh_null.
 * @return The entity, or ::lh_null when @p node is ::lh_null.
 */
lh_gx_entity_t *
lh_gx_entity_from_node(const lh_gx_node_t *node);

/**
 * @brief An entity with no owner and no children, visible, at @p x, @p y and
 *        @p width by @p height, of @p klass and @p style, with no content.
 *
 * @param self   The entity; must not be null. Not attached: a node that was
 *               attached is detached first.
 * @param klass  The class; may be ::lh_null.
 * @param style  The style; may be ::lh_null.
 * @param x      Left edge, relative to the owner.
 * @param y      Top edge, relative to the owner.
 * @param width  Width.
 * @param height Height.
 */
lh_void
lh_gx_entity_init(lh_gx_entity_t *self, const struct lh_gx_class *klass,
                  const struct lh_gx_style *style, lh_gx_scalar_t x, lh_gx_scalar_t y,
                  lh_gx_scalar_t width, lh_gx_scalar_t height);

/**
 * @brief The owner of @p self, or ::lh_null when it is not attached.
 *
 * @param self The entity; must not be null.
 */
lh_gx_entity_t *
lh_gx_entity_get_owner(const lh_gx_entity_t *self);

/**
 * @brief The first entity @p self owns — the one at the back — or ::lh_null.
 *
 * @param self The entity; must not be null.
 */
lh_gx_entity_t *
lh_gx_entity_get_first_child(const lh_gx_entity_t *self);

/**
 * @brief The entity after @p self in its owner's children — the next one in
 *        front — or ::lh_null.
 *
 * @param self The entity; must not be null.
 */
lh_gx_entity_t *
lh_gx_entity_get_next_sibling(const lh_gx_entity_t *self);

/**
 * @brief The entity at the top of @p self's tree: @p self itself when it has no
 *        owner.
 *
 * @param self The entity; must not be null.
 */
lh_gx_entity_t *
lh_gx_entity_get_root(const lh_gx_entity_t *self);

/**
 * @brief True when @p self and every owner above it are visible — when it can be
 *        seen at all.
 *
 * @param self The entity; must not be null.
 */
lh_bool_t
lh_gx_entity_is_shown(const lh_gx_entity_t *self);

/**
 * @brief The rectangle of @p self on the screen: its place moved by the places of
 *        every owner above it.
 *
 * @param self The entity; must not be null.
 * @param out  Receives the rectangle; must not be null.
 */
lh_void
lh_gx_entity_get_screen_rect(const lh_gx_entity_t *self, lh_gx_rect_t *out);

/**
 * @brief Tell the root of @p self's tree that @p area of the screen changed.
 *
 * The root's class hears it through its `report` slot; a root whose class has
 * none — a tree that is not on a screen — hears nothing.
 *
 * @param self The entity; must not be null.
 * @param area The changed rectangle, screen coordinates.
 */
lh_void
lh_gx_entity_report(const lh_gx_entity_t *self, const lh_gx_rect_t *area);

/**
 * @brief Report that @p self looks different now, when it can be seen.
 *
 * What the application calls after it changed what the class draws — the
 * colour in the content, the text — so the rectangle is drawn again. Nothing is
 * reported for an entity that is hidden, or under a hidden owner.
 *
 * @param self The entity; must not be null.
 */
lh_void
lh_gx_entity_invalidate(const lh_gx_entity_t *self);

/**
 * @brief Move @p self to @p place, reporting where it was and where it is now.
 *
 * @param self  The entity; must not be null.
 * @param place The new place, relative to the owner.
 */
lh_void
lh_gx_entity_move(lh_gx_entity_t *self, lh_gx_rect_t place);

/**
 * @brief Show or hide @p self, reporting its rectangle when that changes what is
 *        seen.
 *
 * @param self    The entity; must not be null.
 * @param visible True to show it.
 */
lh_void
lh_gx_entity_set_shown(lh_gx_entity_t *self, lh_bool_t visible);

/**
 * @brief Make @p owner the owner of @p child, in front of its other children,
 *        and report the child's rectangle.
 *
 * The fast form (::lh_gx_node_attach): it does not check. See
 * ::lh_gx_entity_attach_v.
 *
 * @param owner The new owner; must not be null.
 * @param child The entity to attach; not attached, and not an ancestor of @p owner.
 */
lh_void
lh_gx_entity_attach(lh_gx_entity_t *owner, lh_gx_entity_t *child);

/**
 * @brief ::lh_gx_entity_attach, after checking that it keeps the tree a tree
 *        (::lh_gx_node_attach_v), and report the child's rectangle.
 *
 * @param owner The new owner; must not be null.
 * @param child The entity to attach; must not be null.
 */
lh_void
lh_gx_entity_attach_v(lh_gx_entity_t *owner, lh_gx_entity_t *child);

/**
 * @brief Report the rectangle of @p self, then take it out of its owner's
 *        children, with its own subtree.
 *
 * Call it before the memory of @p self goes. A detached entity is left as it is.
 *
 * @param self The entity; must not be null.
 */
lh_void
lh_gx_entity_detach(lh_gx_entity_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_ENTITY_H */
