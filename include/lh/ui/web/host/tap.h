/**
 * @file tap.h
 * @brief ::lh_ui_web_host_tap_t — a reader that remembers what it answered last.
 *
 * A streaming route's handler reads the request body through a reader and
 * returns; it does not say why it stopped. The host needs to know: the body
 * ended and validated (the last read was 0), more is on its way (the last read
 * was ::LH_IO_PENDING), it failed (an error), or the handler simply stopped
 * early (the last read gave bytes, or nothing was read). The tap sits between
 * the handler and the decoded body and keeps that last result.
 */

#ifndef LH_UI_WEB_HOST_TAP_H
#define LH_UI_WEB_HOST_TAP_H

#include <lh/core/bool.h>
#include <lh/core/io/reader.h>
#include <lh/ui/web/host/tap/fields.h>

/**
 * @brief A reader that remembers its last result.
 */
struct lh_ui_web_host_tap
{
    /** Members: ::lh_ui_web_host_tap_fields. */
    lh_ui_web_host_tap_fields(lh_io_reader_t, lh_ssize_t);
};

/** @copybrief lh_ui_web_host_tap */
typedef struct lh_ui_web_host_tap lh_ui_web_host_tap_t;

/**
 * @def LH_UI_WEB_HOST_TAP_NOTHING_YET
 * @brief The last result of a tap nothing was read through yet (positive: "no end").
 */
#define LH_UI_WEB_HOST_TAP_NOTHING_YET 1

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The reader tapped, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_io_reader_t *
lh_ui_web_host_tap_get_source(const lh_ui_web_host_tap_t *self);

/**
 * @brief The reader tapped, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_io_reader_t *
lh_ui_web_host_tap_get_source_mut(lh_ui_web_host_tap_t *self);

/**
 * @brief The last result a read returned (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_ssize_t
lh_ui_web_host_tap_get_last(const lh_ui_web_host_tap_t *self);

/**
 * @brief Store the last result a read returned (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_tap_set_last(lh_ui_web_host_tap_t *self, lh_ssize_t value);

/**
 * @brief The tap as an ::lh_io_reader_t.
 * @param self Tap; must not be null. It must outlive the reader.
 * @param out  Receives the reader.
 */
lh_void
lh_ui_web_host_tap_get_reader(lh_ui_web_host_tap_t *self, lh_io_reader_t *out);

/**
 * @brief Tap @p source; the last result starts as ::LH_UI_WEB_HOST_TAP_NOTHING_YET.
 * @param self   Tap; must not be null.
 * @param source The reader passed through, copied.
 */
lh_void
lh_ui_web_host_tap_init(lh_ui_web_host_tap_t *self, const lh_io_reader_t *source);

/**
 * @brief Read through the tap and remember the result (an ::lh_io_reader_read_fn).
 *
 * A read of zero bytes passes nothing and remembers nothing.
 *
 * @param context The ::lh_ui_web_host_tap_t.
 * @param out     Destination.
 * @param size    Capacity of @p out.
 * @return What the source returned.
 */
lh_ssize_t
lh_ui_web_host_tap_read(lh_self_ptr context, lh_ptr out, lh_usize_t size);

/**
 * @brief Whether the last read reached the end of the source.
 * @param self Tap; must not be null.
 * @return ::lh_bool_true when the last result was 0.
 */
lh_bool_t
lh_ui_web_host_tap_is_at_end(const lh_ui_web_host_tap_t *self);

/**
 * @brief Whether the last read found nothing yet.
 * @param self Tap; must not be null.
 * @return ::lh_bool_true when the last result was ::LH_IO_PENDING.
 */
lh_bool_t
lh_ui_web_host_tap_is_pending(const lh_ui_web_host_tap_t *self);

/**
 * @brief Whether the last read failed.
 * @param self Tap; must not be null.
 * @return ::lh_bool_true when the last result was an error (negative, not pending).
 */
lh_bool_t
lh_ui_web_host_tap_is_failed(const lh_ui_web_host_tap_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_TAP_H */
