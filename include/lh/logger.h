/**
 * @file logger.h
 * @brief Level-filtered logger: flags plus one optional callback per level.
 *
 * PSR-3 shaped, not PHP-shaped: eight shortcuts (::lh_logger_debug, …) take
 * the logger, a format mask, and substitutions — no level argument. The
 * umbrella ::lh_logger_log also takes a ::lh_logger_level_t and dispatches
 * to that slot.
 *
 * Filtering is two gates, both before the callback:
 * 1. @c flags — `lh_bit_disjoint` with `lh_bit_mask(level)` drops the record.
 * 2. The slot's callback — ::lh_null means that level is not written.
 *
 * Formatting is the callback's job. There is no scratch string on the
 * logger; when ::lh_str_t exists, log calls can grow a typed overload.
 * Until then the mask is ::lh_str_cptr, same as ::lh_str_ptr_format_text.
 *
 * @c context is not a second sink: it is the pointer the callbacks receive,
 * typically an ::lh_io_writer_t * (or UART / `FILE *`).
 */

#ifndef LH_LOGGER_H
#define LH_LOGGER_H

#include <lh/compiler/extern/c.h>
#include <lh/logger/emit/cb.h>
#include <lh/logger/fields.h>
#include <lh/logger/level.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#include <stdarg.h>

/* ── shortcuts ───────────────────────────────────────────────────────────── */

/**
 * @def lh_logger_emergency(self, ...)
 * @brief Emit at ::lh_logger_level_emergency.
 * @param self Logger.
 * @param ...  Format mask followed by its arguments.
 * @return See ::lh_logger_log_v.
 */
#define lh_logger_emergency(self, ...) lh_logger_log((self), lh_logger_level_emergency, __VA_ARGS__)

/**
 * @def lh_logger_alert(self, ...)
 * @brief Emit at ::lh_logger_level_alert.
 * @param self Logger.
 * @param ...  Format mask followed by its arguments.
 * @return See ::lh_logger_log_v.
 */
#define lh_logger_alert(self, ...) lh_logger_log((self), lh_logger_level_alert, __VA_ARGS__)

/**
 * @def lh_logger_critical(self, ...)
 * @brief Emit at ::lh_logger_level_critical.
 * @param self Logger.
 * @param ...  Format mask followed by its arguments.
 * @return See ::lh_logger_log_v.
 */
#define lh_logger_critical(self, ...) lh_logger_log((self), lh_logger_level_critical, __VA_ARGS__)

/**
 * @def lh_logger_error(self, ...)
 * @brief Emit at ::lh_logger_level_error.
 * @param self Logger.
 * @param ...  Format mask followed by its arguments.
 * @return See ::lh_logger_log_v.
 */
#define lh_logger_error(self, ...) lh_logger_log((self), lh_logger_level_error, __VA_ARGS__)

/**
 * @def lh_logger_warning(self, ...)
 * @brief Emit at ::lh_logger_level_warning.
 * @param self Logger.
 * @param ...  Format mask followed by its arguments.
 * @return See ::lh_logger_log_v.
 */
#define lh_logger_warning(self, ...) lh_logger_log((self), lh_logger_level_warning, __VA_ARGS__)

/**
 * @def lh_logger_notice(self, ...)
 * @brief Emit at ::lh_logger_level_notice.
 * @param self Logger.
 * @param ...  Format mask followed by its arguments.
 * @return See ::lh_logger_log_v.
 */
#define lh_logger_notice(self, ...) lh_logger_log((self), lh_logger_level_notice, __VA_ARGS__)

/**
 * @def lh_logger_info(self, ...)
 * @brief Emit at ::lh_logger_level_info.
 * @param self Logger.
 * @param ...  Format mask followed by its arguments.
 * @return See ::lh_logger_log_v.
 */
#define lh_logger_info(self, ...) lh_logger_log((self), lh_logger_level_info, __VA_ARGS__)

/**
 * @def lh_logger_debug(self, ...)
 * @brief Emit at ::lh_logger_level_debug.
 * @param self Logger.
 * @param ...  Format mask followed by its arguments.
 * @return See ::lh_logger_log_v.
 */
#define lh_logger_debug(self, ...) lh_logger_log((self), lh_logger_level_debug, __VA_ARGS__)

/* ── struct ──────────────────────────────────────────────────────────────── */

/**
 * @struct lh_logger
 * @brief A level-filtered logger with a callback slot per severity.
 *
 * The concrete fields are expanded by ::lh_logger_fields.
 */
typedef struct lh_logger
{
    lh_logger_fields(lh_logger_emit_fn);
} lh_logger_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── set / init ──────────────────────────────────────────────────────────── */

/**
 * @brief Copy logger state from @p other into @p self.
 *
 * @param self  Destination logger.
 * @param other Source logger.
 */
void
lh_logger_assign(lh_logger_t *self, const lh_logger_t *other);

/**
 * @brief Reconfigure every field of an already-initialized logger at once.
 *
 * Reach for this when @p self is live and you're changing it (e.g. swapping
 * sinks at runtime); for first-time construction with an explicit callback
 * per level, use ::lh_logger_init_of instead — the name is the hint.
 *
 * @param self         Logger to modify.
 * @param flags        Level filter.
 * @param emergency_cb Emergency slot (may be ::lh_null).
 * @param alert_cb     Alert slot (may be ::lh_null).
 * @param critical_cb  Critical slot (may be ::lh_null).
 * @param error_cb     Error slot (may be ::lh_null).
 * @param warning_cb   Warning slot (may be ::lh_null).
 * @param notice_cb    Notice slot (may be ::lh_null).
 * @param info_cb      Info slot (may be ::lh_null).
 * @param debug_cb     Debug slot (may be ::lh_null).
 * @param context      Userdata passed to whichever slot runs.
 */
void
lh_logger_set(lh_logger_t *self, lh_logger_level_flags_t flags, lh_logger_emit_cb emergency_cb,
              lh_logger_emit_cb alert_cb, lh_logger_emit_cb critical_cb, lh_logger_emit_cb error_cb,
              lh_logger_emit_cb warning_cb, lh_logger_emit_cb notice_cb, lh_logger_emit_cb info_cb,
              lh_logger_emit_cb debug_cb, lh_ptr context);

/**
 * @brief Initialize a fresh logger with an explicit callback per level.
 *
 * The per-level counterpart to ::lh_logger_init (which stores one callback
 * in every slot); use ::lh_logger_set instead once @p self is already live
 * and you're just reconfiguring it.
 *
 * @param self         Logger to initialize.
 * @param flags        Level filter.
 * @param emergency_cb Emergency slot (may be ::lh_null).
 * @param alert_cb     Alert slot (may be ::lh_null).
 * @param critical_cb  Critical slot (may be ::lh_null).
 * @param error_cb     Error slot (may be ::lh_null).
 * @param warning_cb   Warning slot (may be ::lh_null).
 * @param notice_cb    Notice slot (may be ::lh_null).
 * @param info_cb      Info slot (may be ::lh_null).
 * @param debug_cb     Debug slot (may be ::lh_null).
 * @param context      Userdata passed to whichever slot runs.
 */
void
lh_logger_init_of(lh_logger_t *self, lh_logger_level_flags_t flags, lh_logger_emit_cb emergency_cb,
                  lh_logger_emit_cb alert_cb, lh_logger_emit_cb critical_cb,
                  lh_logger_emit_cb error_cb, lh_logger_emit_cb warning_cb,
                  lh_logger_emit_cb notice_cb, lh_logger_emit_cb info_cb,
                  lh_logger_emit_cb debug_cb, lh_ptr context);

/**
 * @brief Initialize a logger: @p emit_cb is stored in every level slot.
 *
 * Same callback on all eight fields is the usual case; use ::lh_logger_init_of
 * afterwards to null (or replace) individual slots.
 *
 * @param self    Logger to initialize.
 * @param flags   Level filter.
 * @param emit_cb Callback for every slot (may be ::lh_null).
 * @param context Userdata passed to @p emit_cb.
 */
void
lh_logger_init(lh_logger_t *self, lh_logger_level_flags_t flags, lh_logger_emit_cb emit_cb,
               lh_ptr context);

/**
 * @brief Reset a logger (no flags, all slots null, null context).
 *
 * @param self Logger to reset.
 */
void
lh_logger_deinit(lh_logger_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Get the current level filter from @p self.
 * @param self Logger to read from.
 */
lh_logger_level_flags_t
lh_logger_get_flags(const lh_logger_t *self);

/**
 * @brief Get the callback stored for @p level.
 *
 * @param self  Logger to read from.
 * @param level Slot to read (0–7).
 * @return That slot's callback (may be ::lh_null).
 */
lh_logger_emit_cb
lh_logger_get_emit_cb(const lh_logger_t *self, lh_logger_level_t level);

/**
 * @brief Get the userdata passed to callbacks.
 * @param self Logger to read from.
 */
lh_ptr
lh_logger_get_context(const lh_logger_t *self);

/* ── operation ───────────────────────────────────────────────────────────── */

/**
 * @brief Dispatch @p fmt / @p args at @p level, if flags and the slot allow.
 *
 * The `va_list` primitive; see ::lh_logger_log for the variadic wrapper.
 * Dropped by the filter or a null slot: returns 0 and does not call.
 *
 * @param self  Logger.
 * @param level Record severity.
 * @param fmt   NUL-terminated format mask (::lh_str_ptr_format_text).
 * @param args  Arguments, one per conversion in @p fmt.
 *
 * @return Whatever the slot callback returns, or 0 if filtered / no slot.
 */
lh_ssize_t
lh_logger_log_v(lh_logger_t *self, lh_logger_level_t level, lh_str_cptr fmt, va_list args);

/**
 * @brief Dispatch @p fmt at @p level, if flags and the slot allow.
 *
 * @param self  Logger.
 * @param level Record severity.
 * @param fmt   NUL-terminated format mask.
 * @param ...   Arguments, one per conversion in @p fmt.
 *
 * @return See ::lh_logger_log_v.
 */
lh_ssize_t
lh_logger_log(lh_logger_t *self, lh_logger_level_t level, lh_str_cptr fmt, ...);

LH_COMPILER_EXTERN_C_END

#endif /* LH_LOGGER_H */
