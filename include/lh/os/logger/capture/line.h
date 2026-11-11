/**
 * @file line.h
 * @brief ::lh_os_logger_capture_line_t — one line held by a log capture: the
 *        record, and the target it was meant for.
 *
 * The target is two opaque values the producer chose (a sink and its context,
 * say); the capture never reads them, it only gives them back on replay. Whatever
 * they name must outlive the replay.
 */

#ifndef LH_OS_LOGGER_CAPTURE_LINE_H
#define LH_OS_LOGGER_CAPTURE_LINE_H

#include <lh/os/logger/record.h>

/** @brief One captured line. */
struct lh_os_logger_capture_line
{
    lh_cptr_of(lh_void) target;   /**< what the line was meant for */
    lh_ptr context;               /**< the value that goes with the target */
    lh_os_logger_record_t record; /**< the line: its level and text */
};
/** @copybrief lh_os_logger_capture_line */
typedef struct lh_os_logger_capture_line lh_os_logger_capture_line_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What the line was meant for (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 */
lh_cptr_of(lh_void) lh_os_logger_capture_line_get_target(const lh_os_logger_capture_line_t *self);

/**
 * @brief Store what the line was meant for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self   The line; checked by ::lh_assert_runtime_ref.
 * @param target The new value.
 */
lh_void
lh_os_logger_capture_line_set_target(lh_os_logger_capture_line_t *self, lh_cptr_of(lh_void) target);

/**
 * @brief The value that goes with the target (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_logger_capture_line_get_context(const lh_os_logger_capture_line_t *self);

/**
 * @brief Store the value that goes with the target (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self    The line; checked by ::lh_assert_runtime_ref.
 * @param context The new value.
 */
lh_void
lh_os_logger_capture_line_set_context(lh_os_logger_capture_line_t *self, lh_ptr context);

/**
 * @brief The record: the line's level and text (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_os_logger_record_t *
lh_os_logger_capture_line_get_record(const lh_os_logger_capture_line_t *self);

/**
 * @brief The record, to change in place (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_logger_record_t *
lh_os_logger_capture_line_get_record_mut(lh_os_logger_capture_line_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_CAPTURE_LINE_H */
