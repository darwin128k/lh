#include <lh/entity/container.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

const lh_entity_class_t lh_entity_container_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_container_t),
                                lh_null, lh_null, lh_null);

lh_int_t
lh_entity_container_get_count(const lh_entity_container_t *self)
{
    lh_int_t count = 0;
    lh_assert_runtime_ref(self);
    lh_entity_foreach_child(child, lh_ptr_rcast(const lh_entity_t, self))
    {
        (void)child;
        count += 1;
    }
    return count;
}
