/**
 * @file member.c
 * @brief Accessors of a thread group's member.
 */

#include <lh/os/thread/group/member.h>
#include <lh/assert/runtime.h>
#include <lh/util/return.h>
#include <lh/runtime/allocator.h>

const lh_memory_allocator_t *
lh_os_thread_group_member_get_allocator(const lh_os_thread_group_member_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_memory_allocator_t *
lh_os_thread_group_member_get_allocator_mut(lh_os_thread_group_member_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

const lh_list_node_t *
lh_os_thread_group_member_get_node(const lh_os_thread_group_member_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_list_node_t *
lh_os_thread_group_member_get_node_mut(lh_os_thread_group_member_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_void
lh_os_thread_group_member_set_node(lh_os_thread_group_member_t *self, lh_list_node_t node)
{
    lh_assert_runtime_ref(self);
    self->node = node;
}

lh_os_thread_fn
lh_os_thread_group_member_get_wake_fn(const lh_os_thread_group_member_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wake;
}

lh_void
lh_os_thread_group_member_set_wake_fn(lh_os_thread_group_member_t *self, lh_os_thread_fn wake)
{
    lh_assert_runtime_ref(self);
    self->wake = wake;
}

lh_ptr
lh_os_thread_group_member_get_user(const lh_os_thread_group_member_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_os_thread_group_member_set_user(lh_os_thread_group_member_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

lh_bool_t
lh_os_thread_group_member_is_requested(const lh_os_thread_group_member_t *self)
{
    lh_assert_runtime_ref(self);
    return self->requested;
}

lh_void
lh_os_thread_group_member_set_requested(lh_os_thread_group_member_t *self, lh_bool_t requested)
{
    lh_assert_runtime_ref(self);
    self->requested = requested;
}

const lh_os_thread_t *
lh_os_thread_group_member_get_thread(const lh_os_thread_group_member_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->thread);
}

lh_os_thread_t *
lh_os_thread_group_member_get_thread_mut(lh_os_thread_group_member_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->thread);
}

lh_os_thread_group_member_t *
lh_os_thread_group_member_try_create(lh_os_thread_fn wake, lh_ptr user)
{
    lh_memory_allocator_t allocator = lh_ptr_deref(lh_runtime_allocator());
    lh_os_thread_group_member_t *self = lh_ptr_rcast(
        lh_os_thread_group_member_t,
        lh_memory_allocator_try_alloc_zero(lh_addr_of(allocator), 1U, sizeof(lh_ptr_deref(self))));

    lh_return_if(lh_null_eq(self), lh_null);
    lh_memory_allocator_assign(lh_os_thread_group_member_get_allocator_mut(self),
                               lh_addr_of(allocator));
    lh_list_node_init(lh_os_thread_group_member_get_node_mut(self));
    lh_os_thread_group_member_set_wake_fn(self, wake);
    lh_os_thread_group_member_set_user(self, user);
    lh_os_thread_group_member_set_requested(self, lh_bool_false);
    lh_os_thread_init(lh_os_thread_group_member_get_thread_mut(self));
    return self;
}

lh_void
lh_os_thread_group_member_request_stop(lh_os_thread_group_member_t *self)
{
    lh_return_if(lh_os_thread_group_member_is_requested(self));
    lh_os_thread_group_member_set_requested(self, lh_bool_true);
    lh_os_thread_group_member_get_wake_fn(self)(lh_os_thread_group_member_get_user(self));
}

lh_void
lh_os_thread_group_member_destroy(lh_os_thread_group_member_t *self)
{
    lh_memory_allocator_dealloc(lh_os_thread_group_member_get_allocator_mut(self), self);
}
