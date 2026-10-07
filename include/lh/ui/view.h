/**
 * @file view.h
 * @brief Entity tree drawn on an ::lh_ui_canvas_t: ::lh_ui_view_t.
 *
 * Paint is begin, optional clear, ::lh_ui_entity_draw on the root, end. The
 * walk, the hidden skip, and the style fill all live in `lh/ui`. Does not own
 * canvas, root, or clear. Does not need OS — only canvas and entity.
 *
 * Pointer session: ::lh_ui_view_press / ::lh_ui_view_move / ::lh_ui_view_release
 * own thumb-drag grab and synthesize a click on release when the press did not
 * drag. ::lh_ui_view_wheel scrolls the nearest container under the cursor.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_VIEW_H
#define LH_UI_VIEW_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/container.h>
#include <lh/ui/entity/scrollbar.h>
#include <lh/ui/point.h>
#include <lh/ui/scalar.h>
#include <lh/ui/view/fields.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/view.h requires LH_LIBRARY_OPTION_UI"
#endif

/**
 * @struct lh_ui_view
 * @typedef lh_ui_view_t
 * @brief One UI tree bound to a canvas.
 */
struct lh_ui_view
{
    lh_ui_view_fields(lh_ui_canvas_t, lh_ui_entity_t, lh_ui_color_t);
};
typedef struct lh_ui_view lh_ui_view_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty view: no canvas, root, clear, or grab.
 */
lh_void
lh_ui_view_init(lh_ui_view_t *self);

/**
 * @brief Clear pointers and grab on @p self.
 */
lh_void
lh_ui_view_deinit(lh_ui_view_t *self);

/**
 * @brief Bind @p self to @p canvas. Not owned.
 */
lh_void
lh_ui_view_set_canvas(lh_ui_view_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief Canvas of @p self, or ::lh_null.
 */
lh_ui_canvas_t *
lh_ui_view_get_canvas(const lh_ui_view_t *self);

/**
 * @brief Bind @p self to root entity @p root. Not owned.
 */
lh_void
lh_ui_view_set_root(lh_ui_view_t *self, lh_ui_entity_t *root);

/**
 * @brief Root entity of @p self, or ::lh_null.
 */
lh_ui_entity_t *
lh_ui_view_get_root(const lh_ui_view_t *self);

/**
 * @brief Clear color for each paint, or ::lh_null to skip clear.
 */
lh_void
lh_ui_view_set_clear(lh_ui_view_t *self, const lh_ui_color_t *clear);

/**
 * @brief Clear color of @p self, or ::lh_null.
 */
const lh_ui_color_t *
lh_ui_view_get_clear(const lh_ui_view_t *self);

/**
 * @brief begin → clear → ::lh_ui_entity_draw(root, canvas) → end.
 *
 * Same as ::lh_ui_view_draw with ::lh_null damage (full frame).
 */
lh_void
lh_ui_view_paint(lh_ui_view_t *self);

/**
 * @brief Paint @p self clipped to @p damage (target space).
 *
 * ::lh_null @p damage paints the whole frame (clear + tree). With a rect,
 * the clear color fills only that area and the tree is clipped so undamaged
 * pixels on the surface stay from the previous frame.
 */
lh_void
lh_ui_view_draw(lh_ui_view_t *self, const lh_ui_rect_t *damage);

/**
 * @brief Size the canvas of @p self to the root rect. Nothing without a root.
 */
lh_void
lh_ui_view_fit_canvas(lh_ui_view_t *self);

/**
 * @brief Fill with the clear color: the whole canvas for a ::lh_null
 *        @p damage, else only @p damage. Nothing without a clear color.
 */
lh_void
lh_ui_view_clear(lh_ui_view_t *self, const lh_ui_rect_t *damage);

/**
 * @brief ::lh_ui_entity_draw of the root on the canvas. Nothing without a root.
 */
lh_void
lh_ui_view_draw_root(lh_ui_view_t *self);

/**
 * @brief begin → push the @p damage clip (::lh_null keeps none) → clear → root
 *        → pop → end.
 */
lh_void
lh_ui_view_draw_frame(lh_ui_view_t *self, const lh_ui_rect_t *damage);

/**
 * @brief Topmost visible entity under @p point, or ::lh_null.
 */
lh_ui_entity_t *
lh_ui_view_hit_test(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief Hit-test the root and send ::lh_ui_entity_event_click to the match.
 *
 * Returns the clicked entity, or ::lh_null. Prefer ::lh_ui_view_release for
 * pointer input so a thumb drag does not click.
 */
lh_ui_entity_t *
lh_ui_view_click(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief ::lh_ui_entity_click on the root; when @p box is not ::lh_null (the
 *        container a hit scrollbar drives), record its scroll damage if the
 *        click scrolled it (::lh_ui_view_damage_scroll).
 */
lh_ui_entity_t *
lh_ui_view_click_scrolling(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_point_t point);

/**
 * @brief Close a scroll step on @p box: false when its scroll still equals
 *        @p before; else reset the canvas damage to
 *        ::lh_ui_entity_scrollbar_add_scroll_damage and return true.
 *
 * The one end of every scroll the view drives (click, drag, wheel).
 */
lh_bool_t
lh_ui_view_damage_scroll(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_point_t before);

/**
 * @brief Clear grab, grab offset, pressed and dragged.
 */
lh_void
lh_ui_view_reset_pointer(lh_ui_view_t *self);

/**
 * @brief ::lh_ui_entity_scrollbar_get_thumb_start_at for @p point in the root
 *        space, moved into the space of @p bar (::lh_ui_entity_to_local).
 */
lh_ui_scalar_t
lh_ui_view_get_thumb_start_at(const lh_ui_entity_scrollbar_t *bar, lh_ui_point_t point);

/**
 * @brief Grab @p bar when @p point (root space) is on its thumb; nothing for a
 *        ::lh_null @p bar or a point off the thumb.
 */
lh_void
lh_ui_view_grab_thumb(lh_ui_view_t *self, lh_ui_entity_scrollbar_t *bar, lh_ui_point_t point);

/**
 * @brief Move the thumb of @p bar under @p point (root space), keeping the
 *        grab offset. Returns ::lh_ui_view_damage_scroll.
 */
lh_bool_t
lh_ui_view_drag_thumb(lh_ui_view_t *self, lh_ui_entity_scrollbar_t *bar, lh_ui_point_t point);

/**
 * @brief Start a pointer press: grab a scrollbar thumb when hit, else mark
 *        pressed for a later click.
 */
lh_void
lh_ui_view_press(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief Pointer move: drag a grabbed thumb. Returns true when scroll changed.
 */
lh_bool_t
lh_ui_view_move(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief End a pointer press: clear grab; if not dragged, ::lh_ui_view_click.
 *
 * Returns the clicked entity, or ::lh_null.
 */
lh_ui_entity_t *
lh_ui_view_release(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief Scroll @p box by `(0, @p dy)`; false for a ::lh_null @p box.
 *        Returns ::lh_ui_view_damage_scroll.
 */
lh_bool_t
lh_ui_view_scroll_by(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_scalar_t dy);

/**
 * @brief Scroll the container under @p point
 *        (::lh_ui_entity_scrollbar_find_scrolled) by `(0, @p dy)`.
 *
 * Returns true when scroll changed. @p dy is in content units (caller maps
 * wheel notches).
 */
lh_bool_t
lh_ui_view_wheel(lh_ui_view_t *self, lh_ui_point_t point, lh_ui_scalar_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_VIEW_H */
