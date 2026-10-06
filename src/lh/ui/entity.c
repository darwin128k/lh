/**
 * @file entity.c
 * @brief Implementation of `lh/ui/entity.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/list.h>
#include <lh/list/node.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/entity.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_entity_init(lh_ui_entity_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
    self->style = lh_null;
    self->class = lh_addr_of(lh_ui_entity_class);
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
lh_ui_entity_add_child(lh_ui_entity_t *self, lh_ui_entity_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    lh_assert_runtime_if(self == child, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_addr_of(child->link)),
                         lh_runtime_error_code_invalid_argument);
    lh_list_push_back(lh_addr_of(self->children), lh_addr_of(child->link));
}

lh_void
lh_ui_entity_remove_child(lh_ui_entity_t *self, lh_ui_entity_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    lh_assert_runtime_ifn(lh_list_contains(lh_addr_of(self->children), lh_addr_of(child->link)),
                          lh_runtime_error_code_invalid_argument);
    lh_list_node_unlink(lh_addr_of(child->link));
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

lh_void
lh_ui_entity_face_rect(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(event);
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
}

lh_void
lh_ui_entity_draw(const lh_ui_entity_t *self)
{
    lh_ui_entity_event_t event;
    const lh_ui_entity_class_t *class;
    lh_ui_entity_t *child;
    lh_assert_runtime_ref(self);
    class = lh_ui_entity_get_class(self);
    lh_assert_runtime_ref(class);
    lh_assert_runtime_ref(class->event);
    event.code = lh_ui_entity_event_draw;
    class->event(self, lh_addr_of(event));
    for (child = lh_ui_entity_get_first_child(self); !lh_null_eq(child);
         child = lh_ui_entity_get_next_child(self, child))
    {
        lh_ui_entity_draw(child);
    }
}
