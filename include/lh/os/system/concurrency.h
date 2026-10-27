/** @file concurrency.h @brief Native thread/synchronization handles; CMake selects the backend. */
#ifndef LH_OS_SYSTEM_CONCURRENCY_H
#define LH_OS_SYSTEM_CONCURRENCY_H
#include <lh/config.h>
#include <lh/bool.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/compiler/extern/c.h>
#if !LH_LIBRARY_OPTION_OS
#    error "OS concurrency requires LH_LIBRARY_OPTION_OS"
#endif
typedef lh_ptr lh_os_system_thread_handle_t;
typedef lh_ptr lh_os_system_mutex_handle_t;
typedef lh_ptr lh_os_system_condition_handle_t;
typedef lh_void (*lh_os_thread_fn)(lh_ptr user);
#define lh_os_thread_block_sigpipe 1U
LH_COMPILER_EXTERN_C_BEGIN
lh_os_system_thread_handle_t
lh_os_system_thread_start(lh_os_thread_fn run, lh_ptr user, lh_s32_t cpu, lh_u32_t flags);
lh_bool_t
lh_os_system_thread_join(lh_os_system_thread_handle_t handle);
lh_os_system_mutex_handle_t lh_os_system_mutex_open(lh_void);
lh_bool_t
lh_os_system_mutex_close(lh_os_system_mutex_handle_t handle);
lh_bool_t
lh_os_system_mutex_lock(lh_os_system_mutex_handle_t handle);
lh_bool_t
lh_os_system_mutex_unlock(lh_os_system_mutex_handle_t handle);
lh_os_system_condition_handle_t lh_os_system_condition_open(lh_void);
lh_bool_t
lh_os_system_condition_close(lh_os_system_condition_handle_t handle);
lh_bool_t
lh_os_system_condition_wait(lh_os_system_condition_handle_t handle,
                            lh_os_system_mutex_handle_t mutex);
lh_bool_t
lh_os_system_condition_signal(lh_os_system_condition_handle_t handle);
lh_bool_t
lh_os_system_condition_broadcast(lh_os_system_condition_handle_t handle);
LH_COMPILER_EXTERN_C_END
#endif
