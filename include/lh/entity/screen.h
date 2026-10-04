/**
 * @file screen.h
 * @brief The root of what is shown on one display, redrawn only where it
 *        changed.
 *
 * A screen is a 2D entity the size of the display, at the origin, filled
 * with its background (opaque black by default) so every redraw starts from
 * a clean background. Everything shown is a descendant of it.
 *
 * Changes mark areas dirty (::lh_entity_invalidate; the 2D setters do it
 * themselves) and ::lh_entity_screen_render redraws only those areas: a
 * blinking cursor costs a few pixels, not the whole display. That is what
 * keeps the engine fast on a microcontroller as well as on a PC.
 *
 * The view is orthographic and looks along -z, so a larger world z is
 * closer. When the canvas has a depth plane, each dirty area is cleared to
 * the farthest depth and a nearer surface covers a farther one; equal depth
 * keeps the paint order. The screen is itself a surface at its z (0 by
 * default), so a descendant with a smaller z is behind the background.
 * Perspective comes with cameras.
 */

#ifndef LH_ENTITY_SCREEN_H
#define LH_ENTITY_SCREEN_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/entity/screen/fields.h>
#include <lh/math/rect.h>
#include <lh/size.h>
#include <lh/ui/canvas.h>

/**
 * @struct lh_entity_screen
 * @brief Fields via ::lh_entity_fields, ::lh_entity_2d_fields, then
 *        ::lh_entity_screen_fields.
 */
struct lh_entity_screen
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_entity_screen_fields(lh_math_rect_t, lh_usize_t, lh_entity_t *);
};
typedef struct lh_entity_screen lh_entity_screen_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_screen_t, derived from ::lh_entity_2d_class.
 *
 * Create it as a root (::lh_entity_create_root) and give it the display's
 * size (::lh_entity_2d_set_size), which marks the whole display dirty for
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
lh_entity_screen_invalidate_area(lh_entity_screen_t *self, lh_math_rect_t area);

/**
 * @brief How many areas wait to be redrawn.
 */
lh_usize_t
lh_entity_screen_get_dirty_count(const lh_entity_screen_t *self);

/**
 * @brief The area at @p index (< ::lh_entity_screen_get_dirty_count) that
 *        waits to be redrawn.
 */
lh_math_rect_t
lh_entity_screen_get_dirty_area(const lh_entity_screen_t *self, lh_usize_t index);

/**
 * @brief Redraw every dirty area onto @p canvas, then forget them.
 *
 * In each area the depth plane, when there is one, is cleared to the
 * farthest depth, then every visible entity is drawn in order (a parent
 * before its children, older siblings before younger ones): its background
 * (::lh_entity_2d_draw_background), then ::LH_ENTITY_EVENT_DRAW, clipped to
 * the area and to its parents' boxes unless they let it overflow. A nearer
 * surface (larger z) covers a farther one; equal z keeps that order. An
 * entity whose box misses the area is skipped, and so are the children that
 * box cuts.
 *
 * @return The bounding box of what was redrawn (empty when nothing was):
 *         the part of @p canvas to copy to the display.
 */
lh_math_rect_t
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

/**
 * @brief Deliver a pointer event at the screen position @p point to the
 *        entity on top there (::lh_entity_2d_find_at).
 *
 * @param code  ::LH_ENTITY_EVENT_POINTER_DOWN, `_UP` or `_MOVE`.
 * @param point Position on the screen, e.g. the mouse in window pixels.
 * @return The entity that received it, or ::lh_null when nothing is there.
 */
lh_entity_t *
lh_entity_screen_send_pointer(lh_entity_screen_t *self, lh_uint_t code, lh_math_vec2_t point);

/**
 * @brief The entity that receives keys, or ::lh_null.
 */
lh_entity_t *
lh_entity_screen_get_focus(const lh_entity_screen_t *self);

/**
 * @brief Keys from the window and from an on-screen keyboard go to @p entity.
 *        ::lh_null clears the focus.
 */
lh_void
lh_entity_screen_set_focus(lh_entity_screen_t *self, lh_entity_t *entity);

/**
 * @brief Deliver ::LH_ENTITY_EVENT_KEY with @p code to the focus.
 */
lh_void
lh_entity_screen_send_key(lh_entity_screen_t *self, lh_uint_t code);

/**
 * @brief Forget a held press on @p entity.
 *
 * A button calls this as it is deleted, so the screen does not release a
 * press onto an entity that is already gone.
 */
lh_void
lh_entity_screen_clear_pressed(lh_entity_screen_t *self, lh_entity_t *entity);

/**
 * @brief The entity that is holding the pointer, or ::lh_null.
 */
lh_entity_t *
lh_entity_screen_get_pressed(const lh_entity_screen_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_SCREEN_H */
