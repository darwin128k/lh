/**
 * @file entity.c
 * @brief Implementation of `lh/ui/entity.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/list.h>
#include <lh/list/node.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/canvas.h>
#include <lh/ui/entity.h>
#include <lh/ui/point.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

lh_void
lh_ui_entity_init(lh_ui_entity_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
    self->style = lh_null;
    self->class = lh_addr_of(lh_ui_entity_class);
    self->hidden = lh_bool_false;
    self->pressed = lh_bool_false;
    self->parent = lh_null;
    lh_list_init(lh_addr_of(self->children));
    lh_list_node_init(lh_addr_of(self->link));
}

lh_ui_rect_t
lh_ui_entity_get_rect(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rect;
}

lh_void
lh_ui_entity_set_rect(lh_ui_entity_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
}

const lh_ui_style_t *
lh_ui_entity_get_style(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_ui_entity_set_style(lh_ui_entity_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->style = style;
}

const lh_ui_color_t *
lh_ui_entity_get_fill_color(const lh_ui_entity_t *self)
{
    const lh_ui_style_t *style;

    lh_assert_runtime_ref(self);
    style = lh_ui_entity_get_style_now(self);
    if (lh_null_eq(style))
    {
        return lh_null;
    }
    return lh_ui_style_get_fill_color(style);
}

lh_bool_t
lh_ui_entity_is_pressed(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

lh_void
lh_ui_entity_set_pressed(lh_ui_entity_t *self, lh_bool_t pressed)
{
    lh_assert_runtime_ref(self);
    self->pressed = pressed;
}

const lh_ui_style_t *
lh_ui_entity_get_style_now(const lh_ui_entity_t *self)
{
    const lh_ui_style_t *style;

    lh_assert_runtime_ref(self);
    style = self->style;
    /* Painting is the only thing the pressed style is asked for, so it cannot
       change where anything lands. What a press *hits* keeps to the own radius
       (::lh_ui_entity_contains_point): a hit test runs before the press that
       asked for it, so a shape read from the pressed flag would answer one
       frame too late. */
    if (!self->pressed || lh_null_eq(style))
    {
        return style;
    }
    return lh_null_eq(lh_ui_style_get_pressed(style)) ? style : lh_ui_style_get_pressed(style);
}

lh_ui_scalar_t
lh_ui_entity_get_radius(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_null_eq(self->style) ? lh_ui_scalar(0) : lh_ui_style_get_radius(self->style);
}

lh_ui_scalar_t
lh_ui_entity_get_radius_now(const lh_ui_entity_t *self)
{
    const lh_ui_style_t *style;

    lh_assert_runtime_ref(self);
    style = lh_ui_entity_get_style_now(self);
    return lh_null_eq(style) ? lh_ui_scalar(0) : lh_ui_style_get_radius(style);
}

lh_ui_insets_t
lh_ui_entity_get_padding(const lh_ui_entity_t *self)
{
    lh_ui_insets_t none;

    lh_assert_runtime_ref(self);
    lh_ui_insets_init_all(lh_addr_of(none), lh_ui_scalar(0));
    return lh_null_eq(self->style) ? none : *lh_ui_style_get_padding(self->style);
}

lh_bool_t
lh_ui_entity_contains_point(const lh_ui_entity_t *self, lh_ui_point_t point)
{
    return lh_ui_radius_contains(lh_addr_of(self->rect), lh_ui_entity_get_radius(self), point);
}

const lh_ui_entity_class_t *
lh_ui_entity_get_class(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->class;
}

lh_void
lh_ui_entity_set_class(lh_ui_entity_t *self, const lh_ui_entity_class_t *class)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(class);
    lh_assert_runtime_ref(class->event);
    self->class = class;
}

lh_void
lh_ui_entity_move_by(lh_ui_entity_t *self, lh_ui_scalar_t dx, lh_ui_scalar_t dy)
{
    lh_ui_entity_t *child;

    lh_assert_runtime_ref(self);
    self->rect = lh_ui_rect_offset(lh_addr_of(self->rect), dx, dy);
    for (child = lh_ui_entity_get_first_child(self); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(self, child))
    {
        lh_ui_entity_move_by(child, dx, dy);
    }
}

lh_void
lh_ui_entity_move_to(lh_ui_entity_t *self, lh_ui_point_t origin)
{
    const lh_ui_point_t *from = lh_ui_rect_get_origin_as_const(lh_addr_of(self->rect));

    lh_ui_entity_move_by(self, lh_ui_point_get_x(lh_addr_of(origin)) - lh_ui_point_get_x(from),
                         lh_ui_point_get_y(lh_addr_of(origin)) - lh_ui_point_get_y(from));
}

lh_bool_t
lh_ui_entity_is_hidden(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hidden;
}

lh_void
lh_ui_entity_set_hidden(lh_ui_entity_t *self, lh_bool_t hidden)
{
    lh_assert_runtime_ref(self);
    self->hidden = hidden;
}

lh_ui_entity_t *
lh_ui_entity_get_parent(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->parent;
}

lh_bool_t
lh_ui_entity_is_ancestor(const lh_ui_entity_t *ancestor, const lh_ui_entity_t *self)
{
    const lh_ui_entity_t *walk = self;

    while (lh_null_ne(walk) && walk != ancestor)
    {
        walk = walk->parent;
    }
    return lh_null_ne(walk) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_entity_add_child(lh_ui_entity_t *self, lh_ui_entity_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    lh_assert_runtime_if(self == child, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_addr_of(child->link)),
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_null_ne(child->parent), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_ui_entity_is_ancestor(child, self),
                         lh_runtime_error_code_invalid_argument);
    child->parent = self;
    lh_list_push_back(lh_addr_of(self->children), lh_addr_of(child->link));
}

lh_void
lh_ui_entity_remove_child(lh_ui_entity_t *self, lh_ui_entity_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    lh_assert_runtime_ifn(child->parent == self, lh_runtime_error_code_invalid_argument);
    lh_list_node_unlink(lh_addr_of(child->link));
    child->parent = lh_null;
}

lh_ui_entity_t *
lh_ui_entity_get_first_child(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_list_entry(lh_ui_entity_t, link, lh_list_get_first(lh_addr_of(self->children)));
}

lh_ui_entity_t *
lh_ui_entity_get_next_child(const lh_ui_entity_t *self, const lh_ui_entity_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    return lh_list_entry(lh_ui_entity_t, link,
                         lh_list_get_next(lh_addr_of(self->children), lh_addr_of(child->link)));
}

lh_ui_entity_t *
lh_ui_entity_get_last_child(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_list_entry(lh_ui_entity_t, link, lh_list_get_last(lh_addr_of(self->children)));
}

lh_ui_entity_t *
lh_ui_entity_get_prev_child(const lh_ui_entity_t *self, const lh_ui_entity_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    return lh_list_entry(lh_ui_entity_t, link,
                         lh_list_get_prev(lh_addr_of(self->children), lh_addr_of(child->link)));
}

lh_bool_t
lh_ui_entity_walk(const lh_ui_entity_t *self, lh_ui_entity_visit_cb visit, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(visit);
    lh_return_if(!visit(self, context), lh_bool_false);
    return lh_ui_entity_walk_children(self, visit, context);
}

lh_bool_t
lh_ui_entity_walk_children(const lh_ui_entity_t *self, lh_ui_entity_visit_cb visit, lh_ptr context)
{
    lh_ui_entity_t *child;
    lh_bool_t all = lh_bool_true;

    lh_assert_runtime_ref(self);
    for (child = lh_ui_entity_get_first_child(self); all && lh_null_ne(child);
         child = lh_ui_entity_get_next_child(self, child))
    {
        all = lh_ui_entity_walk(child, visit, context);
    }
    return all;
}

/* ── Class queries ───────────────────────────────────────────────────────── */

lh_void
lh_ui_entity_send(const lh_ui_entity_t *self, lh_ui_entity_event_code_t code, lh_ptr context)
{
    lh_ui_entity_event_t event;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(self->class);
    lh_ui_entity_event_init(lh_addr_of(event), code, context);
    self->class->event(self, lh_addr_of(event));
}

lh_bool_t
lh_ui_entity_is_shown(const lh_ui_entity_t *self)
{
    lh_bool_t visible = lh_bool_true;

    lh_assert_runtime_ref(self);
    lh_return_if(self->hidden, lh_bool_false);
    lh_ui_entity_send(self, lh_ui_entity_event_visible, lh_addr_of(visible));
    return visible;
}

lh_bool_t
lh_ui_entity_is_focusable(const lh_ui_entity_t *self)
{
    lh_bool_t focusable = lh_bool_false;

    lh_return_if(!lh_ui_entity_is_shown(self), lh_bool_false);
    lh_ui_entity_send(self, lh_ui_entity_event_focusable, lh_addr_of(focusable));
    return focusable;
}

lh_void
lh_ui_entity_send_pointer(const lh_ui_entity_t *self, lh_ui_entity_event_code_t code, lh_ui_point_t point)
{
    lh_ui_point_t local;

    lh_return_if(lh_null_eq(self));
    local = lh_ui_entity_to_local(self, point);
    lh_ui_entity_send(self, code, lh_addr_of(local));
}

lh_ui_entity_t *
lh_ui_entity_find_focusable(lh_ui_entity_t *self)
{
    for (; lh_null_ne(self); self = self->parent)
    {
        lh_return_if(lh_ui_entity_is_focusable(self), self);
    }
    return lh_null;
}

lh_void
lh_ui_entity_ask_children(const lh_ui_entity_t *self, lh_ui_entity_transform_t *transform)
{
    lh_ui_entity_transform_init(transform);
    lh_ui_entity_send(self, lh_ui_entity_event_children, transform);
}

lh_bool_t
lh_ui_entity_get_children_transform(const lh_ui_entity_t *self, lh_ui_point_t *offset)
{
    lh_ui_entity_transform_t transform;

    lh_assert_runtime_ref(offset);
    lh_ui_entity_ask_children(self, lh_addr_of(transform));
    *offset = lh_ui_entity_transform_get_offset(lh_addr_of(transform));
    return lh_ui_entity_transform_is_clip(lh_addr_of(transform));
}

/* ── Geometry ────────────────────────────────────────────────────────────── */

lh_ui_rect_t
lh_ui_entity_extend_bounds(const lh_ui_entity_t *self, lh_ui_rect_t bounds)
{
    lh_assert_runtime_ref(self);
    lh_return_if(self->hidden, bounds);
    return lh_ui_rect_union(lh_addr_of(bounds), lh_addr_of(self->rect));
}

lh_ui_rect_t
lh_ui_entity_get_children_bounds(const lh_ui_entity_t *self)
{
    const lh_ui_entity_t *child;
    lh_ui_rect_t bounds;

    lh_assert_runtime_ref(self);
    lh_ui_rect_init_empty(lh_addr_of(bounds));
    for (child = lh_ui_entity_get_first_child(self); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(self, child))
    {
        bounds = lh_ui_entity_extend_bounds(child, bounds);
    }
    return bounds;
}

lh_ui_rect_t
lh_ui_entity_get_content_bounds(const lh_ui_entity_t *self)
{
    lh_ui_rect_t bounds = lh_ui_entity_get_children_bounds(self);

    lh_ui_entity_send(self, lh_ui_entity_event_measure, lh_addr_of(bounds));
    return bounds;
}

lh_ui_point_t
lh_ui_entity_to_children_space(const lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_ui_point_t offset;

    (void)lh_ui_entity_get_children_transform(self, lh_addr_of(offset));
    return lh_ui_point_offset(lh_addr_of(point), -lh_ui_point_get_x(lh_addr_of(offset)),
                              -lh_ui_point_get_y(lh_addr_of(offset)));
}

lh_ui_point_t
lh_ui_entity_add_children_offset(const lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_ui_point_t offset;

    (void)lh_ui_entity_get_children_transform(self, lh_addr_of(offset));
    return lh_ui_point_offset(lh_addr_of(point), lh_ui_point_get_x(lh_addr_of(offset)),
                              lh_ui_point_get_y(lh_addr_of(offset)));
}

lh_ui_point_t
lh_ui_entity_get_root_offset(const lh_ui_entity_t *self)
{
    lh_ui_point_t total;

    lh_assert_runtime_ref(self);
    lh_ui_point_init(lh_addr_of(total), lh_ui_scalar(0), lh_ui_scalar(0));
    for (self = self->parent; lh_null_ne(self); self = self->parent)
    {
        total = lh_ui_entity_add_children_offset(self, total);
    }
    return total;
}

lh_ui_point_t
lh_ui_entity_to_local(const lh_ui_entity_t *self, lh_ui_point_t point)
{
    const lh_ui_point_t offset = lh_ui_entity_get_root_offset(self);

    return lh_ui_point_offset(lh_addr_of(point), -lh_ui_point_get_x(lh_addr_of(offset)),
                              -lh_ui_point_get_y(lh_addr_of(offset)));
}

lh_ui_rect_t
lh_ui_entity_get_root_rect(const lh_ui_entity_t *self)
{
    const lh_ui_point_t offset = lh_ui_entity_get_root_offset(self);

    return lh_ui_rect_offset(lh_addr_of(self->rect), lh_ui_point_get_x(lh_addr_of(offset)),
                             lh_ui_point_get_y(lh_addr_of(offset)));
}

/* ── Hit test ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_entity_is_hit(const lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_return_if(!lh_ui_entity_contains_point(self, point), lh_bool_false);
    return lh_ui_entity_is_shown(self);
}

lh_ui_entity_t *
lh_ui_entity_find_child_at(const lh_ui_entity_t *self, lh_ui_point_t point, lh_ui_point_t *local)
{
    lh_ui_entity_t *child;
    lh_ui_entity_t *hit = lh_null;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(lh_ui_entity_get_last_child(self)), lh_null);
    point = lh_ui_entity_to_children_space(self, point);
    for (child = lh_ui_entity_get_last_child(self); lh_null_eq(hit) && lh_null_ne(child);
         child = lh_ui_entity_get_prev_child(self, child))
    {
        hit = lh_ui_entity_find_at_local(child, point, local);
    }
    return hit;
}

lh_bool_t
lh_ui_entity_searches_children_at(const lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_ui_entity_contains_point(self, point), lh_bool_true);
    /* Outside: only children could be hit, and asking about the clip may measure content. */
    lh_return_if(lh_null_eq(lh_ui_entity_get_first_child(self)), lh_bool_false);
    return !lh_ui_entity_is_clipping(self) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_ui_entity_may_hit(const lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_return_if(!lh_ui_entity_searches_children_at(self, point), lh_bool_false);
    return lh_ui_entity_is_shown(self);
}

lh_ui_entity_t *
lh_ui_entity_find_at_local(lh_ui_entity_t *self, lh_ui_point_t point, lh_ui_point_t *local)
{
    lh_ui_entity_t *hit;

    lh_assert_runtime_ref(local);
    lh_return_if(!lh_ui_entity_may_hit(self, point), lh_null);
    hit = lh_ui_entity_find_child_at(self, point, local);
    lh_return_if(lh_null_ne(hit), hit);
    lh_return_if(!lh_ui_entity_contains_point(self, point), lh_null);
    *local = point;
    return self;
}

lh_ui_entity_t *
lh_ui_entity_find_at(lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_ui_point_t local;

    return lh_ui_entity_find_at_local(self, point, lh_addr_of(local));
}

lh_ui_entity_t *
lh_ui_entity_click(lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_ui_entity_t *hit;
    lh_ui_point_t local;

    hit = lh_ui_entity_find_at_local(self, point, lh_addr_of(local));
    lh_return_if(lh_null_eq(hit), lh_null);
    lh_ui_entity_send(hit, lh_ui_entity_event_click, lh_addr_of(local));
    return hit;
}

/* ── Draw ────────────────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_entity_push_children(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    lh_ui_entity_transform_t transform;
    lh_ui_rect_t rect;

    lh_return_if(lh_null_eq(canvas), lh_bool_false);
    lh_ui_entity_ask_children(self, lh_addr_of(transform));
    lh_return_if(lh_ui_entity_transform_is_identity(lh_addr_of(transform)), lh_bool_false);
    rect = self->rect;
    lh_ui_canvas_push_round(canvas, lh_ui_entity_transform_get_offset(lh_addr_of(transform)),
                            lh_ui_entity_transform_is_clip(lh_addr_of(transform)) ? lh_addr_of(rect) : lh_null,
                            lh_ui_entity_get_radius_now(self));
    return lh_bool_true;
}

lh_bool_t
lh_ui_entity_is_clipping(const lh_ui_entity_t *self)
{
    lh_ui_point_t offset;

    return lh_ui_entity_get_children_transform(self, lh_addr_of(offset));
}

lh_bool_t
lh_ui_entity_shows_on(const lh_ui_entity_t *self, const lh_ui_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(canvas), lh_bool_true);
    return lh_ui_canvas_shows_rect(canvas, lh_addr_of(self->rect));
}

lh_bool_t
lh_ui_entity_shows_children_on(const lh_ui_entity_t *self, const lh_ui_canvas_t *canvas)
{
    lh_return_if(lh_null_eq(lh_ui_entity_get_first_child(self)), lh_bool_false);
    /* The rect test is cheap; the clip question may measure content. */
    lh_return_if(lh_ui_entity_shows_on(self, canvas), lh_bool_true);
    return !lh_ui_entity_is_clipping(self) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_entity_draw_each_child(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    lh_ui_entity_t *child;

    for (child = lh_ui_entity_get_first_child(self); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(self, child))
    {
        lh_ui_entity_draw(child, canvas);
    }
}

lh_void
lh_ui_entity_draw_children(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    lh_bool_t pushed;

    lh_return_if(!lh_ui_entity_shows_children_on(self, canvas));
    pushed = lh_ui_entity_push_children(self, canvas);
    lh_ui_entity_draw_each_child(self, canvas);
    lh_return_if(!pushed);
    lh_ui_canvas_pop(canvas);
}

lh_void
lh_ui_entity_draw_self(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    lh_return_if(!lh_ui_entity_shows_on(self, canvas));
    lh_ui_entity_send(self, lh_ui_entity_event_draw, canvas);
}

lh_void
lh_ui_entity_draw(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    lh_return_if(!lh_ui_entity_is_shown(self));
    lh_ui_entity_draw_self(self, canvas);
    lh_ui_entity_draw_children(self, canvas);
}

lh_void
lh_ui_entity_add_damage(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    lh_ui_rect_t rect;

    lh_return_if(lh_null_eq(self) || lh_null_eq(canvas));
    rect = lh_ui_entity_get_root_rect(self);
    lh_ui_canvas_add_damage(canvas, lh_addr_of(rect));
}
