/**
 * @file entity.h
 * @brief Entity: the node of lh's scene / UI tree (the LVGL `lv_obj` model).
 *
 * Everything on screen or in a scene is an entity, and an entity may contain
 * other entities: a window holds panels, a panel holds buttons, a button holds
 * its label. What an entity is and does comes from its class
 * (::lh_entity_class_t); classes derive from one another by embedding fields.
 *
 * This core knows nothing about space or paint. A 2D entity
 * (::lh_entity_2d_t) adds a place, a box and a style and is what gets
 * drawn; a 3D one extends that place into space. The tree, events and
 * lifetime work the same for all of them.
 *
 * Lifetime: every entity is a block of an ownership tree (`lh/memory/tree.h`),
 * a child of its parent entity. Deleting an entity deletes its children;
 * memory an entity allocates under itself with ::lh_memory_tree_alloc_child
 * goes with it too.
 *
 * Events (::lh_entity_send_event) reach the entity's classes (derived first),
 * then its handlers in the order they were added; with
 * ::lh_entity_flags_event_bubble they continue to the parent, and so on up,
 * until a receiver calls ::lh_entity_event_stop.
 */

#ifndef LH_ENTITY_H
#define LH_ENTITY_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/cb.h>
#include <lh/entity/class.h>
#include <lh/entity/event.h>
#include <lh/entity/fields.h>
#include <lh/entity/flags.h>
#include <lh/list.h>
#include <lh/memory/sized/allocator.h>
#include <lh/numeric/types.h>
#include <lh/ptr.h>

/**
 * @struct lh_entity
 * @brief Fields via ::lh_entity_fields.
 */
struct lh_entity
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
};
typedef struct lh_entity lh_entity_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Create a root entity of class @p entity_class: the top of a tree,
 *        such as a screen or a scene.
 *
 * @param entity_class Class of the new entity (not ::lh_null).
 * @param allocator    Sized allocator the whole tree is allocated from (not
 *                     ::lh_null; must outlive the tree).
 * @return The new entity, zeroed and set up by its classes' constructors.
 */
lh_entity_t *
lh_entity_create_root(const lh_entity_class_t *entity_class,
                      lh_memory_sized_allocator_t *allocator);

/**
 * @brief Create an entity of class @p entity_class as @p parent's youngest
 *        child.
 *
 * @param entity_class Class of the new entity (not ::lh_null).
 * @param parent       Owner (not ::lh_null).
 * @return The new entity, zeroed and set up by its classes' constructors.
 */
lh_entity_t *
lh_entity_create(const lh_entity_class_t *entity_class, lh_entity_t *parent);

/**
 * @brief Delete @p self with all its children (::lh_null is ignored).
 *
 * @p self receives ::LH_ENTITY_EVENT_DELETE while still in its tree, then
 * its classes' destructors run (derived first), then its children are
 * deleted the same way, oldest first, then its memory is freed.
 */
lh_void
lh_entity_delete(lh_entity_t *self);

/**
 * @brief The class @p self was created with.
 */
const lh_entity_class_t *
lh_entity_get_class(const lh_entity_t *self);

/**
 * @brief True when @p self's class is @p entity_class or derives from it,
 *        i.e. @p self may be used as an instance of @p entity_class.
 */
lh_bool_t
lh_entity_is_instance_of(const lh_entity_t *self, const lh_entity_class_t *entity_class);

/**
 * @brief @p self as an instance of @p entity_class, or ::lh_null when it is
 *        not one (::lh_entity_is_instance_of).
 *
 * The checked way down a class hierarchy: assign the result to a pointer to
 * the class's instance struct and test it.
 * @code{.c}
 * const lh_entity_2d_t *box = lh_entity_cast(entity, &lh_entity_2d_class);
 * if (box) { ... }
 * @endcode
 */
lh_ptr
lh_entity_cast(const lh_entity_t *self, const lh_entity_class_t *entity_class);

/**
 * @brief The entity that contains @p self, or ::lh_null for a root.
 */
lh_entity_t *
lh_entity_get_parent(const lh_entity_t *self);

/**
 * @brief Move @p self (with its children) to the end of @p parent's
 *        children, or make it a root with ::lh_null.
 *
 * @param parent Not @p self nor one of its descendants.
 */
lh_void
lh_entity_set_parent(lh_entity_t *self, lh_entity_t *parent);

/**
 * @brief @p self's oldest child entity, or ::lh_null.
 */
lh_entity_t *
lh_entity_get_first_child(const lh_entity_t *self);

/**
 * @brief The next younger child of @p self's parent, or ::lh_null after the
 *        youngest (and for a root).
 */
lh_entity_t *
lh_entity_get_next_sibling(const lh_entity_t *self);

/**
 * @def lh_entity_foreach_child(child, parent)
 * @brief Loop over @p parent's children, oldest first, with each in turn in
 *        the new `lh_entity_t *` variable @p child.
 *
 * The body must not delete or move @p child.
 */
#define lh_entity_foreach_child(child, parent)                                                     \
    for (lh_entity_t *child = lh_entity_get_first_child(parent); lh_ptr_is_set(child);             \
         child = lh_entity_get_next_sibling(child))

/**
 * @brief The top of @p self's tree: the ancestor with no parent, or @p self
 *        when it has none (a screen, a scene).
 */
lh_entity_t *
lh_entity_get_root(const lh_entity_t *self);

/**
 * @brief Set the `lh_entity_flags_*` bits of @p flags on @p self.
 */
lh_void
lh_entity_add_flags(lh_entity_t *self, lh_entity_flags_t flags);

/**
 * @brief Clear the `lh_entity_flags_*` bits of @p flags on @p self.
 */
lh_void
lh_entity_clear_flags(lh_entity_t *self, lh_entity_flags_t flags);

/**
 * @brief True when every bit of @p flags is set on @p self.
 */
lh_bool_t
lh_entity_has_flags(const lh_entity_t *self, lh_entity_flags_t flags);

/**
 * @brief Call @p handler with @p user_data for every event reaching @p self,
 *        after its classes and the handlers added before.
 *
 * The same pair may be added more than once; it is then called that many
 * times. Handlers are freed with the entity.
 */
lh_void
lh_entity_add_handler(lh_entity_t *self, lh_entity_handler_cb handler, lh_ptr user_data);

/**
 * @brief Remove the oldest @p handler / @p user_data pair added to @p self.
 *
 * A handler may remove itself while it runs, but no other handler.
 *
 * @return False when there was no such pair.
 */
lh_bool_t
lh_entity_remove_handler(lh_entity_t *self, lh_entity_handler_cb handler, lh_ptr user_data);

/**
 * @brief Send the event @p code with @p param to @p self.
 *
 * Returns once every receiver ran: @p self's classes and handlers, then,
 * while each receiver has ::lh_entity_flags_event_bubble and no one called
 * ::lh_entity_event_stop, its parent's.
 *
 * @return True when a receiver stopped the event.
 */
lh_bool_t
lh_entity_send_event(lh_entity_t *self, lh_uint_t code, lh_ptr param);

/**
 * @brief Send the event @p code with @p param to @p self alone: its classes
 *        and handlers, never its ancestors (whatever the bubble flag).
 *
 * For events about one entity only, such as ::LH_ENTITY_EVENT_DRAW.
 *
 * @return True when a receiver stopped the event.
 */
lh_bool_t
lh_entity_notify(lh_entity_t *self, lh_uint_t code, lh_ptr param);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_H */
