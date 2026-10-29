/**
 * @file call.h
 * @brief ::lh_console_call_t — one run of a command: what was typed and where the reply goes.
 *
 * Built by ::lh_console_run for the length of the handler call; the handler reads it
 * and prints through it. Every pointer in it is borrowed.
 */

#ifndef LH_CONSOLE_CALL_H
#define LH_CONSOLE_CALL_H

#include <stdarg.h>
#include <lh/console/sink/cb.h>
#include <lh/size.h>

/** @brief One run of a command. */
struct lh_console_call
{
    const lh_char_t *scope;       /**< the first word of the line (`qproto`) */
    const lh_char_t *name;        /**< the command word (`info`) */
    lh_usize_t argc;              /**< how many arguments follow the command word */
    const lh_char_t *const *argv; /**< the arguments, @c argc of them */
    lh_console_sink_cb sink;      /**< where the reply goes; ::lh_null drops it */
    lh_ptr out;                   /**< the value the sink is given */
};
/** @copybrief lh_console_call */
typedef struct lh_console_call lh_console_call_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The scope word of the line (level 1).
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_console_call_get_scope(const lh_console_call_t *self);

/**
 * @brief Store the scope word of the line (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 * @param scope The new value.
 */
lh_void
lh_console_call_set_scope(lh_console_call_t *self, const lh_char_t *scope);

/**
 * @brief The command word of the line (level 1).
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_console_call_get_name(const lh_console_call_t *self);

/**
 * @brief Store the command word of the line (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 * @param name The new value.
 */
lh_void
lh_console_call_set_name(lh_console_call_t *self, const lh_char_t *name);

/**
 * @brief How many arguments follow the command word (level 1).
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_console_call_get_argc(const lh_console_call_t *self);

/**
 * @brief Store how many arguments follow the command word (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 * @param argc The new value.
 */
lh_void
lh_console_call_set_argc(lh_console_call_t *self, lh_usize_t argc);

/**
 * @brief The arguments (level 1).
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *const *
lh_console_call_get_argv(const lh_console_call_t *self);

/**
 * @brief Store the arguments (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 * @param argv The new value.
 */
lh_void
lh_console_call_set_argv(lh_console_call_t *self, const lh_char_t *const *argv);

/**
 * @brief Where the reply goes; ::lh_null drops it (level 1).
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 */
lh_console_sink_cb
lh_console_call_get_sink_fn(const lh_console_call_t *self);

/**
 * @brief Store where the reply goes; ::lh_null drops it (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 * @param sink The new value.
 */
lh_void
lh_console_call_set_sink_fn(lh_console_call_t *self, lh_console_sink_cb sink);

/**
 * @brief The value the sink is given (level 1).
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_console_call_get_out(const lh_console_call_t *self);

/**
 * @brief Store the value the sink is given (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The call; checked by ::lh_assert_runtime_ref.
 * @param out The new value.
 */
lh_void
lh_console_call_set_out(lh_console_call_t *self, lh_ptr out);

/**
 * @brief Set up a call over borrowed text (level 2).
 *
 * @param self  The call; must not be ::lh_null (not checked).
 * @param scope The scope word.
 * @param name  The command word.
 * @param argc  How many arguments there are.
 * @param argv  The arguments, @p argc of them, or ::lh_null for none.
 * @param sink  Where the reply goes, or ::lh_null to drop it.
 * @param out   The value the sink is given.
 */
lh_void
lh_console_call_init(lh_console_call_t *self, const lh_char_t *scope, const lh_char_t *name,
                     lh_usize_t argc, const lh_char_t *const *argv, lh_console_sink_cb sink,
                     lh_ptr out);

/**
 * @brief Argument @p index, or an empty string when there is no such argument (level 2).
 *
 * Never ::lh_null, so a handler can compare it without a check first.
 *
 * @param self  The call.
 * @param index Argument index, from 0 for the first one after the command word.
 */
const lh_char_t *
lh_console_call_get_arg(const lh_console_call_t *self, lh_usize_t index);

/**
 * @brief Write one line of reply: @p text and a newline (level 2).
 *
 * Does nothing when the call has no sink.
 *
 * @param self The call.
 * @param text The line, without its newline.
 */
lh_void
lh_console_call_print(const lh_console_call_t *self, const lh_char_t *text);

/**
 * @brief Write one formatted line of reply (level 3).
 *
 * Formats with ::lh_str_format_v into a string of its own, so a line has no length
 * limit; a failed allocation is a runtime failure, as for ::lh_str_t.
 *
 * @param self The call.
 * @param fmt  printf-style format.
 */
lh_void
lh_console_call_printf(const lh_console_call_t *self, const lh_char_t *fmt, ...);

/** @brief ::lh_console_call_printf with a ::va_list (level 3). */
lh_void
lh_console_call_printf_v(const lh_console_call_t *self, const lh_char_t *fmt, va_list args);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CONSOLE_CALL_H */
