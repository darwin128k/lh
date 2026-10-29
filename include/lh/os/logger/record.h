/**
 * @file record.h
 * @brief ::lh_os_logger_record_t — one entry of a log queue: a line, or a channel opening or
 *        closing.
 *
 * Fixed size, so a queue holds records without allocating. A line carries its
 * channel, level, UTC time, tag and text. A channel is opened and closed by
 * records too, in the same queue, so whoever writes the files sees the channel
 * before its first line and after its last: the writer's state never needs a lock.
 * Text longer than its field is cut.
 */

#ifndef LH_OS_LOGGER_RECORD_H
#define LH_OS_LOGGER_RECORD_H

#include <lh/config.h>
#include <lh/logger/level.h>
#include <lh/timestamp.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/logger/record.h requires LH_LIBRARY_OPTION_OS"
#endif

/** @brief Room for a tag, the NUL included. */
#define LH_OS_LOGGER_RECORD_TAG_SIZE 64U

/** @brief Room for a line's text or a channel's directory, the NUL included. */
#define LH_OS_LOGGER_RECORD_TEXT_SIZE 1024U

/** @brief What a record is. */
enum lh_os_logger_record_kind
{
    /** A line of a channel. */
    lh_os_logger_record_kind_line = 0,
    /** Open a channel: its directory in the text, its retention and format. */
    lh_os_logger_record_kind_open = 1,
    /** Close a channel: its file is closed and its id may be reused. */
    lh_os_logger_record_kind_close = 2
};
/** @copybrief lh_os_logger_record_kind */
typedef enum lh_os_logger_record_kind lh_os_logger_record_kind_t;

/** @brief One entry of a log queue. */
struct lh_os_logger_record
{
    lh_u32_t channel;                             /**< the channel, chosen by the producer */
    lh_u8_t kind;                                 /**< an ::lh_os_logger_record_kind_t */
    lh_logger_level_t level;                      /**< a line's level */
    lh_bool_t raw;                                /**< an opened channel writes text only */
    lh_s32_t keep_days;                           /**< an opened channel's days to keep; 0 keeps all */
    lh_timestamp_t time;                          /**< a line's moment, in UTC */
    lh_char_t tag[LH_OS_LOGGER_RECORD_TAG_SIZE];  /**< a line's tag; empty for none */
    lh_char_t text[LH_OS_LOGGER_RECORD_TEXT_SIZE]; /**< a line's text or a channel's directory */
};
/** @copybrief lh_os_logger_record */
typedef struct lh_os_logger_record lh_os_logger_record_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The channel (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_os_logger_record_get_channel(const lh_os_logger_record_t *self);

/**
 * @brief Store the channel (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @param channel The new value.
 */
lh_void
lh_os_logger_record_set_channel(lh_os_logger_record_t *self, lh_u32_t channel);

/**
 * @brief The kind, an ::lh_os_logger_record_kind_t (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_os_logger_record_get_kind(const lh_os_logger_record_t *self);

/**
 * @brief Store the kind, an ::lh_os_logger_record_kind_t (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @param kind The new value.
 */
lh_void
lh_os_logger_record_set_kind(lh_os_logger_record_t *self, lh_u8_t kind);

/**
 * @brief The level of a line (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 */
lh_logger_level_t
lh_os_logger_record_get_level(const lh_os_logger_record_t *self);

/**
 * @brief Store the level of a line (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @param level The new value.
 */
lh_void
lh_os_logger_record_set_level(lh_os_logger_record_t *self, lh_logger_level_t level);

/**
 * @brief Whether an opened channel writes text only (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_logger_record_is_raw(const lh_os_logger_record_t *self);

/**
 * @brief Store whether an opened channel writes text only (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @param raw The new value.
 */
lh_void
lh_os_logger_record_set_raw(lh_os_logger_record_t *self, lh_bool_t raw);

/**
 * @brief The days an opened channel keeps, 0 for all (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_os_logger_record_get_keep_days(const lh_os_logger_record_t *self);

/**
 * @brief Store the days an opened channel keeps, 0 for all (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @param keep_days The new value.
 */
lh_void
lh_os_logger_record_set_keep_days(lh_os_logger_record_t *self, lh_s32_t keep_days);

/**
 * @brief The moment of a line, in UTC (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 */
lh_timestamp_t
lh_os_logger_record_get_time(const lh_os_logger_record_t *self);

/**
 * @brief Store the moment of a line, in UTC (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @param time The new value.
 */
lh_void
lh_os_logger_record_set_time(lh_os_logger_record_t *self, lh_timestamp_t time);

/**
 * @brief The tag of a line (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_os_logger_record_get_tag(const lh_os_logger_record_t *self);

/**
 * @brief The tag of a line, to change in place (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_char_t *
lh_os_logger_record_get_tag_mut(lh_os_logger_record_t *self);

/**
 * @brief The text of a line or the directory of a channel (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_os_logger_record_get_text(const lh_os_logger_record_t *self);

/**
 * @brief The text of a line or the directory of a channel, to change in place (level 1).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_char_t *
lh_os_logger_record_get_text_mut(lh_os_logger_record_t *self);

/**
 * @brief Copy @p text into the tag, cut to ::LH_OS_LOGGER_RECORD_TAG_SIZE - 1 characters
 *        (level 2).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @param text NUL-terminated text, or ::lh_null for no tag.
 */
lh_void
lh_os_logger_record_assign_tag(lh_os_logger_record_t *self, const lh_char_t *text);

/**
 * @brief Copy @p text into the text, cut to ::LH_OS_LOGGER_RECORD_TEXT_SIZE - 1 characters
 *        (level 2).
 *
 * @param self The record; checked by ::lh_assert_runtime_ref.
 * @param text NUL-terminated text, or ::lh_null for none.
 */
lh_void
lh_os_logger_record_assign_text(lh_os_logger_record_t *self, const lh_char_t *text);

/**
 * @brief Make @p self a line of @p channel (level 3).
 *
 * @param self    The record; checked by ::lh_assert_runtime_ref.
 * @param channel The channel, opened before by an ::lh_os_logger_record_init_open record.
 * @param level   The level.
 * @param time    The moment, in UTC.
 * @param tag     The tag, or ::lh_null.
 * @param text    The text; a newline at its end is not doubled.
 */
lh_void
lh_os_logger_record_init_line(lh_os_logger_record_t *self, lh_u32_t channel,
                              lh_logger_level_t level, lh_timestamp_t time, const lh_char_t *tag,
                              const lh_char_t *text);

/**
 * @brief Make @p self the opening of @p channel (level 3).
 *
 * Opening a channel that is open changes its directory, retention and format.
 *
 * @param self      The record; checked by ::lh_assert_runtime_ref.
 * @param channel   The channel.
 * @param dir       The directory its daily files go to; created when missing.
 * @param keep_days Days of files to keep; 0 keeps all.
 * @param raw       Whether its lines are written as text only, without time or level.
 */
lh_void
lh_os_logger_record_init_open(lh_os_logger_record_t *self, lh_u32_t channel, const lh_char_t *dir,
                              lh_s32_t keep_days, lh_bool_t raw);

/**
 * @brief Make @p self the closing of @p channel (level 3).
 *
 * @param self    The record; checked by ::lh_assert_runtime_ref.
 * @param channel The channel.
 */
lh_void
lh_os_logger_record_init_close(lh_os_logger_record_t *self, lh_u32_t channel);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_RECORD_H */
