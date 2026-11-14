/**
 * @file entity.h
 * @brief A rectangle that knows how to draw itself: ::lh_gl_entity_t, an area
 *        (`rect`), an owner (`parent`), a class and a list node.
 *
 * An entity is the smallest thing the graphics bricks put on a canvas. It has
 * four fields and no more: the `rect` it occupies, relative to the origin of
 * its parent; the `parent`, the layer that owns it (::lh_gl_layer_t; only a
 * layer can own an entity, and a root has none); the `cls`, a pointer to the
 * `const` ::lh_gl_entity_class_t that says what kind of entity it is and how it
 * draws (::lh_gl_entity_draw); and the `node`, an ::lh_list_node_t that links
 * the entity into its parent's list of children. An entity is in at most one
 * list, which is the same rule as having at most one parent.
 *
 * An entity knows nothing about input, styles or drivers, and nothing about an
 * operating system. Those are separate objects that act on an entity from
 * outside. Nothing here allocates: the caller owns the entity and decides where
 * it lives, so the bricks run on a target without a heap.
 *
 * A layer is an entity that also holds other entities (`lh/gl/layer.h`): it
 * starts with an ::lh_gl_entity_t, so a pointer to a layer's entity is a pointer
 * to the layer. ::lh_gl_entity_is_layer tells which an entity is, from its class.
 *
 * Fields are private. Nobody names `rect`, `parent`, `cls` or `node` outside the
 * level-1 accessors; everything else, here and in callers, goes through them.
 */

#ifndef LH_GL_ENTITY_H
#define LH_GL_ENTITY_H

#include <lh/core/list/node.h>
#include <lh/gl/entity/class.h>
#include <lh/gl/entity/fields.h>
#include <lh/gl/layer/decl.h>
#include <lh/gl/rect.h>

/**
 * @struct lh_gl_entity
 * @brief An area, its owning layer, its class and its list node. Fields via
 *        ::lh_gl_entity_fields.
 */
struct lh_gl_entity
{
    /** Members: ::lh_gl_entity_fields. */
    lh_gl_entity_fields(lh_gl_rect_t, lh_gl_layer_t, lh_gl_entity_class_t, lh_list_node_t);
};

/* -- Accessors ------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Mutable pointer to the area of @p self.
 *
 * @param self Entity to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
lh_gl_rect_t *
lh_gl_entity_get_rect_mut(lh_gl_entity_t *self);

/**
 * @brief Const pointer to the area of @p self, relative to its parent.
 *
 * @param self Entity to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
const lh_gl_rect_t *
lh_gl_entity_get_rect(const lh_gl_entity_t *self);

/**
 * @brief Replace the area of @p self with @p rect.
 *
 * Does not tell the parent: nothing is redrawn because of this call.
 *
 * @param self Entity to change; must not be null (asserted in debug builds).
 * @param rect New area, relative to the parent; copied.
 */
lh_void
lh_gl_entity_set_rect(lh_gl_entity_t *self, lh_gl_rect_t rect);

/**
 * @brief The layer that owns @p self.
 *
 * @param self Entity to read; must not be null (asserted in debug builds).
 * @return The owning layer, or ::lh_null when @p self has no owner.
 */
lh_gl_layer_t *
lh_gl_entity_get_parent(const lh_gl_entity_t *self);

/**
 * @brief Store the layer that owns @p self, without touching the layer.
 *
 * Level-1 setter: no validation. The layer's own methods
 * (::lh_gl_layer_try_add, ::lh_gl_layer_try_remove) keep the owner and the
 * layer's list in step; use them instead of this.
 *
 * @param self   Entity to change; must not be null (asserted in debug builds).
 * @param parent The owning layer, or ::lh_null for none.
 */
lh_void
lh_gl_entity_set_parent(lh_gl_entity_t *self, lh_gl_layer_t *parent);

/**
 * @brief The class of @p self.
 *
 * @param self Entity to read; must not be null (asserted in debug builds).
 * @return The class table; not null for an initialized entity.
 */
const lh_gl_entity_class_t *
lh_gl_entity_get_class(const lh_gl_entity_t *self);

/**
 * @brief Store the class of @p self.
 *
 * Level-1 setter: no validation. Changing the kind of a live entity (entity to
 * layer or back) is the caller's mistake to avoid.
 *
 * @param self Entity to change; must not be null (asserted in debug builds).
 * @param cls  The class table; must outlive @p self.
 */
lh_void
lh_gl_entity_set_class(lh_gl_entity_t *self, const lh_gl_entity_class_t *cls);

/**
 * @brief Mutable pointer to the list node of @p self.
 *
 * @param self Entity to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
lh_list_node_t *
lh_gl_entity_get_node_mut(lh_gl_entity_t *self);

/**
 * @brief Const pointer to the list node of @p self.
 *
 * @param self Entity to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
const lh_list_node_t *
lh_gl_entity_get_node(const lh_gl_entity_t *self);

/**
 * @brief The entity whose list node is @p node, or ::lh_null if @p node is.
 *
 * The inverse of ::lh_gl_entity_get_node. @p node must be the `node` field of
 * an ::lh_gl_entity_t, or ::lh_null.
 *
 * @param node A node from ::lh_gl_entity_get_node, or ::lh_null.
 */
lh_gl_entity_t *
lh_gl_entity_from_node(const lh_list_node_t *node);

/* -- Init and queries, by level -------------------------------------------- */

/**
 * @brief Fill @p self with an area and a class, no owner, and an unlinked node.
 *
 * @param self Entity to fill; must not be null (asserted in debug builds).
 * @param rect Area, relative to the parent that will own it; copied.
 * @param cls  The class table; must outlive @p self.
 */
lh_void
lh_gl_entity_init(lh_gl_entity_t *self, lh_gl_rect_t rect, const lh_gl_entity_class_t *cls);

/**
 * @brief The kind of @p self, from its class.
 *
 * @param self Entity to read; must not be null (asserted in debug builds).
 * @return One of the `lh_gl_entity_class_kind_*` constants.
 */
lh_gl_entity_class_kind_t
lh_gl_entity_get_kind(const lh_gl_entity_t *self);

/**
 * @brief True when @p self is a layer: its class says so.
 *
 * @param self Entity to read; must not be null (asserted in debug builds).
 */
lh_bool_t
lh_gl_entity_is_layer(const lh_gl_entity_t *self);

/**
 * @brief True when @p self has no owning layer.
 *
 * @param self Entity to read; must not be null (asserted in debug builds).
 */
lh_bool_t
lh_gl_entity_is_root(const lh_gl_entity_t *self);

/**
 * @brief Draw @p self on @p canvas with the draw call of its class.
 *
 * Does nothing when the class has no draw call. Draws the entity only: it does
 * not draw its parent, and does not move the canvas to the entity's origin.
 *
 * @param self   Entity to draw; must not be null (asserted in debug builds).
 * @param canvas Surface to draw on; passed to the draw call as is.
 */
lh_void
lh_gl_entity_draw(const lh_gl_entity_t *self, lh_gl_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_ENTITY_H */
