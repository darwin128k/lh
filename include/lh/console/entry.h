/**
 * @file entry.h
 * @brief ::lh_console_entry_t — one registered command, owned by the console.
 *
 * The console's copy of a command: its scope, word, usage and help as owned text,
 * the handler and its value, and who registered it. A removed entry is marked dead
 * and freed once no command is running.
 */

#ifndef LH_CONSOLE_ENTRY_H
#define LH_CONSOLE_ENTRY_H

#include <lh/console/command.h>
#include <lh/list/node.h>
#include <lh/str.h>

/** @brief One registered command. */
struct lh_console_entry
{
    lh_list_node_t node; /**< the link in the console's list */
    lh_ptr owner;        /**< who registered it; compared by identity */
    lh_str_t scope;      /**< the scope it answers to */
    lh_str_t name;       /**< the command word */
    lh_str_t usage;      /**< the argument synopsis; empty for none */
    lh_str_t help;       /**< one line of help; empty for none */
    lh_console_cb fn;    /**< the handler */
    lh_ptr user;         /**< the value the handler is given */
    lh_bool_t dead;      /**< removed; freed when no command runs */
};
/** @copybrief lh_console_entry */
typedef struct lh_console_entry lh_console_entry_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The link in the console's list (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
const lh_list_node_t *
lh_console_entry_get_node(const lh_console_entry_t *self);

/**
 * @brief The link in the console's list, to change in place (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_list_node_t *
lh_console_entry_get_node_mut(lh_console_entry_t *self);

/**
 * @brief Who registered the command (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_console_entry_get_owner(const lh_console_entry_t *self);

/**
 * @brief Store who registered the command (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @param owner The new value.
 */
lh_void
lh_console_entry_set_owner(lh_console_entry_t *self, lh_ptr owner);

/**
 * @brief The scope the command answers to (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
const lh_str_t *
lh_console_entry_get_scope(const lh_console_entry_t *self);

/**
 * @brief The scope the command answers to, to change in place (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_str_t *
lh_console_entry_get_scope_mut(lh_console_entry_t *self);

/**
 * @brief The command word (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
const lh_str_t *
lh_console_entry_get_name(const lh_console_entry_t *self);

/**
 * @brief The command word, to change in place (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_str_t *
lh_console_entry_get_name_mut(lh_console_entry_t *self);

/**
 * @brief The argument synopsis; empty for none (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
const lh_str_t *
lh_console_entry_get_usage(const lh_console_entry_t *self);

/**
 * @brief The argument synopsis; empty for none, to change in place (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_str_t *
lh_console_entry_get_usage_mut(lh_console_entry_t *self);

/**
 * @brief The line of help; empty for none (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
const lh_str_t *
lh_console_entry_get_help(const lh_console_entry_t *self);

/**
 * @brief The line of help; empty for none, to change in place (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_str_t *
lh_console_entry_get_help_mut(lh_console_entry_t *self);

/**
 * @brief The handler (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
lh_console_cb
lh_console_entry_get_fn(const lh_console_entry_t *self);

/**
 * @brief Store the handler (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @param fn The new value.
 */
lh_void
lh_console_entry_set_fn(lh_console_entry_t *self, lh_console_cb fn);

/**
 * @brief The value the handler is given (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_console_entry_get_user(const lh_console_entry_t *self);

/**
 * @brief Store the value the handler is given (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @param user The new value.
 */
lh_void
lh_console_entry_set_user(lh_console_entry_t *self, lh_ptr user);

/**
 * @brief Whether the command was removed (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_console_entry_is_dead(const lh_console_entry_t *self);

/**
 * @brief Store whether the command was removed (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @param dead The new value.
 */
lh_void
lh_console_entry_set_dead(lh_console_entry_t *self, lh_bool_t dead);

/**
 * @brief Allocate an entry for @p command, registered by @p owner under @p scope (level 2).
 *
 * Copies the scope, the word, the usage and the help; a ::lh_null usage or help is
 * empty. The entry starts unlinked and live.
 *
 * @param owner   Who registers it; compared by identity, never dereferenced.
 * @param scope   The scope word.
 * @param command The command; its text is copied.
 * @return The entry, or ::lh_null when it could not be allocated. A failed copy of
 *         the text is a runtime failure, as for ::lh_str_t.
 */
lh_console_entry_t *
lh_console_entry_try_create(lh_ptr owner, const lh_char_t *scope,
                            const lh_console_command_t *command);

/**
 * @brief Free the text and the entry (level 2).
 *
 * The entry must be unlinked first.
 *
 * @param self The entry, from ::lh_console_entry_try_create.
 */
lh_void
lh_console_entry_destroy(lh_console_entry_t *self);

/**
 * @brief Whether @p self answers to @p scope and @p name, without case (level 2).
 *
 * @param self  The entry.
 * @param scope The scope word.
 * @param name  The command word, or ::lh_null to match any word in @p scope.
 */
lh_bool_t
lh_console_entry_is_named(const lh_console_entry_t *self, const lh_char_t *scope,
                          const lh_char_t *name);

/**
 * @brief Whether @p self was registered by @p owner with this handler and value (level 2).
 *
 * @param self    The entry.
 * @param owner   Who registered it.
 * @param command The command it would be.
 */
lh_bool_t
lh_console_entry_is_same(const lh_console_entry_t *self, lh_ptr owner,
                         const lh_console_command_t *command);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CONSOLE_ENTRY_H */
