#include <lh/entity/group.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_group_construct(lh_entity_t *self)
{
    lh_ptr_rcast(lh_entity_group_t, self)->mode = LH_ENTITY_GROUP_ONE;
}

const lh_entity_class_t lh_entity_group_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_group_t),
                                lh_entity_group_construct, lh_null, lh_null);

lh_int_t
lh_entity_group_get_mode(const lh_entity_group_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode;
}

lh_void
lh_entity_group_set_mode(lh_entity_group_t *self, lh_int_t mode)
{
    lh_assert_runtime_ref(self);
    self->mode = mode == LH_ENTITY_GROUP_MANY ? LH_ENTITY_GROUP_MANY : LH_ENTITY_GROUP_ONE;
}
