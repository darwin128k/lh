/**
 * @file result.h
 * @brief ::lh_console_result_t — how running a command ended.
 */

#ifndef LH_CONSOLE_RESULT_H
#define LH_CONSOLE_RESULT_H

/** @brief How ::lh_console_run ended. */
enum lh_console_result
{
    /** No live command has that scope and name. */
    lh_console_result_not_found = 0,
    /** The command ran and accepted its arguments. */
    lh_console_result_done = 1,
    /** The command ran and refused its arguments: the caller shows its usage. */
    lh_console_result_bad_usage = 2
};
/** @copybrief lh_console_result */
typedef enum lh_console_result lh_console_result_t;

#endif /* LH_CONSOLE_RESULT_H */
