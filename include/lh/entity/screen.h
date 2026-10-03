/**
 * @file screen.h
 * @brief The root of what is shown on one display, redrawn only where it
 *        changed.
 *
 * A screen is a rectangle the size of the display, at the origin, filled
 * with its color (opaque black by default) so every redraw starts from a
 * clean background. Everything shown is a descendant of it.
 *
 * Changes mark areas "dirty" (::lh_entity_invalidate; the setters of
 * ::lh_entity_2d_t and ::lh_entity_rect_t do it themselves) and
 * ::lh_entity_screen_render redraws only those areas: a blinking cursor
 * costs a few pixels, not the whole display. That is what keeps the
 * engine fast on a microcontroller as well as on a PC.
 *
 * Drawing is flat: the screen is seen straight along z, so 3D entities in
 * the tree are drawn by their x and y only. Perspective comes with cameras.
 */

#ifndef LH_ENTITY_SCREEN_H
#define LH_ENTITY_SCREEN_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/rect.h>
#include <lh/entity/screen/fields.h>
#include <lh/size.h>
#include <lh/ui/canvas.h>
#include <lh/ui/geom.h>

/**
 * @struct lh_entity_screen
 * @brief Fields via ::lh_entity_fields, ::lh_entity_2d_fields,
 *        ::lh_entity_rect_fields, then ::lh_entity_screen_fields.
 */
struct lh_entity_screen
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_vec2_t, lh_float_t);
    lh_entity_rect_fields(lh_vec2_t, lh_ui_color_t);
    lh_entity_screen_fields(lh_ui_rect_t, lh_usize_t);
};
typedef struct lh_entity_screen lh_entity_screen_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_screen_t, derived from ::lh_entity_rect_class.
 *
 * Create it as a root (::lh_entity_create_root) and give it the display's
 * size (::lh_entity_rect_set_size), which marks the whole display dirty for
 * the first render.
 */
extern const lh_entity_class_t lh_entity_screen_class;

/**
 * @brief Mark the screen area @p area (cut to the screen) for redrawing.
 *
 * Overlapping areas merge; past ::LH_ENTITY_SCREEN_DIRTY_MAX separate ones,
 * all merge into their bounding box.
 */
lh_void
lh_entity_screen_invalidate_area(lh_entity_screen_t *self, lh_ui_rect_t area);

/**
 * @brief How many areas wait to be redrawn.
 */
lh_usize_t
lh_entity_screen_get_dirty_count(const lh_entity_screen_t *self);

/**
 * @brief The area at @p index (< ::lh_entity_screen_get_dirty_count) that
 *        waits to be redrawn.
 */
lh_ui_rect_t
lh_entity_screen_get_dirty_area(const lh_entity_screen_t *self, lh_usize_t index);

/**
 * @brief Redraw every dirty area onto @p canvas, then forget them.
 *
 * In each area, every visible entity of the tree receives
 * ::LH_ENTITY_EVENT_DRAW in drawing order (a parent before its children,
 * older siblings before younger ones), clipped to the area and to its
 * parents unless they let it overflow.
 *
 * @return The bounding box of what was redrawn (empty when nothing was):
 *         the part of @p canvas to copy to the display.
 */
lh_ui_rect_t
lh_entity_screen_render(lh_entity_screen_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief Mark the screen area @p self covers, with its children, for
 *        redrawing.
 *
 * Does nothing when @p self is not under a screen. Call it before and after
 * changes the setters do not cover, such as hiding @p self or moving it to
 * another parent.
 */
lh_void
lh_entity_invalidate(lh_entity_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_SCREEN_H */
