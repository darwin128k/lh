/**
 * @file group.h
 * @brief ::lh_os_thread_group_t — threads that belong to one owner and stop together.
 *
 * Each thread runs its own code until its `wake` callback makes it return; nothing
 * is cancelled by force, so `run` must wait in bounded steps. Stopping one thread
 * wakes it and joins it. Stopping the group wakes every thread first and only then
 * joins them, so threads that wait for each other still end. After a group stop
 * nothing more starts. A module host gives each module a group; a program gives
 * each subsystem one.
 *
 * All calls come from the owner's thread. `wake` runs there too: it must not block
 * and may run once per thread.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_THREAD_GROUP_H
#define LH_OS_THREAD_GROUP_H

#include <lh/list.h>
#include <lh/os/thread.h>

/** @brief One thread of a group; opaque. */
typedef struct lh_os_thread_group_member lh_os_thread_group_member_t;

/** @brief Threads that stop together. */
struct lh_os_thread_group
{
    lh_list_t members; /**< the running threads */
    lh_usize_t count;  /**< how many there are */
    lh_bool_t closing; /**< a group stop was asked: nothing more starts */
};
/** @copybrief lh_os_thread_group */
typedef struct lh_os_thread_group lh_os_thread_group_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The running threads (level 1).
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 */
const lh_list_t *
lh_os_thread_group_get_members(const lh_os_thread_group_t *self);

/**
 * @brief The running threads, to change in place (level 1).
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_list_t *
lh_os_thread_group_get_members_mut(lh_os_thread_group_t *self);

/**
 * @brief How many threads run (level 1).
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_os_thread_group_get_count(const lh_os_thread_group_t *self);

/**
 * @brief Store how many threads run (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 * @param count The new value.
 */
lh_void
lh_os_thread_group_set_count(lh_os_thread_group_t *self, lh_usize_t count);

/**
 * @brief Whether a group stop was asked (level 1).
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_thread_group_is_closing(const lh_os_thread_group_t *self);

/**
 * @brief Store whether a group stop was asked (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 * @param closing The new value.
 */
lh_void
lh_os_thread_group_set_closing(lh_os_thread_group_t *self, lh_bool_t closing);

/**
 * @brief Start an empty, open group (level 2).
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_os_thread_group_init(lh_os_thread_group_t *self);

/**
 * @brief Start a thread that runs @p run, stopped by @p wake (level 3).
 *
 * @param self  The group, not stopped.
 * @param run   The thread's code; returns when @p wake asks it to.
 * @param wake  Makes @p run return; runs on the owner's thread, must not block.
 * @param user  The value @p run and @p wake are given.
 * @param cpu   Logical CPU index, or -1 to inherit affinity.
 * @param flags Zero or ::lh_os_thread_block_sigpipe.
 * @return The thread, or ::lh_null when the group is stopped, an argument is wrong,
 *         memory is short or the OS refused (its error is set); nothing ran then.
 */
lh_os_thread_group_member_t *
lh_os_thread_group_try_start(lh_os_thread_group_t *self, lh_os_thread_fn run, lh_os_thread_fn wake,
                             lh_ptr user, lh_s32_t cpu, lh_u32_t flags);

/**
 * @brief Wake @p member, join it and free it (level 3).
 *
 * @param self   The group.
 * @param member A thread of @p self.
 * @return ::lh_bool_false when @p member is not in @p self or the join failed; the
 *         thread then stays in the group.
 */
lh_bool_t
lh_os_thread_group_stop(lh_os_thread_group_t *self, lh_os_thread_group_member_t *member);

/**
 * @brief Close the group, wake every thread, then join every one (level 3).
 *
 * The threads stay in the group, joined, until ::lh_os_thread_group_deinit. A join
 * that fails terminates the program: a live thread must not outlive the code it runs.
 *
 * @param self The group.
 */
lh_void
lh_os_thread_group_stop_all(lh_os_thread_group_t *self);

/**
 * @brief Stop every thread and free them all (level 4).
 *
 * @param self The group.
 */
lh_void
lh_os_thread_group_deinit(lh_os_thread_group_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_THREAD_GROUP_H */
