/**
 * @file fields.h
 * @brief Macro for declaring `read_cb` / `context` fields of a reader struct.
 */

#ifndef LH_IO_READER_FIELDS_H
#define LH_IO_READER_FIELDS_H

#include <lh/ptr.h>

/**
 * @def lh_io_reader_fields(read_fn_type)
 * @brief Expands to the `read_cb` and `context` members of a reader struct.
 *
 * Intended to be embedded inside a struct body so ::lh_io_reader_t (and
 * compatible wrappers) share one declaration shape.
 *
 * @param read_fn_type Function type for reading (::lh_io_reader_read_fn);
 *                      the member is `read_fn_type *read_cb`.
 *
 * Example usage:
 * @code{.c}
 * typedef struct lh_io_reader {
 *     lh_io_reader_fields(lh_io_reader_read_fn);
 * } lh_io_reader_t;
 * @endcode
 *
 * @see lh_io_reader_t
 */
#define lh_io_reader_fields(read_fn_type)                                                          \
    read_fn_type *read_cb;                                                                         \
    lh_ptr context

#endif /* LH_IO_READER_FIELDS_H */
