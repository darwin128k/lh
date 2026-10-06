/**
 * @file class.c
 * @brief Implementation of `lh/ui/entity/class.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/entity/class.h>
#include <lh/ui/entity.h>
#include <lh/null.h>
#include <lh/util/return.h>

const lh_ui_entity_class_t lh_ui_entity_class = {lh_ui_entity_face_rect, lh_null};

lh_void
lh_ui_entity_class_event_base(const lh_ui_entity_class_t *class, const struct lh_ui_entity *self,
                              const lh_ui_entity_event_t *event)
{
    const lh_ui_entity_class_t *base;
    lh_assert_runtime_ref(class);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(event);
    base = class->base;
    lh_return_if(base == lh_null);
    lh_assert_runtime_ref(base->event);
    base->event(self, event);
}
