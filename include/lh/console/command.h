/**
 * @file command.h
 * @brief ::lh_console_command_t — what a command is: its word, its help and its handler.
 *
 * What a caller hands ::lh_console_add. The console copies the text, so the
 * description may be a temporary.
 */

#ifndef LH_CONSOLE_COMMAND_H
#define LH_CONSOLE_COMMAND_H

#include <lh/console/cb.h>

/** @brief A command to register. */
struct lh_console_command
{
    const lh_char_t *name;  /**< the command word (`info`) */
    const lh_char_t *usage; /**< the argument synopsis (`[#userid | name]`), or ::lh_null */
    const lh_char_t *help;  /**< one line of help, or ::lh_null */
    lh_console_cb fn;       /**< the handler */
    lh_ptr user;            /**< the value the handler is given */
};
/** @copybrief lh_console_command */
typedef struct lh_console_command lh_console_command_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The command word (level 1).
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_console_command_get_name(const lh_console_command_t *self);

/**
 * @brief Store the command word (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 * @param name The new value.
 */
lh_void
lh_console_command_set_name(lh_console_command_t *self, const lh_char_t *name);

/**
 * @brief The argument synopsis; ::lh_null for none (level 1).
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_console_command_get_usage(const lh_console_command_t *self);

/**
 * @brief Store the argument synopsis; ::lh_null for none (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 * @param usage The new value.
 */
lh_void
lh_console_command_set_usage(lh_console_command_t *self, const lh_char_t *usage);

/**
 * @brief The line of help; ::lh_null for none (level 1).
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_console_command_get_help(const lh_console_command_t *self);

/**
 * @brief Store the line of help; ::lh_null for none (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 * @param help The new value.
 */
lh_void
lh_console_command_set_help(lh_console_command_t *self, const lh_char_t *help);

/**
 * @brief The handler (level 1).
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 */
lh_console_cb
lh_console_command_get_fn(const lh_console_command_t *self);

/**
 * @brief Store the handler (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 * @param fn The new value.
 */
lh_void
lh_console_command_set_fn(lh_console_command_t *self, lh_console_cb fn);

/**
 * @brief The value the handler is given (level 1).
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_console_command_get_user(const lh_console_command_t *self);

/**
 * @brief Store the value the handler is given (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The command; checked by ::lh_assert_runtime_ref.
 * @param user The new value.
 */
lh_void
lh_console_command_set_user(lh_console_command_t *self, lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CONSOLE_COMMAND_H */
