/**
 * @file state.h
 * @brief Backend-private: an open Win32 directory listing, what a directory handle points to.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_FS_DIR_STATE_H
#define LH_SRC_OS_SYSTEM_WIN_FS_DIR_STATE_H

#include <lh/os/system/win/kernel32.h>
#include <lh/str.h>

/** An open directory listing: the `FindFirstFileW` handle and the entry in hand. */
struct lh_os_system_fs_dir_state
{
    lh_os_system_win_handle_t find;    /**< the listing, from FindFirstFileW */
    lh_os_system_win_find_data_t data; /**< the entry FindFirstFileW / FindNextFileW last wrote */
    lh_str_t name;                     /**< data.cFileName as UTF-8: what read hands out. */
    lh_bool_t ready; /**< data holds an entry not yet reported (FindFirstFile's). */
};

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The listing handle from FindFirstFileW (level 1).
 *
 * @param self The directory state; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_win_handle_t
lh_os_system_fs_dir_state_get_find(const struct lh_os_system_fs_dir_state *self);

/**
 * @brief Store the listing handle from FindFirstFileW (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The directory state; checked by ::lh_assert_runtime_ref.
 * @param find The new value.
 */
lh_void
lh_os_system_fs_dir_state_set_find(struct lh_os_system_fs_dir_state *self,
                                   lh_os_system_win_handle_t find);

/**
 * @brief The entry FindFirstFileW or FindNextFileW last wrote (level 1).
 *
 * @param self The directory state; checked by ::lh_assert_runtime_ref.
 */
const lh_os_system_win_find_data_t *
lh_os_system_fs_dir_state_get_data(const struct lh_os_system_fs_dir_state *self);

/**
 * @brief The entry FindFirstFileW or FindNextFileW last wrote, to change in place (level 1).
 *
 * @param self The directory state; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_system_win_find_data_t *
lh_os_system_fs_dir_state_get_data_mut(struct lh_os_system_fs_dir_state *self);

/**
 * @brief The name of the entry in hand, as UTF-8 (level 1).
 *
 * @param self The directory state; checked by ::lh_assert_runtime_ref.
 */
const lh_str_t *
lh_os_system_fs_dir_state_get_name(const struct lh_os_system_fs_dir_state *self);

/**
 * @brief The name of the entry in hand, as UTF-8, to change in place (level 1).
 *
 * @param self The directory state; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_str_t *
lh_os_system_fs_dir_state_get_name_mut(struct lh_os_system_fs_dir_state *self);

/**
 * @brief Whether the entry in hand is not reported yet (level 1).
 *
 * @param self The directory state; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_system_fs_dir_state_is_ready(const struct lh_os_system_fs_dir_state *self);

/**
 * @brief Store whether the entry in hand is not reported yet (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The directory state; checked by ::lh_assert_runtime_ref.
 * @param ready The new value.
 */
lh_void
lh_os_system_fs_dir_state_set_ready(struct lh_os_system_fs_dir_state *self, lh_bool_t ready);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_WIN_FS_DIR_STATE_H */
