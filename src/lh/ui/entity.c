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
#include <lh/ui/entity.h>
#include <lh/ui/point.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

static lh_bool_t
lh_ui_entity_is_ancestor(const lh_ui_entity_t *ancestor, const lh_ui_entity_t *self)
{
    const lh_ui_entity_t *walk;

    for (walk = self; lh_null_ne(walk); walk = walk->parent)
    {
        if (walk == ancestor)
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_void
lh_ui_entity_init(lh_ui_entity_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
    self->style = lh_null;
    self->class = lh_addr_of(lh_ui_entity_class);
    self->hidden = lh_bool_false;
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
    style = self->style;
    if (lh_null_eq(style))
    {
        return lh_null;
    }
    return lh_ui_style_get_fill_color(style);
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
    lh_ui_entity_t *child;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(visit);
    if (!visit(self, context))
    {
        return lh_bool_false;
    }
    for (child = lh_ui_entity_get_first_child(self); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(self, child))
    {
        if (!lh_ui_entity_walk(child, visit, context))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_ui_entity_t *
lh_ui_entity_find_at(lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_ui_entity_t *child;
    lh_ui_entity_t *hit;
    lh_ui_rect_t rect;

    lh_assert_runtime_ref(self);
    if (self->hidden)
    {
        return lh_null;
    }
    rect = lh_ui_entity_get_rect(self);
    if (!lh_ui_rect_contains_point(lh_addr_of(rect), point))
    {
        return lh_null;
    }
    for (child = lh_ui_entity_get_last_child(self); lh_null_ne(child);
         child = lh_ui_entity_get_prev_child(self, child))
    {
        hit = lh_ui_entity_find_at(child, point);
        if (lh_null_ne(hit))
        {
            return hit;
        }
    }
    return self;
}

lh_ui_entity_t *
lh_ui_entity_click(lh_ui_entity_t *self, lh_ui_point_t point)
{
    lh_ui_entity_t *hit;
    const lh_ui_entity_class_t *klass;
    lh_ui_entity_event_t event;

    lh_assert_runtime_ref(self);
    hit = lh_ui_entity_find_at(self, point);
    if (lh_null_eq(hit))
    {
        return lh_null;
    }
    klass = lh_ui_entity_get_class(hit);
    lh_assert_runtime_ref(klass);
    lh_assert_runtime_ref(klass->event);
    lh_ui_entity_event_init(lh_addr_of(event), lh_ui_entity_event_click, lh_addr_of(point));
    klass->event(hit, lh_addr_of(event));
    return hit;
}

lh_void
lh_ui_entity_draw(const lh_ui_entity_t *self, lh_ui_canvas_t *canvas)
{
    lh_ui_entity_event_t event;
    const lh_ui_entity_class_t *klass;
    lh_ui_entity_t *child;

    lh_assert_runtime_ref(self);
    if (self->hidden)
    {
        return;
    }
    klass = lh_ui_entity_get_class(self);
    lh_assert_runtime_ref(klass);
    lh_assert_runtime_ref(klass->event);
    lh_ui_entity_event_init(lh_addr_of(event), lh_ui_entity_event_draw, canvas);
    klass->event(self, lh_addr_of(event));
    for (child = lh_ui_entity_get_first_child(self); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(self, child))
    {
        lh_ui_entity_draw(child, canvas);
    }
}
