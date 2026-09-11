/**
 * @file fields.h
 * @brief Macro for declaring `write_cb` / `context` fields of a writer struct.
 */

#ifndef LH_IO_WRITER_FIELDS_H
#define LH_IO_WRITER_FIELDS_H

#include <lh/ptr.h>

/**
 * @def lh_io_writer_fields(write_fn_type)
 * @brief Expands to the `write_cb` and `context` members of a writer struct.
 *
 * Intended to be embedded inside a struct body so ::lh_io_writer_t (and
 * compatible wrappers) share one declaration shape.
 *
 * @param write_fn_type Function type for writing (::lh_io_writer_write_fn);
 *                       the member is `write_fn_type *write_cb`.
 *
 * Example usage:
 * @code{.c}
 * typedef struct lh_io_writer {
 *     lh_io_writer_fields(lh_io_writer_write_fn);
 * } lh_io_writer_t;
 * @endcode
 *
 * @see lh_io_writer_t
 */
#define lh_io_writer_fields(write_fn_type)                                                         \
    write_fn_type *write_cb;                                                                       \
    lh_ptr context

#endif /* LH_IO_WRITER_FIELDS_H */
