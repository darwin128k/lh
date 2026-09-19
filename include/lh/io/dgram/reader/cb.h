/**
 * @file cb.h
 * @brief Callable wrapper for datagram recv callbacks.
 *
 * @see lh_io_dgram_writer_send_cb
 */

#ifndef LH_IO_DGRAM_READER_CB_H
#define LH_IO_DGRAM_READER_CB_H

#include <lh/io/dgram/reader/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_io_dgram_reader_recv_cb
 * @brief Pointer to ::lh_io_dgram_reader_recv_fn.
 */
#define lh_io_dgram_reader_recv_cb lh_ptr_of(lh_io_dgram_reader_recv_fn)

#endif /* LH_IO_DGRAM_READER_CB_H */
