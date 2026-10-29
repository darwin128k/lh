/**
 * @file console.h
 * @brief ::lh_console_t — a registry of console commands, scoped and owned.
 *
 * A command answers to a scope and a word (`qproto info`): the scope is usually
 * the name of whoever registered it. Each command remembers its owner, so a
 * module that goes away takes its commands with it (::lh_console_drop_owner).
 * Words are compared without case. A handler may add and remove commands, its
 * own included: a removed command stops answering at once and is freed when the
 * outermost run returns.
 *
 * The console knows nothing of where a line comes from or where the reply goes
 * (::lh_console_sink_fn), nor of how long a word may be or how help is laid
 * out: those are the caller's. Calls are not synchronized; use it from one
 * thread.
 */

#ifndef LH_CONSOLE_H
#define LH_CONSOLE_H

#include <lh/console/entry.h>
#include <lh/console/result.h>
#include <lh/list.h>

/** @brief A registry of commands. */
struct lh_console
{
    lh_list_t entries; /**< the registered commands, in the order they were added */
    lh_usize_t depth;  /**< how many runs are in progress, nested */
};
/** @copybrief lh_console */
typedef struct lh_console lh_console_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The registered commands, in the order they were added (level 1).
 *
 * @param self The console; checked by ::lh_assert_runtime_ref.
 */
const lh_list_t *
lh_console_get_entries(const lh_console_t *self);

/**
 * @brief The registered commands, in the order they were added, to change in place (level 1).
 *
 * @param self The console; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_list_t *
lh_console_get_entries_mut(lh_console_t *self);

/**
 * @brief How many runs are in progress, nested (level 1).
 *
 * @param self The console; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_console_get_depth(const lh_console_t *self);

/**
 * @brief Store how many runs are in progress, nested (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The console; checked by ::lh_assert_runtime_ref.
 * @param depth The new value.
 */
lh_void
lh_console_set_depth(lh_console_t *self, lh_usize_t depth);

/**
 * @brief Whether @p text can be a scope or a command word (level 1).
 *
 * Letters, digits, `_`, `-` and `.`, at least one of them. How long a word may be is
 * the caller's to decide.
 *
 * @param text NUL-terminated text, or ::lh_null (which is not a word).
 */
lh_bool_t
lh_console_is_word(const lh_char_t *text);

/**
 * @brief Start an empty console; allocates nothing (level 2).
 *
 * @param self The console; must not be ::lh_null (not checked).
 */
lh_void
lh_console_init(lh_console_t *self);

/**
 * @brief The first live command, in the order they were added (level 2).
 *
 * For listing, with ::lh_console_get_next. A command removed while the walk runs
 * stays readable until the outermost run returns.
 *
 * @param self The console; must not be ::lh_null (not checked).
 * @return The entry, or ::lh_null when there are none.
 */
const lh_console_entry_t *
lh_console_get_first(const lh_console_t *self);

/**
 * @brief The live command after @p entry, or ::lh_null at the end (level 2).
 *
 * @param self The console; must not be ::lh_null (not checked).
 * @param entry A command of this console.
 */
const lh_console_entry_t *
lh_console_get_next(const lh_console_t *self, const lh_console_entry_t *entry);

/**
 * @brief Free the removed commands, unless a command is running (level 3).
 *
 * @param self The console; must not be ::lh_null (not checked).
 */
lh_void
lh_console_collect(lh_console_t *self);

/**
 * @brief The live command answering to @p scope and @p name, without case (level 3).
 *
 * @param self The console; must not be ::lh_null (not checked).
 * @param scope The scope word.
 * @param name  The command word.
 * @return The entry, or ::lh_null.
 */
const lh_console_entry_t *
lh_console_find(const lh_console_t *self, const lh_char_t *scope, const lh_char_t *name);

/**
 * @brief Whether any live command answers to @p scope (level 3).
 *
 * @param self The console; must not be ::lh_null (not checked).
 * @param scope The scope word.
 */
lh_bool_t
lh_console_has_scope(const lh_console_t *self, const lh_char_t *scope);

/**
 * @brief How many live commands there are (level 3).
 *
 * @param self The console; must not be ::lh_null (not checked).
 */
lh_usize_t
lh_console_get_count(const lh_console_t *self);

/**
 * @brief Remove every command; nothing answers afterwards (level 4).
 *
 * Commands removed while one runs are freed when the outermost run returns.
 *
 * @param self The console; must not be ::lh_null (not checked).
 */
lh_void
lh_console_clear(lh_console_t *self);

/**
 * @brief Remove every command @p owner registered (level 4).
 *
 * @param self The console; must not be ::lh_null (not checked).
 * @param owner Who registered them; compared by identity.
 */
lh_void
lh_console_drop_owner(lh_console_t *self, lh_ptr owner);

/**
 * @brief Register @p command under @p scope for @p owner (level 4).
 *
 * The same owner, handler and value under the same scope and word is registered
 * once: asking again succeeds and adds nothing. Another command already answering
 * to that scope and word wins, and this one is refused.
 *
 * @param self The console; must not be ::lh_null (not checked).
 * @param owner   Who registers it.
 * @param scope   The scope word (::lh_console_is_word).
 * @param command The command; its word must be a word and its handler set.
 * @return True when the command answers now.
 */
lh_bool_t
lh_console_add(lh_console_t *self, lh_ptr owner, const lh_char_t *scope,
               const lh_console_command_t *command);

/**
 * @brief Remove the command @p owner registered under @p scope and @p name (level 4).
 *
 * It stops answering at once. Removing one that another owner registered fails.
 *
 * @param self The console; must not be ::lh_null (not checked).
 * @param owner Who registered it.
 * @param scope The scope word.
 * @param name  The command word.
 * @return True when there was one to remove.
 */
lh_bool_t
lh_console_remove(lh_console_t *self, lh_ptr owner, const lh_char_t *scope, const lh_char_t *name);

/**
 * @brief Run the command answering to @p scope and @p name (level 4).
 *
 * The handler gets an ::lh_console_call_t over the arguments and the sink. It may
 * add and remove commands, its own included.
 *
 * @param self The console; must not be ::lh_null (not checked).
 * @param scope The scope word.
 * @param name  The command word.
 * @param argc  How many arguments follow it.
 * @param argv  The arguments, or ::lh_null for none.
 * @param sink  Where the reply goes, or ::lh_null to drop it.
 * @param out   The value the sink is given.
 * @param ran   Where to store the command that ran, for showing its usage; may be
 *              ::lh_null. Valid until the next call that can remove commands.
 * @return Whether it was found, and whether it accepted its arguments.
 */
lh_console_result_t
lh_console_run(lh_console_t *self, const lh_char_t *scope, const lh_char_t *name, lh_usize_t argc,
               const lh_char_t *const *argv, lh_console_sink_cb sink, lh_ptr out,
               const lh_console_entry_t **ran);

/**
 * @brief Remove and free every command (level 5).
 *
 * No command may be running.
 *
 * @param self The console; must not be ::lh_null (not checked).
 */
lh_void
lh_console_deinit(lh_console_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CONSOLE_H */
