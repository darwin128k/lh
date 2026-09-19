/**
 * @file fields.h
 * @brief Macro for declaring `send_cb` / `context` fields of a datagram writer.
 */

#ifndef LH_IO_DGRAM_WRITER_FIELDS_H
#define LH_IO_DGRAM_WRITER_FIELDS_H

#include <lh/ptr.h>

/**
 * @def lh_io_dgram_writer_fields(send_fn_type)
 * @brief Expands to the `send_cb` and `context` members of a datagram writer.
 *
 * @param send_fn_type Function type for sending (::lh_io_dgram_writer_send_fn);
 *                     the member is `send_fn_type *send_cb`.
 */
#define lh_io_dgram_writer_fields(send_fn_type)                                                    \
    send_fn_type *send_cb;                                                                         \
    lh_ptr context

#endif /* LH_IO_DGRAM_WRITER_FIELDS_H */
