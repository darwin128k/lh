#include <lh/entity/pages.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/entity/screen.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

const lh_entity_class_t lh_entity_pages_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_pages_t), lh_null, lh_null,
                                lh_null);

lh_int_t
lh_entity_pages_get_index(const lh_entity_pages_t *self)
{
    lh_assert_runtime_ref(self);
    return self->index;
}

lh_void
lh_entity_pages_set_index(lh_entity_pages_t *self, lh_int_t index)
{
    lh_int_t i = 0;
    lh_assert_runtime_ref(self);
    self->index = index < 0 ? 0 : index;
    lh_entity_foreach_child(child, lh_ptr_rcast(lh_entity_t, self))
    {
        if (i == self->index)
        {
            lh_entity_clear_flags(child, lh_entity_flags_hidden);
        }
        else
        {
            lh_entity_add_flags(child, lh_entity_flags_hidden);
        }
        i += 1;
    }
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}
