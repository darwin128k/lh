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
                        struct lh_ui_entity);
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
 * @brief Class of @p self.
 */
const lh_ui_entity_class_t *
lh_ui_entity_get_class(const lh_ui_entity_t *self);

/**
 * @brief Point @p self at @p class. The class is not copied.
 */
lh_void
lh_ui_entity_set_class(lh_ui_entity_t *self, const lh_ui_entity_class_t *);

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

/* ── Hit test ────────────────────────────────────────────────────────────── */

/**
 * @brief True when the rect of @p self contains @p point and @p self is shown.
 */
lh_bool_t
lh_ui_entity_is_hit(const lh_ui_entity_t *self, lh_ui_point_t point);

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
 * skipped. Children outside their parent's rect are not searched. Below a
 * parent, @p point is moved back by the parent's children offset
 * (::lh_ui_entity_to_children_space), so a scrolled child is found where it
 * is drawn.
 *
 * @return That entity, or ::lh_null when @p point is outside @p self.
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
 *        anything: its offset, and the rect of @p self as clip when asked.
 *
 * @return ::lh_bool_true when something was pushed (the caller pops it);
 *         never with a ::lh_null @p canvas.
 */
lh_bool_t
lh_ui_entity_push_children(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief Draw each child of @p self in order inside one
 *        ::lh_ui_entity_push_children / ::lh_ui_canvas_pop.
 */
lh_void
lh_ui_entity_draw_children(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief Send ::lh_ui_entity_event_draw with @p canvas to @p self, then to
 *        each subtree in child order. @p self not shown
 *        (::lh_ui_entity_is_shown) returns without drawing.
 *
 * The base class (::lh_ui_entity_class) fills the rect with
 * ::lh_ui_entity_get_fill_color. Children are drawn inside one
 * ::lh_ui_canvas_push / ::lh_ui_canvas_pop with the offset (and, when asked,
 * the rect of @p self as clip) from ::lh_ui_entity_get_children_transform; no
 * push when there is neither. @p canvas may be ::lh_null: the events are
 * still sent, nothing is painted by the base class.
 */
lh_void
lh_ui_entity_draw(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief Union the rect of @p self into the damage of @p canvas.
 *
 * Nothing when @p self or @p canvas is ::lh_null. The one place an entity
 * marks itself dirty on a canvas.
 */
lh_void
lh_ui_entity_add_damage(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_H */
