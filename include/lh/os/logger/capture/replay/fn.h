/**
 * @file fn.h
 * @brief ::lh_os_logger_capture_replay_fn — hand one captured line to its target.
 */

#ifndef LH_OS_LOGGER_CAPTURE_REPLAY_FN_H
#define LH_OS_LOGGER_CAPTURE_REPLAY_FN_H

#include <lh/os/logger/capture/line.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Hand one captured line to its target.
 *
 * Called by ::lh_os_logger_capture_replay with capture suspended on the calling
 * thread, so whatever the callback logs goes out instead of back into the buffer.
 *
 * @param user The value ::lh_os_logger_capture_replay was given.
 * @param line The line; borrowed for the call.
 */
typedef lh_void(lh_os_logger_capture_replay_fn)(lh_ptr user,
                                                const lh_os_logger_capture_line_t *line);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_CAPTURE_REPLAY_FN_H */
