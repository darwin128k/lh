/**
 * @file capture.h
 * @brief ::lh_os_logger_capture_t — log lines held on the thread that wrote them,
 *        to be handed out later on another.
 *
 * A worker that must not log directly (the sink is not thread-safe, say) begins
 * a capture scope; whatever it logs while the scope is open is copied into a
 * bounded buffer of lines it owns. When the work is done, the owning thread
 * replays the lines in order and clears the buffer. Pushing never locks or
 * waits: a full buffer drops the new line and counts it.
 *
 * The scope is per thread and nests: ::lh_os_logger_capture_begin returns the
 * capture it replaced, ::lh_os_logger_capture_end puts it back. One capture is
 * used by one thread at a time; the caller orders the hand-over (a task's
 * completion, a join).
 */

#ifndef LH_OS_LOGGER_CAPTURE_H
#define LH_OS_LOGGER_CAPTURE_H

#include <lh/os/logger/capture/replay/cb.h>

#include <stdarg.h>

/** @brief A bounded buffer of captured lines over the caller's storage. */
struct lh_os_logger_capture
{
    lh_os_logger_capture_line_t *lines; /**< the caller's storage, @c capacity lines */
    lh_usize_t capacity;                /**< lines @c lines has room for */
    lh_usize_t count;                   /**< lines held, from the first */
    lh_usize_t dropped;                 /**< lines refused for want of room */
};
/** @copybrief lh_os_logger_capture */
typedef struct lh_os_logger_capture lh_os_logger_capture_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The caller's storage (level 1).
 *
 * @param self The capture; checked by ::lh_assert_runtime_ref.
 */
lh_os_logger_capture_line_t *
lh_os_logger_capture_get_lines(const lh_os_logger_capture_t *self);

/**
 * @brief Store the caller's storage (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The capture; checked by ::lh_assert_runtime_ref.
 * @param lines The new value.
 */
lh_void
lh_os_logger_capture_set_lines(lh_os_logger_capture_t *self, lh_os_logger_capture_line_t *lines);

/**
 * @brief Lines the storage has room for (level 1).
 *
 * @param self The capture; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_os_logger_capture_get_capacity(const lh_os_logger_capture_t *self);

/**
 * @brief Store the lines the storage has room for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self     The capture; checked by ::lh_assert_runtime_ref.
 * @param capacity The new value.
 */
lh_void
lh_os_logger_capture_set_capacity(lh_os_logger_capture_t *self, lh_usize_t capacity);

/**
 * @brief Lines held (level 1).
 *
 * @param self The capture; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_os_logger_capture_get_count(const lh_os_logger_capture_t *self);

/**
 * @brief Store the lines held (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The capture; checked by ::lh_assert_runtime_ref.
 * @param count The new value.
 */
lh_void
lh_os_logger_capture_set_count(lh_os_logger_capture_t *self, lh_usize_t count);

/**
 * @brief Lines refused for want of room since the last clear (level 1).
 *
 * @param self The capture; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_os_logger_capture_get_dropped(const lh_os_logger_capture_t *self);

/**
 * @brief Store the lines refused for want of room (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self    The capture; checked by ::lh_assert_runtime_ref.
 * @param dropped The new value.
 */
lh_void
lh_os_logger_capture_set_dropped(lh_os_logger_capture_t *self, lh_usize_t dropped);

/**
 * @brief The capture the calling thread's lines go to, or ::lh_null outside a scope.
 */
lh_os_logger_capture_t *lh_os_logger_capture_get_current(lh_void);

/**
 * @brief Open a capture scope on the calling thread.
 *
 * @param self The capture the thread's lines go to from now on; ::lh_null suspends
 *             capture, so that lines go out directly.
 * @return The capture it replaced (::lh_null for none), for ::lh_os_logger_capture_end.
 */
lh_os_logger_capture_t *
lh_os_logger_capture_begin(lh_os_logger_capture_t *self);

/**
 * @brief Close the scope ::lh_os_logger_capture_begin opened.
 *
 * @param previous What that call returned.
 */
lh_void
lh_os_logger_capture_end(lh_os_logger_capture_t *previous);

/**
 * @brief Forget the held lines and the drop count (level 2).
 *
 * @param self The capture; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_os_logger_capture_clear(lh_os_logger_capture_t *self);

/**
 * @brief The held line at @p index, or ::lh_null past the last one (level 2).
 *
 * @param self  The capture; checked by ::lh_assert_runtime_ref.
 * @param index From 0.
 */
const lh_os_logger_capture_line_t *
lh_os_logger_capture_get_line(const lh_os_logger_capture_t *self, lh_usize_t index);

/**
 * @brief Format a line and hold it, or count it as dropped when the buffer is full
 *        (level 2).
 *
 * The text is cut to ::LH_OS_LOGGER_RECORD_TEXT_SIZE - 1 characters.
 *
 * @param self    The capture; checked by ::lh_assert_runtime_ref.
 * @param target  What the line is meant for; given back on replay.
 * @param context The value that goes with @p target; given back on replay.
 * @param level   The line's level.
 * @param fmt     NUL-terminated format mask (::lh_str_ptr_format_text); checked by
 *                ::lh_assert_runtime_ref.
 * @param args    Substitutions for @p fmt; read only when the line is held.
 * @return ::lh_bool_false when the line was dropped.
 */
lh_bool_t
lh_os_logger_capture_try_push_v(lh_os_logger_capture_t *self, lh_cptr_of(lh_void) target,
                                lh_ptr context, lh_logger_level_t level, const lh_str_ptr fmt,
                                va_list args);

/**
 * @brief Make @p self an empty capture over @p lines (level 3).
 *
 * @param self     The capture; checked by ::lh_assert_runtime_ref.
 * @param lines    Room for @p capacity lines; must outlive the capture.
 * @param capacity Lines @p lines has room for; 0 drops every line.
 */
lh_void
lh_os_logger_capture_init(lh_os_logger_capture_t *self, lh_os_logger_capture_line_t *lines,
                          lh_usize_t capacity);

/**
 * @brief Hand every held line to @p fn, in order, with capture suspended on the
 *        calling thread (level 3).
 *
 * The lines stay held: ::lh_os_logger_capture_clear forgets them.
 *
 * @param self The capture; checked by ::lh_assert_runtime_ref.
 * @param fn   Called once per line; checked by ::lh_assert_runtime_ref.
 * @param user Passed to @p fn.
 */
lh_void
lh_os_logger_capture_replay(const lh_os_logger_capture_t *self, lh_os_logger_capture_replay_cb fn,
                            lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_CAPTURE_H */
