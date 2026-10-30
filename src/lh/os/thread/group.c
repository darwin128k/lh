/**
 * @file group.c
 * @brief ::lh_os_thread_group_t: a list of owned threads, woken all, then joined all.
 */

#include <lh/os/thread/group.h>
#include <lh/assert/runtime.h>
#include <lh/util/return.h>
#include <lh/runtime/terminate.h>
#include <lh/os/thread/group/member.h>

const lh_list_t *
lh_os_thread_group_get_members(const lh_os_thread_group_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->members);
}

lh_list_t *
lh_os_thread_group_get_members_mut(lh_os_thread_group_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->members);
}

lh_usize_t
lh_os_thread_group_get_count(const lh_os_thread_group_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_os_thread_group_set_count(lh_os_thread_group_t *self, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_bool_t
lh_os_thread_group_is_closing(const lh_os_thread_group_t *self)
{
    lh_assert_runtime_ref(self);
    return self->closing;
}

lh_void
lh_os_thread_group_set_closing(lh_os_thread_group_t *self, lh_bool_t closing)
{
    lh_assert_runtime_ref(self);
    self->closing = closing;
}

lh_os_thread_group_member_t *
lh_os_thread_group_get_member(lh_list_node_t *node)
{
    return lh_list_entry(lh_os_thread_group_member_t, node, node);
}

lh_void
lh_os_thread_group_init(lh_os_thread_group_t *self)
{
    lh_list_init(lh_os_thread_group_get_members_mut(self));
    lh_os_thread_group_set_count(self, 0U);
    lh_os_thread_group_set_closing(self, lh_bool_false);
}

lh_os_thread_group_member_t *
lh_os_thread_group_try_start(lh_os_thread_group_t *self, lh_os_thread_fn run, lh_os_thread_fn wake,
                             lh_ptr user, lh_s32_t cpu, lh_u32_t flags)
{
    lh_os_thread_group_member_t *member;

    lh_return_if(lh_os_thread_group_is_closing(self) || lh_null_eq(run) || lh_null_eq(wake),
                 lh_null);
    member = lh_os_thread_group_member_try_create(wake, user);
    lh_return_if(lh_null_eq(member), lh_null);
    if (!lh_os_thread_start(lh_os_thread_group_member_get_thread_mut(member), run, user, cpu,
                            flags))
    {
        lh_os_thread_group_member_destroy(member);
        return lh_null;
    }
    lh_list_push_back(lh_os_thread_group_get_members_mut(self),
                      lh_os_thread_group_member_get_node_mut(member));
    lh_os_thread_group_set_count(self, lh_os_thread_group_get_count(self) + 1U);
    return member;
}

lh_bool_t
lh_os_thread_group_stop(lh_os_thread_group_t *self, lh_os_thread_group_member_t *member)
{
    lh_return_if(lh_null_eq(member) ||
                     !lh_list_contains(lh_os_thread_group_get_members(self),
                                       lh_os_thread_group_member_get_node(member)),
                 lh_bool_false);
    lh_os_thread_group_member_request_stop(member);
    lh_return_ifn(lh_os_thread_join(lh_os_thread_group_member_get_thread_mut(member)),
                  lh_bool_false);
    lh_list_node_unlink(lh_os_thread_group_member_get_node_mut(member));
    lh_os_thread_group_set_count(self, lh_os_thread_group_get_count(self) - 1U);
    lh_os_thread_group_member_destroy(member);
    return lh_bool_true;
}

lh_void
lh_os_thread_group_stop_all(lh_os_thread_group_t *self)
{
    const lh_list_t *list = lh_os_thread_group_get_members(self);

    lh_os_thread_group_set_closing(self, lh_bool_true);
    /* Wake all first: threads may wait for each other to end. */
    for (lh_list_node_t *node = lh_list_get_first(list); lh_null_ne(node);
         node = lh_list_get_next(list, node))
    {
        lh_os_thread_group_member_request_stop(lh_os_thread_group_get_member(node));
    }
    for (lh_list_node_t *node = lh_list_get_first(list); lh_null_ne(node);
         node = lh_list_get_next(list, node))
    {
        lh_runtime_terminate_unless(lh_os_thread_join(
            lh_os_thread_group_member_get_thread_mut(lh_os_thread_group_get_member(node))));
    }
}

lh_void
lh_os_thread_group_deinit(lh_os_thread_group_t *self)
{
    lh_list_t *list = lh_os_thread_group_get_members_mut(self);

    lh_os_thread_group_stop_all(self);
    while (!lh_list_is_empty(list))
    {
        lh_os_thread_group_member_destroy(lh_os_thread_group_get_member(lh_list_pop_front(list)));
    }
    lh_os_thread_group_set_count(self, 0U);
}
