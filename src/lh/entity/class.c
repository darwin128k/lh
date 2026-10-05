/**
 * @file class.c
 * @brief Implementation of `lh/entity/class.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/entity/class.h>
#include <lh/entity.h>
#include <lh/null.h>
#include <lh/util/return.h>

const lh_entity_class_t lh_entity_class = {lh_entity_face_rect, lh_null};

lh_void
lh_entity_class_event_base(const lh_entity_class_t *class_p, const struct lh_entity *self,
                           const lh_entity_event_t *event)
{
    const lh_entity_class_t *base;
    lh_assert_runtime_ref(class_p);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(event);
    base = class_p->base;
    lh_return_if(base == lh_null);
    lh_assert_runtime_ref(base->event);
    base->event(self, event);
}
