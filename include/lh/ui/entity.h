/**
 * @file entity.h
 * @brief One object in a UI tree: ::lh_ui_entity_t.
 *
 * An entity covers a ::lh_ui_rect_t, may share a ::lh_ui_style_t, and points
 * at a class whose event function is shared by every instance of that kind.
 * Larger widgets are built by linking children (composition), not by copying
 * getters onto every specialized type. Hidden entities (and their descendants)
 * skip paint and hit tests. ::lh_ui_entity_draw walks the tree and sends
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
 * visited; callers that care about visibility check ::lh_ui_entity_is_hidden.
 *
 * @return ::lh_bool_true when every entity was visited.
 */
lh_bool_t
lh_ui_entity_walk(const lh_ui_entity_t *self, lh_ui_entity_visit_cb visit, lh_ptr context);

/**
 * @brief Topmost visible entity under @p point in the tree rooted at @p self.
 *
 * Topmost is the deepest, last-drawn entity whose rect contains @p point.
 * Hidden nodes (and their subtrees) are skipped. Children outside their
 * parent's rect are not searched.
 *
 * @return That entity, or ::lh_null when @p point is outside @p self.
 */
lh_ui_entity_t *
lh_ui_entity_find_at(lh_ui_entity_t *self, lh_ui_point_t point);

/**
 * @brief Hit-test from @p self and send ::lh_ui_entity_event_click to the match.
 *
 * Returns the entity that received the click, or ::lh_null when nothing hit.
 */
lh_ui_entity_t *
lh_ui_entity_click(lh_ui_entity_t *self, lh_ui_point_t point);

/* ── Draw ────────────────────────────────────────────────────────────────── */

/**
 * @brief Event of ::lh_ui_entity_class.
 */
lh_void
lh_ui_entity_face_rect(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief Send ::lh_ui_entity_event_draw with @p canvas to @p self, then to
 *        each subtree in child order. Hidden @p self returns without drawing.
 *
 * @p canvas may be ::lh_null: the events are still sent, nothing is painted
 * by the base class.
 */
lh_void
lh_ui_entity_draw(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_H */
