/**
 * @file label.c
 * @brief Implementation of `lh/entity/label.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/entity/label.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

static lh_void
lh_entity_label_event(const struct lh_entity *self, const lh_entity_event_t *event)
{
    const lh_entity_label_t *label;
    lh_assert_runtime_ref(self);
    label = lh_ptr_rcast(const lh_entity_label_t, self);
    lh_assert_runtime_ref(label);
    lh_entity_class_event_base(lh_addr_of(lh_entity_label_class), self, event);
}

const lh_entity_class_t lh_entity_label_class = {lh_entity_label_event, lh_addr_of(lh_entity_class)};

lh_void
lh_entity_label_init(lh_entity_label_t *self, lh_math_rect_t rect, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    lh_entity_init(lh_addr_of(self->entity), rect);
    self->text = text;
    lh_entity_set_class(lh_addr_of(self->entity), lh_addr_of(lh_entity_label_class));
}

lh_math_rect_t
lh_entity_label_get_rect(const lh_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_entity_get_rect(lh_addr_of(self->entity));
}

lh_void
lh_entity_label_set_rect(lh_entity_label_t *self, lh_math_rect_t rect)
{
    lh_assert_runtime_ref(self);
    lh_entity_set_rect(lh_addr_of(self->entity), rect);
}

const lh_char_t *
lh_entity_label_get_text(const lh_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_entity_label_set_text(lh_entity_label_t *self, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

lh_void
lh_entity_label_draw(const lh_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    lh_entity_draw(lh_addr_of(self->entity));
}
