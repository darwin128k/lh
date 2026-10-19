#include <lh/pair.h>
#include <lh/assert.h>
#include <lh/cast/static.h>

const lh_value_t *
lh_pair_get_first(const lh_pair_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->first);
}

const lh_value_t *
lh_pair_get_second(const lh_pair_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->second);
}

lh_void
lh_pair_set_first(lh_pair_t *self, lh_value_t value)
{
    lh_assert_runtime_ref(self);
    lh_value_assign(lh_addr_of(self->first), lh_addr_of(value));
}

lh_void
lh_pair_set_second(lh_pair_t *self, lh_value_t value)
{
    lh_assert_runtime_ref(self);
    lh_value_assign(lh_addr_of(self->second), lh_addr_of(value));
}

lh_void
lh_pair_init(lh_pair_t *self)
{
    lh_pair_set_first(self, lh_value_make_none());
    lh_pair_set_second(self, lh_value_make_none());
}

lh_void
lh_pair_assign(lh_pair_t *self, const lh_pair_t *other)
{
    lh_pair_set_first(self, lh_ptr_deref(lh_pair_get_first(other)));
    lh_pair_set_second(self, lh_ptr_deref(lh_pair_get_second(other)));
}

lh_pair_t
lh_pair_make_none(lh_void)
{
    lh_pair_t self;

    lh_pair_init(lh_addr_of(self));
    return self;
}

lh_pair_t
lh_pair_make(lh_value_t first, lh_value_t second)
{
    lh_pair_t self;

    lh_pair_set_first(lh_addr_of(self), first);
    lh_pair_set_second(lh_addr_of(self), second);
    return self;
}

lh_bool_t
lh_pair_equals(const lh_pair_t *self, const lh_pair_t *other)
{
    return lh_cast_static(
        lh_bool_t, lh_value_equals(lh_pair_get_first(self), lh_pair_get_first(other)) &&
                       lh_value_equals(lh_pair_get_second(self), lh_pair_get_second(other)));
}
