/**
 * @file channel.h
 * @brief Private to `daily.c`: one channel of daily files — its directory, its retention
 *        and the file of the day it is writing.
 *
 * Lives on the queue's worker thread only.
 */

#ifndef LH_SRC_OS_LOGGER_DAILY_CHANNEL_H
#define LH_SRC_OS_LOGGER_DAILY_CHANNEL_H

#include <lh/list/node.h>
#include <lh/os/fs/file.h>

/** @brief One channel of daily files. */
struct lh_os_logger_daily_channel
{
    lh_list_node_t node;  /**< the link in the sink's list */
    lh_u32_t id;          /**< the channel id records carry */
    lh_fs_path_t dir;     /**< the directory its files go to */
    lh_s32_t keep_days;   /**< days of files to keep; 0 keeps all */
    lh_bool_t raw;        /**< lines are written as text only */
    lh_os_fs_file_t file; /**< the file of the day, when open */
    lh_s64_t day;         /**< the local day of that file, in days since the epoch */
    lh_s64_t pruned_day;  /**< the local day retention last ran for */
};
/** @copybrief lh_os_logger_daily_channel */
typedef struct lh_os_logger_daily_channel lh_os_logger_daily_channel_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The link in the sink's list (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 */
const lh_list_node_t *
lh_os_logger_daily_channel_get_node(const lh_os_logger_daily_channel_t *self);

/**
 * @brief The link in the sink's list, to change in place (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_list_node_t *
lh_os_logger_daily_channel_get_node_mut(lh_os_logger_daily_channel_t *self);

/**
 * @brief Store the link in the sink's list (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 * @param node The new value.
 */
lh_void
lh_os_logger_daily_channel_set_node(lh_os_logger_daily_channel_t *self, lh_list_node_t node);

/**
 * @brief The channel id records carry (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_os_logger_daily_channel_get_id(const lh_os_logger_daily_channel_t *self);

/**
 * @brief Store the channel id records carry (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 * @param id The new value.
 */
lh_void
lh_os_logger_daily_channel_set_id(lh_os_logger_daily_channel_t *self, lh_u32_t id);

/**
 * @brief The directory its files go to (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 */
const lh_fs_path_t *
lh_os_logger_daily_channel_get_dir(const lh_os_logger_daily_channel_t *self);

/**
 * @brief The directory its files go to, to change in place (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_fs_path_t *
lh_os_logger_daily_channel_get_dir_mut(lh_os_logger_daily_channel_t *self);

/**
 * @brief The days of files it keeps, 0 for all (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_os_logger_daily_channel_get_keep_days(const lh_os_logger_daily_channel_t *self);

/**
 * @brief Store the days of files it keeps, 0 for all (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 * @param keep_days The new value.
 */
lh_void
lh_os_logger_daily_channel_set_keep_days(lh_os_logger_daily_channel_t *self, lh_s32_t keep_days);

/**
 * @brief Whether its lines are written as text only (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_logger_daily_channel_is_raw(const lh_os_logger_daily_channel_t *self);

/**
 * @brief Store whether its lines are written as text only (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 * @param raw The new value.
 */
lh_void
lh_os_logger_daily_channel_set_raw(lh_os_logger_daily_channel_t *self, lh_bool_t raw);

/**
 * @brief The file of the day (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 */
const lh_os_fs_file_t *
lh_os_logger_daily_channel_get_file(const lh_os_logger_daily_channel_t *self);

/**
 * @brief The file of the day, to change in place (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_fs_file_t *
lh_os_logger_daily_channel_get_file_mut(lh_os_logger_daily_channel_t *self);

/**
 * @brief The local day of its file, in days since the epoch (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_os_logger_daily_channel_get_day(const lh_os_logger_daily_channel_t *self);

/**
 * @brief Store the local day of its file, in days since the epoch (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 * @param day The new value.
 */
lh_void
lh_os_logger_daily_channel_set_day(lh_os_logger_daily_channel_t *self, lh_s64_t day);

/**
 * @brief The local day retention last ran for (level 1).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_os_logger_daily_channel_get_pruned_day(const lh_os_logger_daily_channel_t *self);

/**
 * @brief Store the local day retention last ran for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 * @param pruned_day The new value.
 */
lh_void
lh_os_logger_daily_channel_set_pruned_day(lh_os_logger_daily_channel_t *self, lh_s64_t pruned_day);

/**
 * @brief Allocate channel @p id with no directory and no file (level 2).
 *
 * @param id The channel id.
 * @return The channel, or ::lh_null when memory is short.
 */
lh_os_logger_daily_channel_t *
lh_os_logger_daily_channel_try_create(lh_u32_t id);

/**
 * @brief Close the file of the day, if one is open (level 2).
 *
 * @param self The channel; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_os_logger_daily_channel_close_file(lh_os_logger_daily_channel_t *self);

/**
 * @brief Close the file and free the channel; it must be unlinked (level 3).
 *
 * @param self The channel, from ::lh_os_logger_daily_channel_try_create.
 */
lh_void
lh_os_logger_daily_channel_destroy(lh_os_logger_daily_channel_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_LOGGER_DAILY_CHANNEL_H */
