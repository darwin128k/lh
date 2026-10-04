#include <lh/entity/view.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

const lh_entity_class_t lh_entity_view_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_view_t), lh_null, lh_null,
                                lh_null);

lh_void
lh_entity_view_set_content(lh_entity_view_t *self, lh_entity_t *content)
{
    lh_assert_runtime_ref(self);
    self->content = content;
}

lh_void
lh_entity_view_set_offset(lh_entity_view_t *self, lh_int_t x, lh_int_t y)
{
    lh_assert_runtime_ref(self);
    self->x = x < 0 ? 0 : x;
    self->y = y < 0 ? 0 : y;
    if (lh_ptr_is_set(self->content))
    {
        lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, self->content),
                                  lh_math_vec2_make((lh_float_t)(-self->x), (lh_float_t)(-self->y)));
    }
}
