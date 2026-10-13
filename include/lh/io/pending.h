/**
 * @file pending.h
 * @brief Signed answers a stream read or write may return besides a byte count.
 *
 * A stream read has three answers that are not data, and they mean three
 * different things to whoever is reading:
 *
 * - `0` — the end. The file is read to its end, the peer closed the socket.
 *   Nothing will come, ever.
 * - ::LH_IO_PENDING — nothing now. A socket with nothing received, a serial port
 *   whose line is quiet. Ask again later; the stream is fine.
 * - ::LH_IO_ERROR (and any other negative) — a failure.
 *
 * A blocking source never answers ::LH_IO_PENDING: it waits instead. A
 * non-blocking one (a socket after ::lh_os_net_socket_set_blocking, a serial
 * port) answers it on every empty read, so a caller that polls must tell it from
 * a failure — it is negative, so a caller that only checks "negative means stop"
 * stops, which is the safe misreading of it. Treating "nothing now" as "the end"
 * is the unsafe one: that is how a quiet line is taken for a closed one.
 *
 * Writes the same way: ::LH_IO_PENDING when nothing could be taken now (a full
 * send buffer) and nothing was; ::LH_IO_ERROR when the sink failed (or, for
 * ::lh_io_writer_print, when the format did not fit).
 */

#ifndef LH_IO_PENDING_H
#define LH_IO_PENDING_H

#include <lh/cast/static.h>
#include <lh/size.h>

/**
 * @def LH_IO_ERROR
 * @brief The stream (or print) failed (`-1`).
 *
 * Canonical failure for ::lh_ssize_t IO returns. Host callbacks may still use
 * other negatives; callers that only care "did it fail?" test `< 0`.
 */
#define LH_IO_ERROR (lh_cast_static(lh_ssize_t, -1))

/**
 * @def LH_IO_PENDING
 * @brief Nothing could be read or written now; the stream is fine (`-2`).
 */
#define LH_IO_PENDING (lh_cast_static(lh_ssize_t, -2))

#endif /* LH_IO_PENDING_H */
