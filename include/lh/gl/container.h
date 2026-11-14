/**
 * @file container.h
 * @brief A layer plus up to two scroll axes: ::lh_gl_container_t.
 *
 * The container borrows a layer and, optionally, a horizontal axis and a
 * vertical axis. It does not own them and it does not write their rectangles
 * or the children's. A positive axis offset moves that axis of the content
 * toward the start: the window stays where the layer rectangle is, and the
 * children are placed at the window origin minus the offsets.
 *
 * Either axis pointer may be null. Nested layers are placed as whole
 * rectangles; a child layer's own container is applied only when that
 * container is drawn or searched itself.
 */

#ifndef LH_GL_CONTAINER_H
#define LH_GL_CONTAINER_H

#include <lh/gl/canvas/decl.h>
#include <lh/gl/container/fields.h>
#include <lh/gl/entity/decl.h>
#include <lh/gl/layer/decl.h>
#include <lh/gl/rect.h>
#include <lh/gl/scroll/axis.h>

/**
 * @struct lh_gl_container
 * @brief Borrowed layer and borrowed axes. Fields via ::lh_gl_container_fields.
 */
struct lh_gl_container
{
    /** Members: ::lh_gl_container_fields. */
    lh_gl_container_fields(lh_gl_layer_t, lh_gl_scroll_axis_t);
};
/** @copybrief lh_gl_container */
typedef struct lh_gl_container lh_gl_container_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Borrowed layer, or ::lh_null. */
lh_gl_layer_t *
lh_gl_container_get_layer(const lh_gl_container_t *self);

/** @brief Store a borrowed layer. @p layer may be ::lh_null. */
lh_void
lh_gl_container_set_layer(lh_gl_container_t *self, lh_gl_layer_t *layer);

/** @brief Borrowed horizontal axis, or ::lh_null. */
const lh_gl_scroll_axis_t *
lh_gl_container_get_horizontal(const lh_gl_container_t *self);

/** @brief Store a borrowed horizontal axis. @p horizontal may be ::lh_null. */
lh_void
lh_gl_container_set_horizontal(lh_gl_container_t *self, const lh_gl_scroll_axis_t *horizontal);

/** @brief Borrowed vertical axis, or ::lh_null. */
const lh_gl_scroll_axis_t *
lh_gl_container_get_vertical(const lh_gl_container_t *self);

/** @brief Store a borrowed vertical axis. @p vertical may be ::lh_null. */
lh_void
lh_gl_container_set_vertical(lh_gl_container_t *self, const lh_gl_scroll_axis_t *vertical);

/**
 * @brief Borrow @p layer and the axes. Either axis may be ::lh_null.
 *
 * None of the pointers is owned.
 */
lh_void
lh_gl_container_init(lh_gl_container_t *self, lh_gl_layer_t *layer,
                     const lh_gl_scroll_axis_t *horizontal, const lh_gl_scroll_axis_t *vertical);

/**
 * @brief Window origin minus the axis offsets.
 *
 * A null axis subtracts nothing. @p out may be @p viewport.
 *
 * @param self     Container; must not be null.
 * @param viewport Window origin, copied from; must not be null.
 * @param out      Receives the content origin; must not be null.
 */
lh_void
lh_gl_container_content_origin(const lh_gl_container_t *self, const lh_gl_point_t *viewport,
                               lh_gl_point_t *out);

/**
 * @brief Entity under @p point inside this container's window.
 *
 * Uses the layer's absolute rectangle as the window and
 * ::lh_gl_container_content_origin as the child placement. A null layer
 * returns ::lh_null. Children's rectangles are not changed.
 *
 * @param self  Container; must not be null.
 * @param point Point in root coordinates, copied.
 * @return The entity under the point, or ::lh_null.
 */
const lh_gl_entity_t *
lh_gl_container_find(const lh_gl_container_t *self, lh_gl_point_t point);

/**
 * @brief Draw this container's layer, with children placed by the axes.
 *
 * A null layer returns without drawing. The clip is the dirty area in root
 * coordinates. Children's rectangles are not changed.
 *
 * @param self   Container; must not be null.
 * @param canvas Drawing state, borrowed; must not be null.
 * @param clip   Dirty area; must not be null.
 */
lh_void
lh_gl_container_draw(const lh_gl_container_t *self, lh_gl_canvas_t *canvas,
                     const lh_gl_rect_t *clip);

/**
 * @brief Mark the layer's whole window dirty.
 *
 * A null layer does nothing. Changing an axis does not call this.
 *
 * @param self Container; must not be null.
 */
lh_void
lh_gl_container_invalidate(const lh_gl_container_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_CONTAINER_H */
