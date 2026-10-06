/**
 * @file entity.c
 * @brief Implementation of `lh/ui/entity.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/entity.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_entity_init(lh_ui_entity_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
    self->style = lh_null;
    self->class_p = lh_addr_of(lh_ui_entity_class);
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
    return self->class_p;
}

lh_void
lh_ui_entity_set_class(lh_ui_entity_t *self, const lh_ui_entity_class_t *class_p)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(class_p);
    lh_assert_runtime_ref(class_p->event);
    self->class_p = class_p;
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
    const lh_ui_entity_class_t *class_p;
    lh_assert_runtime_ref(self);
    class_p = lh_ui_entity_get_class(self);
    lh_assert_runtime_ref(class_p);
    lh_assert_runtime_ref(class_p->event);
    event.code = lh_ui_entity_event_draw;
    class_p->event(self, lh_addr_of(event));
}
