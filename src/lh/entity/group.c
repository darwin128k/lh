#include <lh/entity/group.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/entity/option.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_group_construct(lh_entity_t *self)
{
    lh_ptr_rcast(lh_entity_group_t, self)->mode = LH_ENTITY_GROUP_ONE;
}

const lh_entity_class_t lh_entity_group_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_group_t),
                                lh_entity_group_construct, lh_null, lh_null);

lh_int_t
lh_entity_group_get_mode(const lh_entity_group_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode;
}

lh_int_t
lh_entity_group_normalize_mode(lh_int_t mode)
{
    return mode == LH_ENTITY_GROUP_MANY ? LH_ENTITY_GROUP_MANY : LH_ENTITY_GROUP_ONE;
}

lh_void
lh_entity_group_set_mode(lh_entity_group_t *self, lh_int_t mode)
{
    lh_assert_runtime_ref(self);
    self->mode = lh_entity_group_normalize_mode(mode);
}

lh_void
lh_entity_group_select(lh_entity_group_t *self, lh_entity_t *member)
{
    lh_entity_t *const root = lh_ptr_rcast(lh_entity_t, self);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(member);
    if (self->mode != LH_ENTITY_GROUP_ONE)
    {
        return;
    }
    /* One walk, one cast. The child is an option or it is not, and that is a
       question the base class answers, so this code names none of the kinds. */
    lh_entity_foreach_child(child, root)
    {
        lh_entity_option_t *const other =
            lh_entity_cast(child, lh_addr_of(lh_entity_option_class));
        if (lh_ptr_is_set(other) && lh_ptr_rcast(lh_entity_t, other) != member)
        {
            lh_entity_option_set_on_raw(other, lh_bool_false);
        }
    }
}
