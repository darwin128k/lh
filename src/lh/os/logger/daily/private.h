/**
 * @file private.h
 * @brief Private to `daily.c`: the functions of the daily sink below its public API.
 *
 * Its fields, its channels, retention and the day files. They name the private
 * channel type, so they are not in `lh/os/logger/daily.h`.
 */

#ifndef LH_SRC_OS_LOGGER_DAILY_PRIVATE_H
#define LH_SRC_OS_LOGGER_DAILY_PRIVATE_H

#include <lh/os/logger/daily.h>
#include <lh/os/logger/daily/channel.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The allocator of @p self, to change in place (level 1).
 */
lh_memory_allocator_t *
lh_os_logger_daily_get_allocator_mut(lh_os_logger_daily_t *self);

/**
 * @brief The channels of @p self (level 1).
 */
const lh_list_t *
lh_os_logger_daily_get_channels(const lh_os_logger_daily_t *self);

/**
 * @brief The channels of @p self, to change in place (level 1).
 */
lh_list_t *
lh_os_logger_daily_get_channels_mut(lh_os_logger_daily_t *self);

/**
 * @brief The format of @p self (level 1).
 */
lh_os_logger_daily_format_cb
lh_os_logger_daily_get_format_fn(const lh_os_logger_daily_t *self);

/**
 * @brief Store the format of @p self (level 1).
 */
lh_void
lh_os_logger_daily_set_format_fn(lh_os_logger_daily_t *self, lh_os_logger_daily_format_cb format);

/**
 * @brief The clock of @p self (level 1).
 */
lh_os_logger_daily_clock_cb
lh_os_logger_daily_get_clock_fn(const lh_os_logger_daily_t *self);

/**
 * @brief Store the clock of @p self (level 1).
 */
lh_void
lh_os_logger_daily_set_clock_fn(lh_os_logger_daily_t *self, lh_os_logger_daily_clock_cb clock);

/**
 * @brief The user of @p self (level 1).
 */
lh_ptr
lh_os_logger_daily_get_user(const lh_os_logger_daily_t *self);

/**
 * @brief Store the user of @p self (level 1).
 */
lh_void
lh_os_logger_daily_set_user(lh_os_logger_daily_t *self, lh_ptr user);

/**
 * @brief The line of @p self, to change in place (level 1).
 */
lh_str_t *
lh_os_logger_daily_get_line_mut(lh_os_logger_daily_t *self);

/**
 * @brief Whether @p self is reported (level 1).
 */
lh_bool_t
lh_os_logger_daily_is_reported(const lh_os_logger_daily_t *self);

/**
 * @brief Store the reported of @p self (level 1).
 */
lh_void
lh_os_logger_daily_set_reported(lh_os_logger_daily_t *self, lh_bool_t reported);

/**
 * @brief The channel of @p self (level 1).
 */
lh_os_logger_daily_channel_t *
lh_os_logger_daily_get_channel(lh_list_node_t *node);

/**
 * @brief The local day of @p utc, in days since the epoch; UTC's when the zone is unknown.
 */
lh_s64_t
lh_os_logger_daily_get_local_day(lh_timestamp_t utc);

/**
 * @brief The sink's clock when none is given: the system's.
 */
lh_bool_t
lh_os_logger_daily_get_system_now(lh_ptr user, lh_timestamp_t *out);

/**
 * @brief The day of a `YYYY-MM-DD.log` name, in days since the epoch.
 */
lh_bool_t
lh_os_logger_daily_try_get_day(lh_str_view_t name, lh_s64_t *out);

/**
 * @brief @p dir joined with the file name of @p day.
 */
lh_bool_t
lh_os_logger_daily_get_path(const lh_fs_path_t *dir, lh_s64_t day, lh_fs_path_t *out);

/**
 * @brief Report the first file failure; later ones would only repeat it.
 */
lh_void
lh_os_logger_daily_report(lh_os_logger_daily_t *self, const lh_char_t *what,
                          const lh_fs_path_t *path);

/**
 * @brief Find (internal).
 */
lh_os_logger_daily_channel_t *
lh_os_logger_daily_find(const lh_os_logger_daily_t *self, lh_u32_t id);

/**
 * @brief Delete the channel's daily files older than its retention allows on @p today.
 */
lh_void
lh_os_logger_daily_prune(lh_os_logger_daily_t *self, lh_os_logger_daily_channel_t *channel,
                         lh_s64_t today);

/**
 * @brief Close channel (internal).
 */
lh_void
lh_os_logger_daily_close_channel(lh_os_logger_daily_t *self, const lh_os_logger_record_t *record);

/**
 * @brief The channel's file for @p day: the open one, or the next one opened.
 */
lh_bool_t
lh_os_logger_daily_open_day(lh_os_logger_daily_t *self, lh_os_logger_daily_channel_t *channel,
                            lh_s64_t day);

/**
 * @brief Open (internal).
 */
lh_void
lh_os_logger_daily_open(lh_os_logger_daily_t *self, const lh_os_logger_record_t *record);

/**
 * @brief Write (internal).
 */
lh_void
lh_os_logger_daily_write(lh_os_logger_daily_t *self, const lh_os_logger_record_t *record);

/**
 * @brief Emit (internal).
 */
lh_void
lh_os_logger_daily_emit(lh_ptr user, const lh_os_logger_record_t *record);

/**
 * @brief Lines are written straight to the files: there is no buffer of ours to flush.
   A flush is when a quiet channel notices a new day and cleans up.
 */
lh_void
lh_os_logger_daily_flush(lh_ptr user);

/**
 * @brief Close (internal).
 */
lh_void
lh_os_logger_daily_close(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_LOGGER_DAILY_PRIVATE_H */
