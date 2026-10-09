/**
 * @file entity.h
 * @brief One object in a UI tree: ::lh_ui_entity_t.
 *
 * An entity covers a ::lh_ui_rect_t, may share a ::lh_ui_style_t, and points
 * at a class whose event function is shared by every instance of that kind.
 * Larger widgets are built by linking children (composition), not by copying
 * getters onto every specialized type. Entities not shown (hidden, or their
 * class says no: ::lh_ui_entity_is_shown) skip paint and hit tests with their
 * descendants. ::lh_ui_entity_draw walks the tree and sends
 * ::lh_ui_entity_event_draw (with the canvas) to each class.
 * ::lh_ui_entity_click hit-tests and sends ::lh_ui_entity_event_click. The
 * style and the children are not owned: they must outlive the links.
 */

#ifndef LH_UI_ENTITY_H
#define LH_UI_ENTITY_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/entity/class.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/entity/face/cb.h>
#include <lh/ui/entity/fields.h>
#include <lh/ui/entity/transform.h>
#include <lh/ui/entity/visit/cb.h>
#include <lh/ui/layout/place.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/void.h>

/**
 * @struct lh_ui_entity
 * @typedef lh_ui_entity_t
 * @brief A tree node: rect, style, class, hidden, parent, children, link.
 */
struct lh_ui_entity
{
    lh_ui_entity_fields(lh_ui_rect_t, lh_ui_style_t, lh_ui_entity_class_t, lh_bool_t,
                        struct lh_ui_entity, lh_ui_place_t);
};
typedef struct lh_ui_entity lh_ui_entity_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

/**
 * @brief Fill @p self so it covers @p rect with no style, parent, or children.
 */
lh_void
lh_ui_entity_init(lh_ui_entity_t *self, lh_ui_rect_t rect);

/**
 * @brief Rectangle @p self covers, in the same space as its parent.
 */
lh_ui_rect_t
lh_ui_entity_get_rect(const lh_ui_entity_t *self);

/**
 * @brief How @p self wants a flow to place it (::lh_ui_place_t): fixed zero at
 *        the start until said otherwise.
 *
 * Carried by the entity and not by the parent that lays it out, because "how
 * long am I" is a fact about the widget and not about whoever holds it — the
 * same label is as wide as its text in a button and in a column without either
 * one knowing.
 */
const lh_ui_place_t *
lh_ui_entity_get_place(const lh_ui_entity_t *self);

/**
 * @brief Put @p place on @p self: what the next ::lh_ui_layout_apply of its
 *        parent has to work with.
 */
lh_void
lh_ui_entity_set_place(lh_ui_entity_t *self, const lh_ui_place_t *place);

/**
 * @brief Replace the rectangle @p self covers with @p rect.
 */
lh_void
lh_ui_entity_set_rect(lh_ui_entity_t *self, lh_ui_rect_t rect);

/**
 * @brief Style of @p self, or ::lh_null when it has none.
 */
const lh_ui_style_t *
lh_ui_entity_get_style(const lh_ui_entity_t *self);

/**
 * @brief Point @p self at @p style. The style is not copied.
 *
 * ::lh_null clears the style.
 */
lh_void
lh_ui_entity_set_style(lh_ui_entity_t *self, const lh_ui_style_t *style);

/**
 * @brief Solid fill color from the style of @p self, or ::lh_null when there
 *        is no style or its fill is not solid.
 */
const lh_ui_color_t *
lh_ui_entity_get_fill_color(const lh_ui_entity_t *self);

/**
 * @brief Corner radius of the style of @p self (unclamped), `0` without one.
 *        Its fill and, when it clips its children, their clip use it.
 *
 * From the entity's own style and not from the pressed one
 * (::lh_ui_entity_get_style_now): the shape a resting entity has is its own,
 * and clipping must not depend on a pointer being down somewhere.
 */
lh_ui_scalar_t
lh_ui_entity_get_radius(const lh_ui_entity_t *self);

/**
 * @brief Corner radius @p self is drawn and clipped with right now (unclamped):
 *        that of ::lh_ui_entity_get_style_now.
 *
 * The clip of children and the text of a label have to follow the shape the
 * fill has, and the fill follows a press. Reading the own radius instead would
 * cut the corners of the resting shape while the pressed fill is a pill — a
 * child would stick out past a corner nothing was painted into.
 */
lh_ui_scalar_t
lh_ui_entity_get_radius_now(const lh_ui_entity_t *self);

/**
 * @brief Corner radius a hit test rounds @p self with (unclamped):
 *        ::lh_ui_style_get_hit_radius of its own style, `0` without one.
 *
 * The whole rect by default: a rounded look is pressed by its rect, because the
 * rect is what a pointer aims at and the rounding is a look. Set the radius to
 * narrow that where the rect is much larger than the shape.
 *
 * From the own style and not from the style in force: the pressed look may not
 * change the target, or a press and the click that ends it would disagree about
 * what was hit. And the hit test runs before the press that asked for it, so a
 * shape read from the pressed flag would answer one frame too late.
 */
lh_ui_scalar_t
lh_ui_entity_get_hit_radius(const lh_ui_entity_t *self);

/**
 * @brief Style of @p self as it is painted right now: its pressed style while it
 *        is pressed (::lh_ui_entity_is_pressed) and one with none, and its own
 *        style otherwise.
 *
 * The one place that answers, so painting cannot read two styles by accident.
 * Geometry — padding, font, alignment — is read from ::lh_ui_entity_get_style
 * instead, and a pressed style may not move anything.
 */
const lh_ui_style_t *
lh_ui_entity_get_style_now(const lh_ui_entity_t *self);

/**
 * @brief True while the pointer is down on @p self.
 *
 * Kept by the view (::lh_ui_view_press sets it, ::lh_ui_view_release clears it)
 * and read by painting, which paints ::lh_ui_entity_get_style_now. Nothing else
 * changes about the entity.
 */
lh_bool_t
lh_ui_entity_is_pressed(const lh_ui_entity_t *self);

/**
 * @brief Press @p self or let it go. What the view does; an app that drives the
 *        pointer itself may do it too.
 */
lh_void
lh_ui_entity_set_pressed(lh_ui_entity_t *self, lh_bool_t pressed);

/**
 * @brief Class of @p self.
 */
const lh_ui_entity_class_t *
lh_ui_entity_get_class(const lh_ui_entity_t *self);

/**
 * @brief Padding of the style of @p self, all `0` without one.
 */
lh_ui_insets_t
lh_ui_entity_get_padding(const lh_ui_entity_t *self);

/**
 * @brief True when @p point (space of the rect of @p self) lies in the shape
 *        @p self is hit with: its rect with ::lh_ui_entity_get_hit_radius on the
 *        corners (::lh_ui_radius_contains).
 *
 * The shape it is *hit* with, not the one it paints: by default that is the
 * whole rect, so a rounded entity is pressed by its cut corner. A clipping
 * parent asks about the drawn shape instead
 * (::lh_ui_entity_searches_children_at), so that a corner it cut away is not a
 * hit of its children either.
 */
lh_bool_t
lh_ui_entity_contains_point(const lh_ui_entity_t *self, lh_ui_point_t point);

/**
 * @brief Point @p self at @p class. The class is not copied.
 */
lh_void
lh_ui_entity_set_class(lh_ui_entity_t *self, const lh_ui_entity_class_t *);

/**
 * @brief Move @p self and its whole subtree by (@p dx, @p dy): rects are
 *        absolute, so children travel with their parent only this way.
 */
lh_void
lh_ui_entity_move_by(lh_ui_entity_t *self, lh_ui_scalar_t dx, lh_ui_scalar_t dy);

/**
 * @brief ::lh_ui_entity_move_by so the rect of @p self starts at @p origin.
 */
lh_void
lh_ui_entity_move_to(lh_ui_entity_t *self, lh_ui_point_t origin);

/**
 * @brief True when @p self is hidden (paint and hit tests skip it).
 */
lh_bool_t
lh_ui_entity_is_hidden(const lh_ui_entity_t *self);

/**
 * @brief Hide or show @p self. Hidden nodes skip paint and hit tests for the
 *        whole subtree.
 */
lh_void
lh_ui_entity_set_hidden(lh_ui_entity_t *self, lh_bool_t hidden);

/* ── Tree ────────────────────────────────────────────────────────────────── */

/**
 * @brief Parent of @p self, or ::lh_null for a root.
 */
lh_ui_entity_t *
lh_ui_entity_get_parent(const lh_ui_entity_t *self);

/**
 * @brief True when @p ancestor is @p self or one of its parents.
 */
lh_bool_t
lh_ui_entity_is_ancestor(const lh_ui_entity_t *ancestor, const lh_ui_entity_t *self);

/**
 * @brief Append @p child to the children of @p self.
 *
 * Neither owns the other. @p child must have no parent, and must not be
 * @p self or one of its ancestors (that would make a cycle).
 */
lh_void
lh_ui_entity_add_child(lh_ui_entity_t *self, lh_ui_entity_t *child);

/**
 * @brief Unlink @p child from the children of @p self. O(1).
 *
 * @p child must be a child of @p self. Does not free @p child.
 */
lh_void
lh_ui_entity_remove_child(lh_ui_entity_t *self, lh_ui_entity_t *child);

/**
 * @brief First child of @p self, or ::lh_null when it has none.
 */
lh_ui_entity_t *
lh_ui_entity_get_first_child(const lh_ui_entity_t *self);

/**
 * @brief Child after @p child under @p self, or ::lh_null when @p child is last.
 */
lh_ui_entity_t *
lh_ui_entity_get_next_child(const lh_ui_entity_t *self, const lh_ui_entity_t *child);

/**
 * @brief Last child of @p self, or ::lh_null when it has none.
 */
lh_ui_entity_t *
lh_ui_entity_get_last_child(const lh_ui_entity_t *self);

/**
 * @brief Child before @p child under @p self, or ::lh_null when @p child is first.
 */
lh_ui_entity_t *
lh_ui_entity_get_prev_child(const lh_ui_entity_t *self, const lh_ui_entity_t *child);

/**
 * @brief Visit @p self, then each subtree in child order (depth-first).
 *
 * Stops as soon as @p visit returns ::lh_bool_false. Hidden nodes are still
 * visited; callers that care about visibility check ::lh_ui_entity_is_shown.
 *
 * @return ::lh_bool_true when every entity was visited.
 */
lh_bool_t
lh_ui_entity_walk(const lh_ui_entity_t *self, lh_ui_entity_visit_cb visit, lh_ptr context);

/**
 * @brief ::lh_ui_entity_walk over each child of @p self in order, not @p self.
 *
 * @return ::lh_bool_true when every entity was visited.
 */
lh_bool_t
lh_ui_entity_walk_children(const lh_ui_entity_t *self, lh_ui_entity_visit_cb visit, lh_ptr context);

/* ── Class queries ───────────────────────────────────────────────────────── */

/**
 * @brief Send the event @p code with @p context to the class of @p self.
 *
 * The one place an event reaches a class: draw, click and the queries below
 * all go through it.
 */
lh_void
lh_ui_entity_send(const lh_ui_entity_t *self, lh_ui_entity_event_code_t code, lh_ptr context);

/**
 * @brief True when @p self is shown: the one answer draw and hit test use.
 *
 * False when @p self is hidden (::lh_ui_entity_set_hidden, the manual switch
 * that always wins). Otherwise sends ::lh_ui_entity_event_visible to the class
 * with the answer preset to true; the base class leaves it, a class may say
 * no (a scrollbar with nothing to scroll). Not shown means no paint and no
 * hit test for the whole subtree.
 */
lh_bool_t
lh_ui_entity_is_shown(const lh_ui_entity_t *self);

/**
 * @brief True when @p self is shown and its class answers yes to
 *        ::lh_ui_entity_event_focusable (the base class says no).
 */
lh_bool_t
lh_ui_entity_is_focusable(const lh_ui_entity_t *self);

/**
 * @brief True when @p self takes a click meant for itself rather than for what it
 *        holds (:lh_ui_entity_event_clickable). The base class says no, so a plain
 *        entity never steals a click from a child of its own.
 */
lh_bool_t
lh_ui_entity_is_clickable(const lh_ui_entity_t *self);

/**
 * @brief The nearest entity at or above @p self that takes clicks, and @p self when
 *        none of them does: a caption or a picture hands the pointer up to the button
 *        it is in, while a click on the background still arrives where it always did.
 */
lh_ui_entity_t *
lh_ui_entity_click_target(lh_ui_entity_t *self);

/**
 * @brief Nearest focusable entity at or above @p self, or ::lh_null.
 */
lh_ui_entity_t *
lh_ui_entity_find_focusable(lh_ui_entity_t *self);

/**
 * @brief Send the pointer event @p code with @p point (root space) moved into
 *        the rect space of @p self. Nothing for a ::lh_null @p self.
 */
lh_void
lh_ui_entity_send_pointer(const lh_ui_entity_t *self, lh_ui_entity_event_code_t code, lh_ui_point_t point);

/**
 * @brief How @p self places its children: the one answer draw and hit test use.
 *
 * Sends ::lh_ui_entity_event_children to the class with a transform preset to
 * no offset, no clip; the base class leaves it so. A scrolling container
 * answers with its scroll as the offset and asks for the clip.
 *
 * @param offset Receives the offset added to every child rect.
 * @return ::lh_bool_true when the children are cut to the rect of @p self.
 */
lh_bool_t
lh_ui_entity_get_children_transform(const lh_ui_entity_t *self, lh_ui_point_t *offset);

/**
 * @brief Fill @p transform with the defaults, then send
 *        ::lh_ui_entity_event_children to the class of @p self with it.
 */
lh_void
lh_ui_entity_ask_children(const lh_ui_entity_t *self, lh_ui_entity_transform_t *transform);

/* ── Geometry ────────────────────────────────────────────────────────────── */

/**
 * @brief @p bounds grown to cover the rect of @p self; unchanged when @p self
 *        is hidden.
 */
lh_ui_rect_t
lh_ui_entity_extend_bounds(const lh_ui_entity_t *self, lh_ui_rect_t bounds);

/**
 * @brief Union of the rects of the children of @p self that are not hidden;
 *        empty when there are none.
 */
lh_ui_rect_t
lh_ui_entity_get_children_bounds(const lh_ui_entity_t *self);

/**
 * @brief Where @p self has its first baseline: rows down from the top of its own
 *        rect, or -1 when it has none.
 *
 * The line the letters of @p self stand on, which is not its box and not its ink:
 * an icon next to a caption has to sit **on** the caption's line rather than in
 * the same room as it, or the two are centred side by side and only look like one
 * line by accident. Measured on the demo's Hide panel button, the 'x' glyph is 9
 * rows of x-height and the caption is a 12-row cap block: centred in the same
 * 16 rows they land on 194.5 and 194.0, and by their own baselines they land on
 * one line exactly.
 *
 * A class answers this only if its content stands on a line — a label does, from
 * the font's baseline under the text (::lh_ui_label_get_baseline), a picture does
 * when it was told where its baseline is (::lh_ui_image_set_baseline), and the base
 * entity says -1 so an unknown child is centred and nothing breaks. A vertical
 * flow never asks: a baseline runs down a picture, and there is no cross row to
 * put it on.
 */
lh_ui_scalar_t
lh_ui_entity_get_baseline(const lh_ui_entity_t *self);

/**
 * @brief What @p self paints of itself beyond its rect: empty, then sent
 *        ::lh_ui_entity_event_measure for the class to grow (a label's ink, which
 *        hangs below the box it is centred in).
 *
 * The children are not in here — ::lh_ui_entity_get_content_bounds is that union
 * — because the two are culled by different rules: a plain parent must still be
 * culled out of its own draw when its child lies outside it.
 */
lh_ui_rect_t
lh_ui_entity_get_measure_bounds(const lh_ui_entity_t *self);

/**
 * @brief What the content of @p self covers: ::lh_ui_entity_get_children_bounds,
 *        then ::lh_ui_entity_event_measure so the class can grow it with
 *        content that is not a child (a label's text). Empty when nothing.
 *
 * A measure answer must not ask for scroll or visibility (that path calls
 * back into this one).
 */
lh_ui_rect_t
lh_ui_entity_get_content_bounds(const lh_ui_entity_t *self);

/**
 * @brief @p point (in the space of the rect of @p self) moved into the space
 *        of its children: the children offset taken away.
 */
lh_ui_point_t
lh_ui_entity_to_children_space(const lh_ui_entity_t *self, lh_ui_point_t point);

/**
 * @brief @p point (in the space of the rect of @p self) moved out to the space
 *        of the parent: the children offset of @p self added. The inverse of
 *        ::lh_ui_entity_to_children_space.
 */
lh_ui_point_t
lh_ui_entity_add_children_offset(const lh_ui_entity_t *self, lh_ui_point_t point);

/**
 * @brief Sum of the children offsets of every ancestor of @p self: where the
 *        space of the rect of @p self sits in the space of the topmost
 *        ancestor (the root, the canvas target space of a view).
 */
lh_ui_point_t
lh_ui_entity_get_root_offset(const lh_ui_entity_t *self);

/**
 * @brief @p point in the root space moved into the space of the rect of
 *        @p self (::lh_ui_entity_get_root_offset taken away). The same point
 *        ::lh_ui_entity_find_at_local stores for a hit.
 */
lh_ui_point_t
lh_ui_entity_to_local(const lh_ui_entity_t *self, lh_ui_point_t point);

/**
 * @brief The rect of @p self in the root space (moved by
 *        ::lh_ui_entity_get_root_offset).
 */
lh_ui_rect_t
lh_ui_entity_get_root_rect(const lh_ui_entity_t *self);

/* ── Hit test ────────────────────────────────────────────────────────────── */

/**
 * @brief True when @p point lies in the hit shape of @p self
 *        (::lh_ui_entity_contains_point) and @p self is shown.
 */
lh_bool_t
lh_ui_entity_is_hit(const lh_ui_entity_t *self, lh_ui_point_t point);

/**
 * @brief True when a hit test at @p point looks into @p self: inside the shape
 *        it is drawn and clipped with (::lh_ui_entity_get_radius_now), or
 *        outside it when it has children and does not clip them
 *        (::lh_ui_entity_is_clipping) — the same rule draw uses
 *        (::lh_ui_entity_shows_children_on).
 *
 * The drawn shape and not the hit one: a clipping parent must not hand a cut
 * corner to a child, and a plain one must, since a plain parent draws its
 * children outside its rounded corners.
 */
lh_bool_t
lh_ui_entity_searches_children_at(const lh_ui_entity_t *self, lh_ui_point_t point);

/**
 * @brief True when @p self or one of its children can be hit at @p point:
 *        ::lh_ui_entity_is_shown, and either its own target
 *        (::lh_ui_entity_contains_point) or somewhere its children are searched
 *        (::lh_ui_entity_searches_children_at).
 *
 * Two shapes, one question: the entity is hit by the target it declares, its
 * children only where it draws them.
 */
lh_bool_t
lh_ui_entity_may_hit(const lh_ui_entity_t *self, lh_ui_point_t point);

/**
 * @brief ::lh_ui_entity_find_at_local over the children of @p self, last
 *        first, with @p point moved into their space.
 *
 * @return The first hit, or ::lh_null.
 */
lh_ui_entity_t *
lh_ui_entity_find_child_at(const lh_ui_entity_t *self, lh_ui_point_t point, lh_ui_point_t *local);

/**
 * @brief ::lh_ui_entity_find_at that also stores in @p local the point in the
 *        space of the rect of the hit.
 */
lh_ui_entity_t *
lh_ui_entity_find_at_local(lh_ui_entity_t *self, lh_ui_point_t point, lh_ui_point_t *local);

/**
 * @brief Topmost visible entity under @p point in the tree rooted at @p self.
 *
 * Topmost is the deepest, last-drawn entity whose rect contains @p point.
 * Entities not shown (::lh_ui_entity_is_shown) and their subtrees are
 * skipped. A child outside its parent's rect is found where it is drawn:
 * under a plain parent anywhere, under a clipping one (a container) only
 * inside the parent rect (::lh_ui_entity_searches_children_at). Below a
 * parent, @p point is moved back by the parent's children offset
 * (::lh_ui_entity_to_children_space), so a scrolled child is found where it
 * is drawn.
 *
 * @return That entity, or ::lh_null when no shown entity there contains
 *         @p point.
 */
lh_ui_entity_t *
lh_ui_entity_find_at(lh_ui_entity_t *self, lh_ui_point_t point);

/**
 * @brief Hit-test from @p self and send ::lh_ui_entity_event_click to the match.
 *
 * The event carries the point in the space of the match's own rect (every
 * children offset on the way down removed). Returns the entity that received
 * the click, or ::lh_null when nothing hit.
 */
lh_ui_entity_t *
lh_ui_entity_click(lh_ui_entity_t *self, lh_ui_point_t point);

/* ── Draw ────────────────────────────────────────────────────────────────── */

/**
 * @brief Push the children transform of @p self on @p canvas, when it does
 *        anything: its offset, and the rect of @p self as clip when asked —
 *        rounded by ::lh_ui_entity_get_radius (::lh_ui_canvas_push_round), so
 *        children are cut along the same corners the fill has.
 *
 * @return ::lh_bool_true when something was pushed (the caller pops it);
 *         never with a ::lh_null @p canvas.
 */
lh_bool_t
lh_ui_entity_push_children(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief True when @p self cuts its children to its rect
 *        (::lh_ui_entity_get_children_transform asks for the clip).
 */
lh_bool_t
lh_ui_entity_is_clipping(const lh_ui_entity_t *self);

/**
 * @brief What @p self paints: its rect grown by the outset of the shadow of the
 *        style it is painted with right now (::lh_ui_entity_get_style_now, so a
 *        pressed look's shadow counts while it is down).
 *
 * The rect on its own is what ::lh_ui_view_damage_looks grows before it records a
 * change, and it has to be what the cull grows too: a shadow reaches past its own
 * box, so an entity culled by the rect alone is thrown away in a frame clipped to
 * its fringe, and the frame leaves the ground bare where the whole frame draws the
 * shadow. Empty when @p self casts none.
 */
lh_ui_rect_t
lh_ui_entity_get_painted_rect(const lh_ui_entity_t *self);

/**
 * @brief True when what @p self paints (::lh_ui_entity_get_painted_rect) meets the
 *        clip of @p canvas; always true with a ::lh_null @p canvas (nothing to
 *        cull against). The one culling test of the draw walk.
 */
lh_bool_t
lh_ui_entity_shows_on(const lh_ui_entity_t *self, const lh_ui_canvas_t *canvas);

/**
 * @brief True when @p self has children and they can show on @p canvas: a
 *        clipping parent (::lh_ui_entity_is_clipping) must show itself; a plain
 *        one never culls them, since its children may lie outside its rect.
 */
lh_bool_t
lh_ui_entity_shows_children_on(const lh_ui_entity_t *self, const lh_ui_canvas_t *canvas);

/**
 * @brief ::lh_ui_entity_draw on each child of @p self, in child order.
 */
lh_void
lh_ui_entity_draw_each_child(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief Draw each child of @p self in order inside one
 *        ::lh_ui_entity_push_children / ::lh_ui_canvas_pop. Nothing when
 *        ::lh_ui_entity_shows_children_on says no.
 */
lh_void
lh_ui_entity_draw_children(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief Send ::lh_ui_entity_event_draw with @p canvas to @p self alone, when
 *        ::lh_ui_entity_shows_on.
 */
lh_void
lh_ui_entity_draw_self(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief Send ::lh_ui_entity_event_draw with @p canvas to @p self, then to
 *        each subtree in child order. @p self not shown
 *        (::lh_ui_entity_is_shown) returns without drawing.
 *
 * The base class (::lh_ui_entity_class) fills the rect with
 * ::lh_ui_entity_get_fill_color. Children are drawn inside one
 * ::lh_ui_canvas_push / ::lh_ui_canvas_pop with the offset (and, when asked,
 * the rect of @p self as clip) from ::lh_ui_entity_get_children_transform; no
 * push when there is neither. @p self outside the clip skips its own draw
 * event; its children are skipped too only when @p self clips them. @p canvas
 * may be ::lh_null: the events are still sent, nothing is painted by the base
 * class.
 */
lh_void
lh_ui_entity_draw(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief Union the rect of @p self, in the root space
 *        (::lh_ui_entity_get_root_rect), into the damage of @p canvas.
 *
 * Nothing when @p self or @p canvas is ::lh_null. The one place an entity
 * marks itself dirty on a canvas.
 */
lh_void
lh_ui_entity_add_damage(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_H */
