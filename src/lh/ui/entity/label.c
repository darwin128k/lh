/**
 * @file label.c
 * @brief Implementation of `lh/ui/entity/label.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/entity/label.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

static lh_void
lh_ui_entity_label_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    const lh_ui_entity_label_t *label;
    lh_assert_runtime_ref(self);
    label = lh_ptr_rcast(const lh_ui_entity_label_t, self);
    lh_assert_runtime_ref(label);
    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_entity_label_class), self, event);
}

const lh_ui_entity_class_t lh_ui_entity_label_class = {lh_ui_entity_label_event, lh_addr_of(lh_ui_entity_class)};

lh_void
lh_ui_entity_label_init(lh_ui_entity_label_t *self, lh_ui_rect_t rect, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    lh_ui_entity_init(lh_addr_of(self->entity), rect);
    self->text = text;
    lh_ui_entity_set_class(lh_addr_of(self->entity), lh_addr_of(lh_ui_entity_label_class));
}

lh_ui_rect_t
lh_ui_entity_label_get_rect(const lh_ui_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_entity_get_rect(lh_addr_of(self->entity));
}

lh_void
lh_ui_entity_label_set_rect(lh_ui_entity_label_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    lh_ui_entity_set_rect(lh_addr_of(self->entity), rect);
}

const lh_char_t *
lh_ui_entity_label_get_text(const lh_ui_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_ui_entity_label_set_text(lh_ui_entity_label_t *self, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

lh_void
lh_ui_entity_label_draw(const lh_ui_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_entity_draw(lh_addr_of(self->entity));
}
