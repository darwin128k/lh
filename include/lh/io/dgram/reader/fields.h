/**
 * @file fields.h
 * @brief Macro for declaring `recv_cb` / `context` fields of a datagram reader.
 */

#ifndef LH_IO_DGRAM_READER_FIELDS_H
#define LH_IO_DGRAM_READER_FIELDS_H

#include <lh/ptr.h>

/**
 * @def lh_io_dgram_reader_fields(recv_fn_type)
 * @brief Expands to the `recv_cb` and `context` members of a datagram reader.
 *
 * @param recv_fn_type Function type for receiving (::lh_io_dgram_reader_recv_fn);
 *                     the member is `recv_fn_type *recv_cb`.
 */
#define lh_io_dgram_reader_fields(recv_fn_type)                                                    \
    recv_fn_type *recv_cb;                                                                         \
    lh_ptr context

#endif /* LH_IO_DGRAM_READER_FIELDS_H */
