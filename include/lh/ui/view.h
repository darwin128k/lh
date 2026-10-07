/**
 * @file view.h
 * @brief Entity tree drawn on an ::lh_ui_canvas_t: ::lh_ui_view_t.
 *
 * Paint is begin, optional clear, ::lh_ui_entity_draw on the root, end. The
 * walk, the hidden skip, and the style fill all live in `lh/ui`. Does not own
 * canvas, root, or clear. Does not need OS — only canvas and entity.
 *
 * Pointer session: ::lh_ui_view_press / ::lh_ui_view_move / ::lh_ui_view_release
 * send press / release to the entity under the pointer, move the focus to
 * the nearest focusable entity, drag a grabbed thumb or — past
 * ::LH_UI_VIEW_DRAG_THRESHOLD — the content of the container under the press,
 * and synthesize a click on release when nothing was dragged. A fast drag
 * keeps gliding: call ::lh_ui_view_tick on a timer (about 60 Hz). Every
 * scroll change sends ::lh_ui_entity_event_scroll to its container, inside one
 * scroll_begin / scroll_end pair per gesture (drag and glide, wheel notch,
 * page click, key). ::lh_ui_view_wheel scrolls the container under the
 * cursor; ::lh_ui_view_key / ::lh_ui_view_text go to the focus, Tab moves it.
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
#include <lh/ui/key.h>
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

/**
 * @def LH_UI_VIEW_DRAG_THRESHOLD
 * @brief How far (on either axis) the pointer must travel from the press
 *        before it drags the content instead of clicking.
 */
#define LH_UI_VIEW_DRAG_THRESHOLD lh_ui_scalar(4)

/**
 * @def LH_UI_VIEW_THROW_MIN
 * @brief Least release speed (per move, either axis) that keeps the content
 *        gliding after a drag.
 */
#define LH_UI_VIEW_THROW_MIN lh_ui_scalar(2)

/**
 * @def LH_UI_VIEW_THROW_KEEP
 * @brief Percent of the glide speed kept from one ::lh_ui_view_tick to the next.
 */
#define LH_UI_VIEW_THROW_KEEP 90

/**
 * @struct lh_ui_view_focus_walk
 * @typedef lh_ui_view_focus_walk_t
 * @brief State of ::lh_ui_view_visit_focus while looking for the next focus.
 */
struct lh_ui_view_focus_walk
{
    const lh_ui_entity_t *current;
    lh_bool_t passed;
    const lh_ui_entity_t *first;
    const lh_ui_entity_t *next;
};
typedef struct lh_ui_view_focus_walk lh_ui_view_focus_walk_t;

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
 *        @p before; else begin the gesture (::lh_ui_view_begin_scroll), send
 *        ::lh_ui_entity_event_scroll, reset the canvas damage to
 *        ::lh_ui_entity_scrollbar_add_scroll_damage and return true.
 *
 * The one end of every scroll the view drives (click, drag, wheel, keys, glide).
 */
lh_bool_t
lh_ui_view_damage_scroll(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_point_t before);

/**
 * @brief Send scroll_begin to @p box unless its gesture is already open
 *        (closing another container's first).
 */
lh_void
lh_ui_view_begin_scroll(lh_ui_view_t *self, lh_ui_entity_container_t *box);

/**
 * @brief Send scroll_end to the container whose gesture is open, if any.
 */
lh_void
lh_ui_view_end_scroll(lh_ui_view_t *self);

/**
 * @brief True when neither axis of @p velocity reaches @p limit.
 */
lh_bool_t
lh_ui_view_is_slow(lh_ui_point_t velocity, lh_ui_scalar_t limit);

/**
 * @brief @p velocity times ::LH_UI_VIEW_THROW_KEEP percent.
 */
lh_ui_point_t
lh_ui_view_slow_down(lh_ui_point_t velocity);

/**
 * @brief Stop a glide (and close its gesture); zero the velocity.
 */
lh_void
lh_ui_view_stop_throw(lh_ui_view_t *self);

/**
 * @brief After a content drag on @p box: glide on when the release was fast
 *        enough (::LH_UI_VIEW_THROW_MIN), else close the gesture.
 */
lh_void
lh_ui_view_start_throw(lh_ui_view_t *self, lh_ui_entity_container_t *box);

/**
 * @brief One glide step: slow the velocity down and scroll by it. Returns
 *        true when the scroll changed (damage recorded, repaint it); the glide
 *        ends, with scroll_end, when it stops moving or gets too slow.
 */
lh_bool_t
lh_ui_view_tick(lh_ui_view_t *self);

/**
 * @brief Clear grab, pressed, dragged, the pressed entity and the drag box.
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
 * @brief The container a press on @p hit may drag: the nearest one at or
 *        above it, none on a scrollbar.
 */
lh_ui_entity_container_t *
lh_ui_view_get_drag_box(lh_ui_entity_t *hit);

/**
 * @brief Begin a pointer session at @p point: stop a glide, reset the state.
 */
lh_void
lh_ui_view_start_pointer(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief The press reaching @p hit: press event, focus, thumb grab, drag box.
 */
lh_void
lh_ui_view_press_on(lh_ui_view_t *self, lh_ui_entity_t *hit, lh_ui_point_t point);

/**
 * @brief True when @p point is ::LH_UI_VIEW_DRAG_THRESHOLD or more from the press.
 */
lh_bool_t
lh_ui_view_is_past_threshold(const lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief Move the tracked pointer to @p point and update the glide velocity.
 */
lh_void
lh_ui_view_track(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief Drag the content of the drag box with the pointer once past the
 *        threshold. Returns ::lh_ui_view_damage_scroll.
 */
lh_bool_t
lh_ui_view_drag_content(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief End of a press: glide after a content drag, else close the gesture.
 */
lh_void
lh_ui_view_finish_gesture(lh_ui_view_t *self);

/**
 * @brief Start a pointer press: grab a scrollbar thumb when hit, else mark
 *        pressed for a later click.
 */
lh_void
lh_ui_view_press(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief Pointer move: drag a grabbed thumb, or the content past the
 *        threshold. Returns true when scroll changed.
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
 * @brief Scroll @p box by `(@p dx, @p dy)`; false for a ::lh_null @p box.
 *        Returns ::lh_ui_view_damage_scroll.
 */
lh_bool_t
lh_ui_view_scroll_by(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_scalar_t dx, lh_ui_scalar_t dy);

/**
 * @brief Scroll the container under @p point
 *        (::lh_ui_entity_scrollbar_find_scrolled) by `(@p dx, @p dy)`.
 *
 * Returns true when scroll changed. @p dy is in content units (caller maps
 * wheel notches).
 */
lh_bool_t
lh_ui_view_wheel(lh_ui_view_t *self, lh_ui_point_t point, lh_ui_scalar_t dx, lh_ui_scalar_t dy);

/* ── Focus and keys ──────────────────────────────────────────────────────── */

/**
 * @brief The entity keys go to, or ::lh_null.
 */
lh_ui_entity_t *
lh_ui_view_get_focus(const lh_ui_view_t *self);

/**
 * @brief Move the focus to @p entity (::lh_null clears it): defocus to the old
 *        one, focus to the new one.
 */
lh_void
lh_ui_view_set_focus(lh_ui_view_t *self, lh_ui_entity_t *entity);

/**
 * @brief Visitor of the Tab walk (::lh_ui_view_get_next_focus).
 */
lh_bool_t
lh_ui_view_visit_focus(const lh_ui_entity_t *entity, lh_ptr context);

/**
 * @brief The focusable entity after the focus in tree order, wrapping to the
 *        first; the first when nothing is focused.
 */
lh_ui_entity_t *
lh_ui_view_get_next_focus(const lh_ui_view_t *self);

/**
 * @brief Send @p input to the focus. True when it scrolled the focus (a
 *        container): damage recorded, repaint it.
 */
lh_bool_t
lh_ui_view_send_input(lh_ui_view_t *self, const lh_ui_key_input_t *input);

/**
 * @brief A key went down or up: Tab (down) moves the focus, every other key
 *        goes to it. True when that scrolled something.
 */
lh_bool_t
lh_ui_view_key(lh_ui_view_t *self, lh_key_t key, lh_bool_t pressed);

/**
 * @brief One typed character (code point @p code) for the focus.
 */
lh_bool_t
lh_ui_view_text(lh_ui_view_t *self, lh_u32_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_VIEW_H */
