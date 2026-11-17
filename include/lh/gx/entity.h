/**
 * @file entity.h
 * @brief One thing on the screen: ::lh_gx_entity_t.
 *
 * An entity has a class, a place, a way of looking, what it says, its state
 * flags and the links to its parent and its children. It is a plain value:
 * the application owns it, it is copied when it is passed, and no part of gx
 * allocates one or frees one.
 *
 * **An entity never remembers that it is invalid.** When something of it
 * changes, it reports where it was and where it is now
 * (::lh_gx_entity_damage_fn), and the canvas — which is what remembers —
 * collects the places that need painting again. A layer is the same entity:
 * one whose class holds children.
 *
 * The links are indices into the tree's array, not pointers: a child is a
 * number, the tree owns the array, and every entity is the same size whatever
 * it is.
 *
 * @see lh_gx_entity_kind_t for the classes, lh_gx_damage_t for what the
 *      canvas remembers.
 */

#ifndef LH_GX_ENTITY_H
#define LH_GX_ENTITY_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/str/view.h>
#include <lh/core/void.h>
#include <lh/gx/entity/damage/cb.h>
#include <lh/gx/entity/fields.h>
#include <lh/gx/entity/kind.h>
#include <lh/gx/rect.h>

/**
 * @def LH_GX_ENTITY_FLAG_VISIBLE
 * @brief The entity is drawn and hit. An entity without this flag is skipped
 *        by the passes and by the hit test.
 */
#define LH_GX_ENTITY_FLAG_VISIBLE (1U << 0)

/**
 * @def LH_GX_ENTITY_FLAG_ENABLED
 * @brief The entity takes input. A disabled entity is drawn as the theme says
 *        and takes no clicks.
 */
#define LH_GX_ENTITY_FLAG_ENABLED (1U << 1)

/**
 * @def LH_GX_ENTITY_FLAG_INVALID
 * @brief The entity's state is wrong — a field with a bad value, a number out
 *        of its range. The theme says how such an entity looks.
 */
#define LH_GX_ENTITY_FLAG_INVALID (1U << 2)

/**
 * @def LH_GX_ENTITY_FLAG_HOVERED
 * @brief The pointer is over the entity.
 */
#define LH_GX_ENTITY_FLAG_HOVERED (1U << 3)

/**
 * @def LH_GX_ENTITY_FLAG_PRESSED
 * @brief The entity is being pressed.
 */
#define LH_GX_ENTITY_FLAG_PRESSED (1U << 4)

/**
 * @def LH_GX_ENTITY_FLAG_FOCUSED
 * @brief The entity has the keyboard focus.
 */
#define LH_GX_ENTITY_FLAG_FOCUSED (1U << 5)

/**
 * @def LH_GX_ENTITY_FLAG_CHECKED
 * @brief The entity is checked — a toggle, a box.
 */
#define LH_GX_ENTITY_FLAG_CHECKED (1U << 6)

/**
 * @def LH_GX_ENTITY_NO_LINK
 * @brief The index of no entity: no parent, no first child, no next sibling.
 */
#define LH_GX_ENTITY_NO_LINK 0xFFFFFFFFU

/**
 * @struct lh_gx_entity
 * @brief One thing on the screen.
 */
struct lh_gx_entity
{
    /** Members: ::lh_gx_entity_fields. */
    lh_gx_entity_fields(lh_gx_entity_kind_t, lh_u32_t, lh_gx_rect_t, lh_u16_t, lh_str_view_t,
                        lh_u32_t, lh_gx_entity_damage_cb, lh_ptr, lh_ptr);
};
/** @copybrief lh_gx_entity */
typedef struct lh_gx_entity lh_gx_entity_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with an entity of @p kind, empty of everything else.
 *
 * @param self The entity; must not be null.
 * @param kind The class of the entity.
 */
lh_void
lh_gx_entity_init(lh_gx_entity_t *self, lh_gx_entity_kind_t kind);

/**
 * @brief Report to the listener that @p self changed from @p before to @p after.
 *
 * The call of @p self is made with its own context. An entity with no
 * listener reports nothing.
 *
 * @param self   The entity; must not be null.
 * @param before Where the entity was; must not be null.
 * @param after  Where the entity is now; must not be null.
 */
lh_void
lh_gx_entity_report(const lh_gx_entity_t *self, const lh_gx_rect_t *before,
                    const lh_gx_rect_t *after);

/**
 * @brief The class of @p self.
 *
 * @param self The entity; must not be null.
 * @return The class.
 */
lh_gx_entity_kind_t
lh_gx_entity_get_kind(const lh_gx_entity_t *self);

/**
 * @brief Store the class of @p self.
 *
 * The place of the entity does not change, but what it is does, so the place
 * is reported as changed.
 *
 * @param self The entity; must not be null.
 * @param kind The new class.
 */
lh_void
lh_gx_entity_set_kind(lh_gx_entity_t *self, lh_gx_entity_kind_t kind);

/**
 * @brief The flags of @p self.
 *
 * @param self The entity; must not be null.
 * @return The flags.
 */
lh_u32_t
lh_gx_entity_get_flags(const lh_gx_entity_t *self);

/**
 * @brief Store the flags of @p self.
 *
 * The place of the entity does not change, but how it looks may, so the place
 * is reported as changed.
 *
 * @param self  The entity; must not be null.
 * @param flags The new flags.
 */
lh_void
lh_gx_entity_set_flags(lh_gx_entity_t *self, lh_u32_t flags);

/**
 * @brief The place of @p self on the screen.
 *
 * @param self The entity; must not be null.
 * @return The rectangle.
 */
const lh_gx_rect_t *
lh_gx_entity_get_rect(const lh_gx_entity_t *self);

/**
 * @brief Store the place of @p self on the screen.
 *
 * Both the old place and the new one are reported as changed.
 *
 * @param self The entity; must not be null.
 * @param rect The new rectangle.
 */
lh_void
lh_gx_entity_set_rect(lh_gx_entity_t *self, const lh_gx_rect_t *rect);

/**
 * @brief The style of @p self: an index into the theme.
 *
 * @param self The entity; must not be null.
 * @return The index.
 */
lh_u16_t
lh_gx_entity_get_style(const lh_gx_entity_t *self);

/**
 * @brief Store the style of @p self.
 *
 * The place of the entity does not change, but how it looks may, so the place
 * is reported as changed.
 *
 * @param self  The entity; must not be null.
 * @param style The new index.
 */
lh_void
lh_gx_entity_set_style(lh_gx_entity_t *self, lh_u16_t style);

/**
 * @brief What @p self says.
 *
 * @param self The entity; must not be null.
 * @return The text; borrowed from whoever owns it.
 */
const lh_str_view_t *
lh_gx_entity_get_text(const lh_gx_entity_t *self);

/**
 * @brief Store what @p self says.
 *
 * The place of the entity does not change, but how it looks may, so the place
 * is reported as changed.
 *
 * @param self The entity; must not be null.
 * @param text The new text; borrowed by the entity.
 */
lh_void
lh_gx_entity_set_text(lh_gx_entity_t *self, const lh_str_view_t *text);

/**
 * @brief Store what @p self says, from a null-terminated string.
 *
 * The text is borrowed: the string must outlive the entity.
 *
 * @param self The entity; must not be null.
 * @param text The new text; borrowed by the entity.
 */
lh_void
lh_gx_entity_set_text_cstr(lh_gx_entity_t *self, const lh_str_ptr text);

/**
 * @brief The parent of @p self in the tree.
 *
 * @param self The entity; must not be null.
 * @return The index of the parent, or `LH_GX_ENTITY_NO_LINK` for a root.
 */
lh_u32_t
lh_gx_entity_get_parent(const lh_gx_entity_t *self);

/**
 * @brief The first child of @p self in the tree.
 *
 * @param self The entity; must not be null.
 * @return The index of the first child, or `LH_GX_ENTITY_NO_LINK` when there
 *         is none.
 */
lh_u32_t
lh_gx_entity_get_first_child(const lh_gx_entity_t *self);

/**
 * @brief How many children @p self has.
 *
 * @param self The entity; must not be null.
 * @return The number of children.
 */
lh_u32_t
lh_gx_entity_get_child_count(const lh_gx_entity_t *self);

/**
 * @brief The next sibling of @p self in the tree.
 *
 * @param self The entity; must not be null.
 * @return The index of the next sibling, or `LH_GX_ENTITY_NO_LINK` when there
 *         is none.
 */
lh_u32_t
lh_gx_entity_get_next(const lh_gx_entity_t *self);

/**
 * @brief The call that hears about a change of @p self.
 *
 * @param self The entity; must not be null.
 * @return The call, or ::lh_null when nobody listens.
 */
lh_gx_entity_damage_cb
lh_gx_entity_get_damage_fn(const lh_gx_entity_t *self);

/**
 * @brief Store the call that hears about a change of @p self.
 *
 * @param self The entity; must not be null.
 * @param fn   The call; may be ::lh_null.
 */
lh_void
lh_gx_entity_set_damage_fn(lh_gx_entity_t *self, lh_gx_entity_damage_cb fn);

/**
 * @brief The state the call of @p self gets.
 *
 * @param self The entity; must not be null.
 * @return The listener's own state.
 */
lh_ptr
lh_gx_entity_get_damage_context(const lh_gx_entity_t *self);

/**
 * @brief Store the state the call of @p self gets.
 *
 * @param self    The entity; must not be null.
 * @param context The listener's own state.
 */
lh_void
lh_gx_entity_set_damage_context(lh_gx_entity_t *self, lh_ptr context);

/**
 * @brief The application's own pointer to its own state.
 *
 * @param self The entity; must not be null.
 * @return The pointer.
 */
lh_ptr
lh_gx_entity_get_user(const lh_gx_entity_t *self);

/**
 * @brief Store the application's own pointer to its own state.
 *
 * @param self The entity; must not be null.
 * @param user The pointer; borrowed by the entity.
 */
lh_void
lh_gx_entity_set_user(lh_gx_entity_t *self, lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_ENTITY_H */
