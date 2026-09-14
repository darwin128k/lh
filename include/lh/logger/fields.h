/**
 * @file fields.h
 * @brief Macro for declaring the members of a logger struct.
 */

#ifndef LH_LOGGER_FIELDS_H
#define LH_LOGGER_FIELDS_H

#include <lh/logger/level/flags.h>
#include <lh/ptr.h>

/**
 * @def lh_logger_fields(emit_fn_type)
 * @brief Expands to the members of a logger struct.
 *
 * Intended to be embedded inside a struct body so ::lh_logger_t (and
 * compatible wrappers) share one declaration shape.
 *
 * @param emit_fn_type Function type for a level slot (::lh_logger_emit_fn);
 *                     each member is `emit_fn_type *…_cb`.
 *
 * @c flags is the level filter. Each `*_cb` is optional: ::lh_null means
 * that level is not written even when its flag is set. @c context is the
 * userdata passed to whichever slot runs (same role as on ::lh_io_writer_t).
 *
 * Example usage:
 * @code{.c}
 * typedef struct lh_logger {
 *     lh_logger_fields(lh_logger_emit_fn);
 * } lh_logger_t;
 * @endcode
 *
 * @see lh_logger_t
 */
#define lh_logger_fields(emit_fn_type)                                                             \
    lh_logger_level_flags_t flags;                                                                 \
    emit_fn_type *emergency_cb;                                                                    \
    emit_fn_type *alert_cb;                                                                        \
    emit_fn_type *critical_cb;                                                                     \
    emit_fn_type *error_cb;                                                                        \
    emit_fn_type *warning_cb;                                                                      \
    emit_fn_type *notice_cb;                                                                       \
    emit_fn_type *info_cb;                                                                         \
    emit_fn_type *debug_cb;                                                                        \
    lh_ptr context

#endif /* LH_LOGGER_FIELDS_H */
