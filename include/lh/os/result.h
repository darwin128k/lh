/**
 * @file result.h
 * @brief What an OS-layer call that returns a count gives back on failure.
 *
 * Reads, writes, sends, receives and directory reads return an
 * ::lh_ssize_t: a count `>= 0`, or ::LH_OS_RESULT_INVALID when the call
 * failed (the reason is in ::lh_os_last_error or ::lh_os_system_last_error).
 * Same spirit as the handle sentinels (::LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID,
 * …): a named "no result", not a bare `-1`.
 *
 * Callers test failure with a negative check (::lh_math_is_negative), not
 * equality: these functions also feed `lh/io` readers and writers, whose
 * contract is "negative on failure".
 */

#ifndef LH_OS_RESULT_H
#define LH_OS_RESULT_H

#include <lh/io/pending.h>

/**
 * @def LH_OS_RESULT_INVALID
 * @brief Failed count-returning OS-layer call (`-1` as ::lh_ssize_t).
 */
#define LH_OS_RESULT_INVALID (lh_cast_static(lh_ssize_t, -1))

/**
 * @def LH_OS_RESULT_PENDING
 * @brief A call on a non-blocking handle that cannot finish **yet** (`-2`).
 *
 * The same value as ::LH_IO_PENDING, on purpose: a socket's send and recv are a
 * stream's write and read (::lh_os_net_socket_get_stream), and the answer passes
 * through unchanged.
 *
 * Not a failure: the handle is fine and the same call will do something later.
 * Negative, so a caller that only knows "negative is failure" still stops — but a
 * caller that polls must tell the two apart, because treating "nothing has
 * arrived" as "the connection is broken" drops every connection the first time
 * it is idle. Nothing is captured into the last-error slot for it: it is the
 * ordinary answer of a poll loop, and formatting an OS message on every empty
 * poll would cost more than the poll.
 */
#define LH_OS_RESULT_PENDING LH_IO_PENDING

#endif /* LH_OS_RESULT_H */
