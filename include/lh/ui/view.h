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
 * Several pointers: every pointer id below ::LH_OS_INPUT_POINTER_MAX has a session
 * of its own (::lh_ui_view_pointer_t, kept in the view, no heap) --
 * ::lh_ui_view_press_pointer / ::lh_ui_view_move_pointer /
 * ::lh_ui_view_release_pointer / ::lh_ui_view_cancel_pointer. Two fingers on two
 * buttons press both and click both. A click is a release over the entity its own
 * pointer went down on. A container one pointer drags, or whose thumb it holds,
 * stays that pointer's until it lets go: another pointer may press inside it, but
 * does not move it. An entity two pointers press stays pressed until the last of
 * them lets go. The functions without a pointer id are pointer `0`, the mouse.
 * Hover is not a session: ::lh_ui_view_set_hot is the app's, and the app decides
 * which pointer it follows (the mouse, as a rule -- a finger has no hover).
 *
 * Input events: ::lh_ui_view_input takes one ::lh_os_input_event_t, the event a
 * window's sink (::lh_os_window_set_on_input) or the web host hands over, and
 * calls the right one of the functions above. The view makes its own click from
 * press and release, so the gestures a recognizer synthesizes are not for it.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_VIEW_H
#define LH_UI_VIEW_H

#include <lh/config.h>
#include <lh/os/input/event.h>
#include <lh/os/input/pointer/id.h>
#include <lh/os/input/wheel.h>
#include <lh/ui/button.h>
#include <lh/ui/view/fields.h>
#include <lh/ui/view/pointer.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/view.h requires LH_LIBRARY_OPTION_UI"
#endif

/**
 * @struct lh_ui_view
 * @brief One UI tree bound to a canvas.
 */
struct lh_ui_view
{
    /** Members: ::lh_ui_view_fields. */
    lh_ui_view_fields(lh_ui_canvas_t, lh_ui_entity_t, lh_ui_color_t, lh_ui_rect_t,
                      lh_ui_view_pointer_t);
};
/** @copybrief lh_ui_view */
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
 * @def LH_UI_VIEW_FRAME_MS
 * @brief What one ::lh_ui_view_tick spends on the transitions in flight: one frame
 *        at the 60 Hz the tick is meant to be called at. An application that knows
 *        how long its frames really were calls ::lh_ui_view_advance with that
 *        instead -- not on top of the tick.
 */
#define LH_UI_VIEW_FRAME_MS 16

/**
 * @def LH_UI_VIEW_WHEEL_STEP
 * @brief How far one wheel notch (::LH_OS_INPUT_WHEEL_NOTCH) scrolls, in content
 *        units, when it comes in through ::lh_ui_view_input in a build without the OS
 *        layer (::LH_LIBRARY_OPTION_OS off).
 *
 * With the OS layer the view asks the system instead, ::lh_os_input_get_wheel_step on
 * each wheel event. The fallback is the OS layer's own default,
 * ::LH_OS_INPUT_WHEEL_STEP_DEFAULT (40), so a notch scrolls the same with and without
 * it while the system is at its default.
 */
#ifndef LH_UI_VIEW_WHEEL_STEP
#    define LH_UI_VIEW_WHEEL_STEP LH_OS_INPUT_WHEEL_STEP_DEFAULT
#endif /* LH_UI_VIEW_WHEEL_STEP */

/**
 * @struct lh_ui_view_focus_walk
 * @brief State of ::lh_ui_view_visit_focus while looking for the next focus.
 */
struct lh_ui_view_focus_walk
{
    const lh_ui_entity_t *current; /**< The focus the walk starts from. */
    lh_bool_t passed;              /**< True once ::current has been visited. */
    const lh_ui_entity_t *first;   /**< First focusable met: the wrap-around answer. */
    const lh_ui_entity_t *next;    /**< First focusable after ::current, once found. */
};
/** @copybrief lh_ui_view_focus_walk */
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
 * @brief Union @p rect (root space) into the one rect the window has yet to
 *        paint. An empty rect is ignored.
 */
lh_void
lh_ui_view_add_damage(lh_ui_view_t *self, const lh_ui_rect_t *rect);

/**
 * @brief Hand back the rect ::lh_ui_view_add_damage has collected and clear it.
 *
 * False when nothing has been recorded, and @p out is left untouched then.
 */
lh_bool_t
lh_ui_view_take_damage(lh_ui_view_t *self, lh_ui_rect_t *out);

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
 * @brief Rows per buffer when @p self draws strip by strip; `0` draws the whole
 *        target in one frame.
 *
 * This is the switch between the two render modes. A strip costs a buffer of
 * this many rows for the whole width and nothing else, so the memory falls with
 * it, and a strip the @p damage of a frame does not reach is not drawn, cleared
 * or presented at all. The picture is the same either way.
 */
lh_void
lh_ui_view_set_strip_height(lh_ui_view_t *self, lh_ui_scalar_t height);

/**
 * @brief Strip height of @p self, `0` for a whole-target frame.
 */
lh_ui_scalar_t
lh_ui_view_get_strip_height(const lh_ui_view_t *self);

/**
 * @brief True when @p self draws strip by strip.
 */
lh_bool_t
lh_ui_view_is_stripped(const lh_ui_view_t *self);

/**
 * @brief The @p index -th strip of @p self, counting from the top of the
 *        target. The whole width, exactly the strip height, the last one pulled
 *        up to fit: every strip is the same size, so the backend sizes its
 *        buffer once and keeps it. Empty past the last strip.
 */
lh_void
lh_ui_view_get_strip(const lh_ui_view_t *self, lh_ui_scalar_t index, lh_ui_rect_t *out);

/**
 * @brief Name a region of @p self that a frame reads pixels of, so a strip-wise
 *        frame draws it whole.
 *
 * A blur (and glass, which is a blur plus a tint) has to find the pixels it is
 * about to blur. In a frame drawn strip by strip those pixels are not in the
 * buffer beyond the strip line, so the effect says it cannot be drawn — which is
 * honest and leaves a hole in the picture. This is the other half: the caller
 * already knows where the panel is, so it says so once here, and every strip that
 * reaches into @p rect is drawn wide enough to hold all of it.
 *
 * Declare it where the region is laid out. The view does not own the rectangles
 * and never frees them; it keeps copies, at most ::LH_UI_VIEW_WHOLE_MAX of them.
 *
 * @return False for an empty rect or when all the room is taken.
 */
lh_bool_t
lh_ui_view_add_whole_area(lh_ui_view_t *self, const lh_ui_rect_t *rect);

/**
 * @brief Forget every whole area of @p self. Call it when the regions move, or
 *        when there are none left.
 */
lh_void
lh_ui_view_clear_whole_areas(lh_ui_view_t *self);

/**
 * @brief @p area grown to hold every whole area @p reach reaches into, cut to
 *        the target. ::lh_null @p area is the whole target.
 *
 * One rule, two answers in ::lh_ui_view_draw_strips: the strip is what the backend
 * sizes its buffer to, and the damage of the frame is what the part being drawn is
 * cut from. Both need the whole area, because an effect that reads pixels has to
 * find them in the buffer *and* be inside the part — the canvas refuses it
 * otherwise (::lh_ui_canvas_can_send_whole), and a refused effect over a cleared
 * part is a hole in the picture. A whole area @p reach does not touch is left out:
 * the effect there is not drawn this frame, so growing for it would buy a bigger
 * buffer and a longer present for nothing.
 */
lh_void
lh_ui_view_area_whole(const lh_ui_view_t *self, const lh_ui_rect_t *area, const lh_ui_rect_t *reach,
                      lh_ui_rect_t *out);

/**
 * @brief Draw @p self strip by strip, skipping the strips @p damage does not
 *        reach. One frame in ::lh_ui_view_draw terms: the clock and the call
 *        counts of the backend cover all of them.
 */
lh_void
lh_ui_view_draw_strips(lh_ui_view_t *self, const lh_ui_rect_t *damage);

/**
 * @brief begin_area(@p area) → push the @p damage clipped to it → clear → root
 *        → pop → end. @p damage is in target space; @p area is the buffer.
 */
lh_void
lh_ui_view_draw_frame_on(lh_ui_view_t *self, const lh_ui_rect_t *area, const lh_ui_rect_t *damage);

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

/** A click may change what the thing clicked looks like — a toggle flips, a row
   re-styles itself — and it happens after the release damaged the looks the
   target had then. So the target is damaged once more, now: the same rect, and
   the union grows only as far as the *new* look reaches. One rect union per
   click, and no extra pixels unless a look really did reach further. */
lh_ui_entity_t *
lh_ui_view_click_plain(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief ::lh_ui_entity_click on the root; when @p box is not ::lh_null (the
 *        container a hit scrollbar drives), record its scroll damage if the
 *        click scrolled it (::lh_ui_view_damage_scroll).
 */
lh_ui_entity_t *
lh_ui_view_click_scrolling(lh_ui_view_t *self, lh_ui_container_t *box, lh_ui_point_t point);

/**
 * @brief Close a scroll step on @p box: false when its scroll still equals
 *        @p before; else begin the gesture (::lh_ui_view_begin_scroll), send
 *        ::lh_ui_entity_event_scroll, reset the canvas damage to
 *        ::lh_ui_scrollbar_add_scroll_damage and return true.
 *
 * The one end of every scroll the view drives (click, drag, wheel, keys, glide).
 */
lh_bool_t
lh_ui_view_damage_scroll(lh_ui_view_t *self, lh_ui_container_t *box, lh_ui_point_t before);

/**
 * @brief Send scroll_begin to @p box unless its gesture is already open
 *        (closing another container's first).
 */
lh_void
lh_ui_view_begin_scroll(lh_ui_view_t *self, lh_ui_container_t *box);

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
lh_void
lh_ui_view_slow_down(lh_ui_point_t velocity, lh_ui_point_t *out);

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
lh_ui_view_start_throw(lh_ui_view_t *self, lh_ui_container_t *box);

/**
 * @brief True while content glides after a throw: keep calling
 *        ::lh_ui_view_tick until it is false again.
 */
lh_bool_t
lh_ui_view_is_gliding(const lh_ui_view_t *self);

/**
 * @brief One frame: the transitions in flight move on by ::LH_UI_VIEW_FRAME_MS,
 *        and a glide takes a step -- slow the velocity down and scroll by it.
 *        Returns true when anything changed (damage recorded, repaint it); the
 *        glide ends, with scroll_end, when it stops moving or gets too slow.
 */
lh_bool_t
lh_ui_view_tick(lh_ui_view_t *self);

/**
 * @brief Move every transition in flight on by @p ms, recording the damage of each
 *        look that changed; a transition whose time is up lands on its style and
 *        frees its slot.
 *
 * @return ::lh_bool_true while anything changed, so a frame that only fades is
 *         painted.
 */
lh_bool_t
lh_ui_view_advance(lh_ui_view_t *self, lh_u16_t ms);

/** The slot @p entity is in flight in, or ::lh_null. */
lh_ui_transition_t *
lh_ui_view_slot_of(const lh_ui_view_t *self, const lh_ui_entity_t *entity);

/** Point the entity of @p slot at its target and free the slot. */
lh_void
lh_ui_view_land(lh_ui_transition_t *slot);

/**
 * @brief Send @p entity towards @p to over @p to's time (::lh_ui_style_get_transition),
 *        from the look it has now -- a look in flight included.
 *
 * Called by ::lh_ui_entity_set_style, which is the way to ask for one.
 *
 * @return ::lh_bool_false when every slot is taken: the switch is then the caller's to
 *         make at once.
 */
lh_bool_t
lh_ui_view_start_transition(lh_ui_view_t *self, lh_ui_entity_t *entity, const lh_ui_style_t *to);

/**
 * @brief The style @p entity is heading for, or ::lh_null when it is not in flight.
 */
const lh_ui_style_t *
lh_ui_view_get_transition_target(const lh_ui_view_t *self, const lh_ui_entity_t *entity);

/**
 * @brief Forget the flight of @p entity without touching its style: the caller is
 *        about to give it another one.
 */
lh_void
lh_ui_view_drop_transition(lh_ui_view_t *self, const lh_ui_entity_t *entity);

/**
 * @brief Land every transition of @p subtree -- the entity and everything under it --
 *        on its style at once, and free the slots.
 *
 * ::lh_ui_entity_remove_child calls it for the subtree it takes out, so an entity that
 * is taken out of the tree before the application frees it is never left behind in a
 * slot. ::lh_null lands every transition of the view.
 */
lh_void
lh_ui_view_finish_transitions(lh_ui_view_t *self, const lh_ui_entity_t *subtree);

/**
 * @brief Clear the session of every pointer (::lh_ui_view_pointer_reset): grab,
 *        pressed, dragged, the pressed entity and the drag box. What an entity
 *        shows as pressed is left as it is (::lh_ui_view_cancel lets it go).
 */
lh_void
lh_ui_view_reset_pointer(lh_ui_view_t *self);

/**
 * @brief The session of pointer @p id, or ::lh_null for an id at or above
 *        ::LH_OS_INPUT_POINTER_MAX, which the view does not follow.
 */
const lh_ui_view_pointer_t *
lh_ui_view_get_pointer(const lh_ui_view_t *self, lh_os_input_pointer_id_t id);

/**
 * @brief ::lh_ui_scrollbar_get_thumb_start_at for @p point in the root
 *        space, moved into the space of @p bar (::lh_ui_entity_to_local).
 */
lh_ui_scalar_t
lh_ui_view_get_thumb_start_at(const lh_ui_scrollbar_t *bar, lh_ui_point_t point);

/** The session of pointer @p id in @p self, or ::lh_null for an id the view does
   not follow. */
lh_ui_view_pointer_t *
lh_ui_view_get_slot(lh_ui_view_t *self, lh_os_input_pointer_id_t id);

/** True when a pointer of @p self other than @p pointer drives @p box: drags its
   content or holds the thumb of a bar that scrolls it. A box is one pointer's at a
   time -- two fingers pulling one list two ways is no scroll anybody meant. */
lh_bool_t
lh_ui_view_is_driven_by_other(const lh_ui_view_t *self, const lh_ui_view_pointer_t *pointer,
                              const lh_ui_container_t *box);

/** True when a pointer of @p self other than @p pointer went down on @p entity
   and has not let go of it. */
lh_bool_t
lh_ui_view_is_held_by_other(const lh_ui_view_t *self, const lh_ui_view_pointer_t *pointer,
                            const lh_ui_entity_t *entity);

/** Close the scroll gesture of @p self as @p pointer lets go, unless another
   pointer still drives the container it is about: that one's gesture is not over. */
lh_void
lh_ui_view_end_scroll_of(lh_ui_view_t *self, const lh_ui_view_pointer_t *pointer);

/**
 * @brief @p pointer grabs @p bar when @p point (root space) is on its thumb;
 *        nothing for a ::lh_null @p bar, a point off the thumb, or a bar whose
 *        container another pointer of @p self already drags.
 */
lh_void
lh_ui_view_grab_thumb(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_scrollbar_t *bar,
                      lh_ui_point_t point);

/**
 * @brief Move the thumb of @p bar under @p point (root space), keeping the
 *        grab offset of @p pointer. Returns ::lh_ui_view_damage_scroll.
 */
lh_bool_t
lh_ui_view_drag_thumb(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_scrollbar_t *bar,
                      lh_ui_point_t point);

/**
 * @brief The container a press on @p hit may drag: the nearest one at or
 *        above it, none on a scrollbar.
 */
lh_ui_container_t *
lh_ui_view_get_drag_box(lh_ui_entity_t *hit);

/** Let go of the entity @p pointer went down on, unless another pointer of @p self
   still presses it: the pressed look is the last finger's to end. */
lh_void
lh_ui_view_unpress_target(lh_ui_view_t *self, const lh_ui_view_pointer_t *pointer);

/**
 * @brief Begin the session of @p pointer at @p point: stop a glide, let go of a
 *        press of @p pointer still open, reset its state.
 */
lh_void
lh_ui_view_start_pointer(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_point_t point);

/**
 * @brief The press of @p pointer reaching @p hit: press event, focus, thumb grab,
 *        drag box -- none when another pointer already drags that container.
 *
 * Also presses it (::lh_ui_entity_set_pressed), so an entity paints its pressed
 * style and the caller has the damage to show it.
 */
lh_void
lh_ui_view_press_on(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_entity_t *hit,
                    lh_ui_point_t point);

/**
 * @brief Press @p entity or let it go, damaging what either look painted
 *        (::lh_ui_view_damage_shape). Nothing for a ::lh_null @p entity or a
 *        flag that is already what it is asked for.
 *
 * What ::lh_ui_view_press and ::lh_ui_view_release call; an app that drives the
 * pointer itself may call it too. The view clears a press that is still open when
 * a new one starts, so a pressed look cannot stick.
 */
lh_void
lh_ui_view_set_pressed(lh_ui_view_t *self, lh_ui_entity_t *entity, lh_bool_t pressed);

/** The rect plus whichever of two looks reaches further with its shadow. Both
   looks and not the current one: a shadow reaches past its own box, so anything
   less leaves the fringe of the look that is being dropped on the surface. The
   two are the resting and pressed looks of a press (::lh_ui_view_damage_shape)
   and the resting and hot looks of a hover (::lh_ui_view_set_hot); either may
   be ::lh_null, which is a look that casts nothing. */
lh_void
lh_ui_view_damage_looks(lh_ui_view_t *self, const lh_ui_entity_t *entity,
                        const lh_ui_style_t *first, const lh_ui_style_t *second);

/**
 * @brief Record in the canvas damage the rect of @p entity grown by whichever of
 *        its two shadows (::lh_ui_style_get_shadow of its style and of its
 *        pressed style) reaches further.
 *
 * Everything @p entity paints, in both of its looks and not in the current one: a
 * press changes what is drawn and what stops being drawn, and so does hiding a card
 * that casts a shadow — a shadow reaches past its own box, so the rect alone would
 * leave the fringe of what is being dropped on the surface. The caller invalidates,
 * as it does for every other damage here.
 */
lh_void
lh_ui_view_damage_shape(lh_ui_view_t *self, const lh_ui_entity_t *entity);

/**
 * @brief Say the pointer is on @p button, or is not, and damage what either of
 *        its two looks painted — the same rule as a press, over the resting and
 *        the hot style. Nothing for a ::lh_null @p button or a flag that is
 *        already what it is asked for.
 *
 * The step an app takes for ::lh_ui_button_set_hot: the pointer arrives from the
 * OS as coordinates and the app decides what is under it. This does the state
 * and the damage together, so the button itself stays a state holder with no
 * canvas in it. The caller invalidates, as it does for every other damage here.
 */
lh_void
lh_ui_view_set_hot(lh_ui_view_t *self, lh_ui_button_t *button, lh_bool_t hot);

/**
 * @brief Drag the content of the drag box of @p pointer with it once past the
 *        threshold. Returns ::lh_ui_view_damage_scroll.
 */
lh_bool_t
lh_ui_view_drag_content(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_point_t point);

/**
 * @brief End of a press of @p pointer: glide at its speed after a content drag,
 *        else close the gesture -- unless another pointer still drives the
 *        container it is about.
 */
lh_void
lh_ui_view_finish_gesture(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer);

/**
 * @brief Pointer @p id goes down at @p point: let go of a press of that pointer
 *        still open, press the entity that takes the pointer there, move the focus,
 *        grab a scrollbar thumb when hit, else mark pressed for a later click.
 *
 * Nothing for an @p id at or above ::LH_OS_INPUT_POINTER_MAX. The other pointers'
 * presses stay as they are.
 */
lh_void
lh_ui_view_press_pointer(lh_ui_view_t *self, lh_os_input_pointer_id_t id, lh_ui_point_t point);

/**
 * @brief Pointer @p id moves to @p point: drag the thumb it holds, or the content
 *        it went down on once past the threshold. Returns true when scroll changed.
 */
lh_bool_t
lh_ui_view_move_pointer(lh_ui_view_t *self, lh_os_input_pointer_id_t id, lh_ui_point_t point);

/**
 * @brief Pointer @p id goes up at @p point: the entity it went down on hears the
 *        release and is let go (::lh_ui_view_set_pressed) unless another pointer
 *        still presses it; with no drag, and over that same entity, a click
 *        (::lh_ui_view_click).
 *
 * Returns the clicked entity, or ::lh_null.
 */
lh_ui_entity_t *
lh_ui_view_release_pointer(lh_ui_view_t *self, lh_os_input_pointer_id_t id, lh_ui_point_t point);

/**
 * @brief Abandon the press of pointer @p id without a click (::lh_ui_view_cancel
 *        for one pointer): its entity hears the release at the last point the
 *        pointer was seen and is let go unless another pointer presses it, its
 *        thumb is dropped, its drag stops where it is. Nothing when it has no press.
 */
lh_void
lh_ui_view_cancel_pointer(lh_ui_view_t *self, lh_os_input_pointer_id_t id);

/**
 * @brief ::lh_ui_view_press_pointer of pointer `0`, the mouse.
 */
lh_void
lh_ui_view_press(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief ::lh_ui_view_move_pointer of pointer `0`, the mouse.
 */
lh_bool_t
lh_ui_view_move(lh_ui_view_t *self, lh_ui_point_t point);

/**
 * @brief ::lh_ui_view_release_pointer of pointer `0`, the mouse.
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
lh_ui_view_scroll_by(lh_ui_view_t *self, lh_ui_container_t *box, lh_ui_scalar_t dx,
                     lh_ui_scalar_t dy);

/**
 * @brief Scroll the container under @p point
 *        (::lh_ui_scrollbar_find_scrolled) by `(@p dx, @p dy)`.
 *
 * Returns true when scroll changed. @p dy is in content units (caller maps
 * wheel notches).
 */
lh_bool_t
lh_ui_view_wheel(lh_ui_view_t *self, lh_ui_point_t point, lh_ui_scalar_t dx, lh_ui_scalar_t dy);

/* -- Focus and keys -------------------------------------------------------- */

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
lh_ui_view_key(lh_ui_view_t *self, lh_os_key_t key, lh_bool_t pressed);

/**
 * @brief One typed character (code point @p code) for the focus.
 */
lh_bool_t
lh_ui_view_text(lh_ui_view_t *self, lh_u32_t code);

/* -- Input events ------------------------------------------------------------ */

/**
 * @brief Abandon every press in progress without a click
 *        (::lh_ui_view_cancel_pointer of each pointer): the pressed entities are
 *        let go, grabbed thumbs dropped, drags stopped where they are (no glide).
 *
 * For the window losing the focus mid-press; the one pointer the system took
 * away (::lh_os_input_kind_pointer_cancel) is ::lh_ui_view_cancel_pointer. Each
 * target still hears ::lh_ui_entity_event_release, at the last point its pointer
 * was seen, so a press is always paired; what it never gets is the click. Does
 * nothing when no press is open.
 */
lh_void
lh_ui_view_cancel(lh_ui_view_t *self);

/** Where @p event happened, in whole view units. */
lh_ui_point_t
lh_ui_view_point_of(const lh_os_input_event_ptr event);

/** Content units one wheel notch scrolls: the system's step
   (::lh_os_input_get_wheel_step), or ::LH_UI_VIEW_WHEEL_STEP where there is no OS layer
   to ask. */
lh_s32_t lh_ui_view_wheel_step(lh_void);

/** A wheel distance in 1/120 notch (positive up / left) as content units to
   scroll by (positive down / right), @p step units a notch. */
lh_ui_scalar_t
lh_ui_view_wheel_distance(lh_s32_t notch_120, lh_s32_t step);

/** The wheel case of ::lh_ui_view_input: both axes by one step, asked once. */
lh_bool_t
lh_ui_view_input_wheel(lh_ui_view_t *self, const lh_os_input_event_ptr event);

/** True when @p event is a button of the first pointer that the view presses with. */
lh_bool_t
lh_ui_view_is_primary_press(const lh_os_input_event_ptr event);

/** The pointer half of ::lh_ui_view_input: each pointer the view follows to its own
   session. */
lh_bool_t
lh_ui_view_input_pointer(lh_ui_view_t *self, const lh_os_input_event_ptr event);

/**
 * @brief One input event, to the pointer session or the focus.
 *
 * | Event                                   | Goes to                     |
 * |-----------------------------------------|-----------------------------|
 * | `pointer_down` of the primary button    | ::lh_ui_view_press_pointer  |
 * | `pointer_move`                          | ::lh_ui_view_move_pointer   |
 * | `pointer_up` of the primary button      | ::lh_ui_view_release_pointer|
 * | `pointer_cancel`                        | ::lh_ui_view_cancel_pointer |
 * | `focus_out`                             | ::lh_ui_view_cancel         |
 * | `wheel`                                 | ::lh_ui_view_wheel          |
 * | `key_down` (a repeat too) / `key_up`    | ::lh_ui_view_key            |
 * | `text`                                  | ::lh_ui_view_text           |
 *
 * Each pointer below ::LH_OS_INPUT_POINTER_MAX is followed in a session of its own,
 * the event's `pointer` the id: the mouse `0`, touch contacts and pens from `1`. A
 * wheel notch up or left scrolls back toward the start by the system's step,
 * ::lh_os_input_get_wheel_step content units (::LH_UI_VIEW_WHEEL_STEP without
 * ::LH_LIBRARY_OPTION_OS). Other buttons, pointers at or above the limit,
 * controllers and every synthesized event (::lh_os_input_event_is_synthesized) are
 * not for the view.
 *
 * @return True when the view acted on @p event; repaint what
 *         ::lh_ui_view_take_damage then returns. A move or a wheel notch acts
 *         only when it scrolled something (::lh_ui_view_move,
 *         ::lh_ui_view_wheel). False for an event the view has no use for.
 *
 * @code
 * lh_void
 * on_input(lh_self_ptr self, const lh_os_input_event_ptr event)
 * {
 *     lh_ui_view_t *view = lh_ptr_rcast(lh_ui_view_t, self);
 *
 *     if (lh_ui_view_input(view, event)) // pressed, scrolled, typed into
 *     {
 *         repaint(view);                 // the app's own: draw the damage
 *     }
 * }
 * @endcode
 */
lh_bool_t
lh_ui_view_input(lh_ui_view_t *self, const lh_os_input_event_ptr event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_VIEW_H */
