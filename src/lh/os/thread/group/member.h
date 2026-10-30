/**
 * @file member.h
 * @brief Private to `group.c`: one thread of an ::lh_os_thread_group_t.
 */

#ifndef LH_SRC_OS_THREAD_GROUP_MEMBER_H
#define LH_SRC_OS_THREAD_GROUP_MEMBER_H

#include <lh/memory/allocator.h>
#include <lh/os/thread/group.h>

/** @brief One thread of a group. */
struct lh_os_thread_group_member
{
    lh_memory_allocator_t allocator; /**< what it was allocated with */
    lh_list_node_t node;             /**< the link in the group's list */
    lh_os_thread_fn wake;            /**< makes the thread's run return */
    lh_ptr user;                     /**< the value run and wake are given */
    lh_bool_t requested;             /**< a stop was asked: wake ran */
    lh_os_thread_t thread;           /**< the thread itself */
};

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The allocator it was made with (level 1).
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 */
const lh_memory_allocator_t *
lh_os_thread_group_member_get_allocator(const lh_os_thread_group_member_t *self);

/**
 * @brief The allocator it was made with, to change in place (level 1).
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_memory_allocator_t *
lh_os_thread_group_member_get_allocator_mut(lh_os_thread_group_member_t *self);

/**
 * @brief The link in the group's list (level 1).
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 */
const lh_list_node_t *
lh_os_thread_group_member_get_node(const lh_os_thread_group_member_t *self);

/**
 * @brief The link in the group's list, to change in place (level 1).
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_list_node_t *
lh_os_thread_group_member_get_node_mut(lh_os_thread_group_member_t *self);

/**
 * @brief Store the link in the group's list (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 * @param node The new value.
 */
lh_void
lh_os_thread_group_member_set_node(lh_os_thread_group_member_t *self, lh_list_node_t node);

/**
 * @brief The callback that makes the thread's run return (level 1).
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 */
lh_os_thread_fn
lh_os_thread_group_member_get_wake_fn(const lh_os_thread_group_member_t *self);

/**
 * @brief Store the callback that makes the thread's run return (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 * @param wake The new value.
 */
lh_void
lh_os_thread_group_member_set_wake_fn(lh_os_thread_group_member_t *self, lh_os_thread_fn wake);

/**
 * @brief The value run and wake are given (level 1).
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_thread_group_member_get_user(const lh_os_thread_group_member_t *self);

/**
 * @brief Store the value run and wake are given (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 * @param user The new value.
 */
lh_void
lh_os_thread_group_member_set_user(lh_os_thread_group_member_t *self, lh_ptr user);

/**
 * @brief Whether a stop was asked and wake ran (level 1).
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_thread_group_member_is_requested(const lh_os_thread_group_member_t *self);

/**
 * @brief Store whether a stop was asked and wake ran (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 * @param requested The new value.
 */
lh_void
lh_os_thread_group_member_set_requested(lh_os_thread_group_member_t *self, lh_bool_t requested);

/**
 * @brief The thread itself (level 1).
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 */
const lh_os_thread_t *
lh_os_thread_group_member_get_thread(const lh_os_thread_group_member_t *self);

/**
 * @brief The thread itself, to change in place (level 1).
 *
 * @param self The member; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_thread_t *
lh_os_thread_group_member_get_thread_mut(lh_os_thread_group_member_t *self);

/**
 * @brief Allocate a member for @p wake and @p user, its thread not started (level 2).
 *
 * @param wake What makes the thread's run return.
 * @param user The value run and wake are given.
 * @return The member, or ::lh_null when memory is short.
 */
lh_os_thread_group_member_t *
lh_os_thread_group_member_try_create(lh_os_thread_fn wake, lh_ptr user);

/**
 * @brief Run its wake, once (level 2).
 *
 * @param self The member.
 */
lh_void
lh_os_thread_group_member_request_stop(lh_os_thread_group_member_t *self);

/**
 * @brief Free a member whose thread is joined or never started (level 2).
 *
 * @param self The member, unlinked.
 */
lh_void
lh_os_thread_group_member_destroy(lh_os_thread_group_member_t *self);

/**
 * @brief The member of @p self (level 1).
 */
lh_os_thread_group_member_t *
lh_os_thread_group_get_member(lh_list_node_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_THREAD_GROUP_MEMBER_H */
