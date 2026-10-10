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
#include <lh/ui/insets.h>
#include <lh/ui/paint.h>
#include <lh/ui/point.h>
#include <lh/ui/shadow.h>
#include <lh/ui/size.h>
#include <lh/ui/style.h>
#include <lh/ui/view.h>
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
    self->view = lh_null;
    lh_ui_place_init(lh_addr_of(self->place), lh_ui_place_size_fixed, lh_ui_scalar(0));
    lh_list_init(lh_addr_of(self->children));
    lh_list_node_init(lh_addr_of(self->link));
}

lh_ui_rect_t
lh_ui_entity_get_rect(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rect;
}

const lh_ui_place_t *
lh_ui_entity_get_place(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->place);
}

lh_void
lh_ui_entity_set_place(lh_ui_entity_t *self, const lh_ui_place_t *place)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(place));
    self->place = *place;
}

lh_void
lh_ui_entity_set_rect(lh_ui_entity_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    /* The same rect is what a layout writes on every poll. Recording it would
       paint the whole table to put each row back where it already is. */
    lh_return_if(lh_ui_rect_eq(lh_addr_of(self->rect), lh_addr_of(rect)));
    lh_ui_entity_note(self);
    self->rect = rect;
    lh_ui_entity_note(self);
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
    /* A shared style switched to itself is the steady state of a card that was
       already selected. Both looks are recorded: a shadow that only the old
       one casts would otherwise stay on the surface. */
    lh_return_if(self->style == style);
    lh_ui_entity_note(self);
    self->style = style;
    lh_ui_entity_note(self);
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

lh_ui_scalar_t
lh_ui_entity_get_hit_radius(const lh_ui_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_null_eq(self->style) ? lh_ui_scalar(0) : lh_ui_style_get_hit_radius(self->style);
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
    return lh_ui_radius_contains(lh_addr_of(self->rect), lh_ui_entity_get_hit_radius(self), point);
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
    lh_return_if(dx == lh_ui_scalar(0) && dy == lh_ui_scalar(0));
    lh_ui_entity_note(self);
    self->rect = lh_ui_rect_offset(lh_addr_of(self->rect), dx, dy);
    for (child = lh_ui_entity_get_first_child(self); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(self, child))
    {
        lh_ui_entity_move_by(child, dx, dy);
    }
    lh_ui_entity_note(self);
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
    lh_return_if(self->hidden == hidden);
    /* Hide records the rect that is still on screen. Show records the rect
       that is about to be: noting a hidden node is a no
       (::lh_ui_entity_note), so the flag has to change first. */
    if (hidden)
    {
        lh_ui_entity_note(self);
        self->hidden = hidden;
        return;
    }
    self->hidden = hidden;
    lh_ui_entity_note(self);
}

static lh_bool_t
lh_ui_entity_chain_shown(const lh_ui_entity_t *self)
{
    const lh_ui_entity_t *walk;

    for (walk = self; lh_null_ne(walk); walk = walk->parent)
    {
        if (walk->hidden)
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

static struct lh_ui_view *
lh_ui_entity_own_view(const lh_ui_entity_t *self)
{
    const lh_ui_entity_t *top = self;

    while (lh_null_ne(top->parent))
    {
        top = top->parent;
    }
    return top->view;
}

lh_void
lh_ui_entity_note(const lh_ui_entity_t *self)
{
    struct lh_ui_view *view;
    lh_ui_rect_t rect;
    lh_ui_point_t offset;

    lh_return_if(lh_null_eq(self));
    lh_return_if(!lh_ui_entity_chain_shown(self));
    view = lh_ui_entity_own_view(self);
    lh_return_if(lh_null_eq(view));
    rect = lh_ui_entity_get_painted_rect(self);
    offset = lh_ui_entity_get_root_offset(self);
    rect = lh_ui_rect_offset(lh_addr_of(rect), lh_ui_point_get_x(lh_addr_of(offset)),
                             lh_ui_point_get_y(lh_addr_of(offset)));
    lh_ui_view_add_damage(view, lh_addr_of(rect));
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

lh_bool_t
lh_ui_entity_is_clickable(const lh_ui_entity_t *self)
{
    lh_bool_t clickable = lh_bool_false;

    lh_return_if(!lh_ui_entity_is_shown(self), lh_bool_false);
    lh_ui_entity_send(self, lh_ui_entity_event_clickable, lh_addr_of(clickable));
    return clickable;
}

lh_ui_entity_t *
lh_ui_entity_click_target(lh_ui_entity_t *self)
{
    lh_ui_entity_t *node = self;

    /* Up the tree, the same walk the focus takes: the nearest thing that takes the
       pointer. What is under the pointer may be a caption, a picture or a row, and
       whether that is the whole of the click or only where it landed is not a
       question about geometry — it is a question about who is clickable. */
    for (; lh_null_ne(node); node = node->parent)
    {
        lh_return_if(lh_ui_entity_is_clickable(node), node);
    }
    /* Nobody above claims it, so the click lands where it was pointed — on the
       background, or on something an app wants to hear about. */
    return self;
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

lh_ui_scalar_t
lh_ui_entity_get_baseline(const lh_ui_entity_t *self)
{
    lh_ui_scalar_t baseline = lh_ui_scalar(-1);

    lh_assert_runtime_ref(self);
    /* No line to stand on, so a row that asks falls back to ::lh_ui_place_align_center.
       A base entity with a fill has nothing to be the baseline of. */
    lh_ui_entity_send(self, lh_ui_entity_event_baseline, lh_addr_of(baseline));
    return baseline;
}

lh_ui_rect_t
lh_ui_entity_get_measure_bounds(const lh_ui_entity_t *self)
{
    lh_ui_rect_t bounds;

    lh_assert_runtime_ref(self);
    lh_ui_rect_init_empty(lh_addr_of(bounds));
    lh_ui_entity_send(self, lh_ui_entity_event_measure, lh_addr_of(bounds));
    return bounds;
}

lh_ui_rect_t
lh_ui_entity_get_content_bounds(const lh_ui_entity_t *self)
{
    const lh_ui_rect_t measured = lh_ui_entity_get_measure_bounds(self);
    const lh_ui_rect_t children = lh_ui_entity_get_children_bounds(self);

    return lh_ui_rect_union(lh_addr_of(measured), lh_addr_of(children));
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
    lh_return_if(!lh_ui_entity_searches_children_at(self, point), lh_null);
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
    /* The shape it is drawn and clipped with, not the shape it is hit with: a
       clipping parent must not hand its cut corner to a child, and a plain one
       must, because a plain parent draws children outside its rounded shape. */
    lh_return_if(lh_ui_radius_contains(lh_addr_of(self->rect), lh_ui_entity_get_radius_now(self), point),
                 lh_bool_true);
    /* Outside: only children could be hit, and asking about the clip may measure content. */
    lh_return_if(lh_null_eq(lh_ui_entity_get_first_child(self)), lh_bool_false);
    return !lh_ui_entity_is_clipping(self) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_ui_entity_may_hit(const lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_return_if(!lh_ui_entity_is_shown(self), lh_bool_false);
    /* Its own target, or somewhere a child may be. Not one gate over both: the
       shape a clipping parent draws is what may hand out its children, while the
       entity itself is hit by its own target, which by default is the whole
       rect. Gating the entity on the drawn shape instead would make
       ::lh_ui_style_set_hit_radius unreachable — a rounded entity could never
       be pressed in the corner it does not paint. */
    return lh_ui_entity_contains_point(self, point) || lh_ui_entity_searches_children_at(self, point);
}

lh_ui_entity_t *
lh_ui_entity_find_at_local(lh_ui_entity_t *self, lh_ui_point_t point, lh_ui_point_t *local)
{
    lh_ui_entity_t *hit;

    lh_assert_runtime_ref(local);
    lh_return_if(!lh_ui_entity_is_shown(self), lh_null);
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
    lh_ui_entity_t *target;
    lh_ui_point_t local;

    hit = lh_ui_entity_find_at(self, point);
    lh_return_if(lh_null_eq(hit), lh_null);
    /* What is under the pointer is where the click landed; who takes it is asked
       separately, so a click on a button's own caption or picture reaches the button
       (::lh_ui_entity_click_target). The point is moved into the space of the node
       that receives it — an ancestor has a children offset of its own. */
    target = lh_ui_entity_click_target(hit);
    lh_return_if(lh_null_eq(target), lh_null);
    local = lh_ui_entity_to_local(target, point);
    lh_ui_entity_send(target, lh_ui_entity_event_click, lh_addr_of(local));
    return target;
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

/* What an entity paints is not only its rect: a shadow reaches past the box of
   the thing casting it — the panel card in the demo drops one 18 rows below its own
   box — and the damage side has said so for a while (::lh_ui_view_damage_looks
   grows the rect by the outset of the shadow). The cull side did not, so a frame
   clipped to the fringe threw the whole card away and left the ground bare exactly
   where the full frame draws the shadow; it showed only in the corners of a
   rounded widget resting on the fringe, which are the one place nothing else
   covers it. The two answers have to be one thing, so this is what the draw walk
   tests and the damage records. */
lh_ui_rect_t
lh_ui_entity_get_painted_rect(const lh_ui_entity_t *self)
{
    const lh_ui_style_t *style = lh_ui_entity_get_style_now(self);
    const lh_ui_scalar_t outset =
        lh_null_eq(style) ? lh_ui_scalar(0)
                          : lh_ui_shadow_get_outset(lh_ui_style_get_shadow(style), lh_addr_of(self->rect));

    return lh_ui_rect_inset(lh_addr_of(self->rect), lh_math_neg(outset), lh_math_neg(outset));
}

lh_bool_t
lh_ui_entity_shows_on(const lh_ui_entity_t *self, const lh_ui_canvas_t *canvas)
{
    lh_ui_rect_t painted;
    lh_ui_rect_t content;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(canvas), lh_bool_true);
    painted = lh_ui_entity_get_painted_rect(self);
    lh_return_if(lh_ui_canvas_shows_rect(canvas, lh_addr_of(painted)), lh_bool_true);
    /* The rect misses the clip, and measuring is not free: ask what the class
       paints of itself only now, when the cheap answer said no. A label's box is
       the cap line to the baseline (::lh_ui_text_get_size) while its ink hangs
       below it, so a clip that catches the tail of a 'p' catches no part of the
       box and would have dropped the whole label — the same defect as the shadow
       above, one layer down. Children are not asked about here: they are
       ::lh_ui_entity_shows_children_on's business, and a plain parent whose child
       lies outside it must still be culled out of its own draw. */
    content = lh_ui_entity_get_measure_bounds(self);
    return lh_ui_canvas_shows_rect(canvas, lh_addr_of(content));
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

static lh_void
lh_ui_entity_fill_edge(lh_ui_canvas_t *canvas, lh_ui_scalar_t x, lh_ui_scalar_t y, lh_ui_scalar_t width,
                       lh_ui_scalar_t height, const lh_ui_color_t *color)
{
    lh_ui_rect_t strip;

    lh_return_if(width <= lh_ui_scalar(0) || height <= lh_ui_scalar(0));
    lh_ui_rect_init(lh_addr_of(strip), x, y, width, height);
    lh_ui_canvas_fill_rect(canvas, lh_addr_of(strip), color);
}

/* The edge is a straight strip, not a stroke that follows the radius. The rows
   it replaces were filled rectangles one pixel tall laid across the box, and a
   rounded card's top pixel was already that strip sitting on the arc. */
static lh_void
lh_ui_entity_draw_border(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_style_t *style;
    const lh_ui_color_t *color;
    const lh_ui_insets_t *border;
    const lh_ui_point_t *origin;
    const lh_ui_size_t *size;
    lh_ui_rect_t rect;
    lh_ui_scalar_t x;
    lh_ui_scalar_t y;
    lh_ui_scalar_t width;
    lh_ui_scalar_t height;
    lh_ui_scalar_t left;
    lh_ui_scalar_t top;
    lh_ui_scalar_t right;
    lh_ui_scalar_t bottom;

    lh_return_if(lh_null_eq(canvas));
    style = lh_ui_entity_get_style_now(self);
    lh_return_if(lh_null_eq(style));
    border = lh_ui_style_get_border(style);
    left = border->left;
    top = border->top;
    right = border->right;
    bottom = border->bottom;
    lh_return_if(left <= lh_ui_scalar(0) && top <= lh_ui_scalar(0) && right <= lh_ui_scalar(0) &&
                 bottom <= lh_ui_scalar(0));
    color = lh_ui_paint_get_color(lh_ui_style_get_border_fill(style));
    lh_return_if(lh_null_eq(color));
    rect = lh_ui_entity_get_rect(self);
    origin = lh_ui_rect_get_origin_as_const(lh_addr_of(rect));
    size = lh_ui_rect_get_size_as_const(lh_addr_of(rect));
    x = lh_ui_point_get_x(origin);
    y = lh_ui_point_get_y(origin);
    width = lh_ui_size_get_width(size);
    height = lh_ui_size_get_height(size);
    if (top > height)
    {
        top = height;
    }
    if (bottom > height - top)
    {
        bottom = height - top;
    }
    if (left > width)
    {
        left = width;
    }
    if (right > width - left)
    {
        right = width - left;
    }
    lh_ui_entity_fill_edge(canvas, x, y, width, top, color);
    lh_ui_entity_fill_edge(canvas, x, y + height - bottom, width, bottom, color);
    lh_ui_entity_fill_edge(canvas, x, y + top, left, height - top - bottom, color);
    lh_ui_entity_fill_edge(canvas, x + width - right, y + top, right, height - top - bottom, color);
}

lh_void
lh_ui_entity_draw(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    lh_return_if(!lh_ui_entity_is_shown(self));
    lh_ui_entity_draw_self(self, canvas);
    lh_ui_entity_draw_children(self, canvas);
    /* After the children, and after their clip has been popped. A row's bottom
       edge is the line under its letters, and the letters are children: drawn
       with the fill it would sit underneath them. */
    lh_return_if(!lh_ui_entity_shows_on(self, canvas));
    lh_ui_entity_draw_border(self, canvas);
}

lh_void
lh_ui_entity_add_damage(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    lh_ui_rect_t rect;

    lh_return_if(lh_null_eq(self) || lh_null_eq(canvas));
    rect = lh_ui_entity_get_root_rect(self);
    lh_ui_canvas_add_damage(canvas, lh_addr_of(rect));
}
