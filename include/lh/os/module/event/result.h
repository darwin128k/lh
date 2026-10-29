/**
 * @file result.h
 * @brief Ordered synchronous dispatch results.
 *
 * A publication returns the strongest result its handlers gave; stop also ends the
 * chain.
 */

#ifndef LH_OS_MODULE_EVENT_RESULT_H
#define LH_OS_MODULE_EVENT_RESULT_H

/** @brief Continue, acknowledge, or stop delivery to later subscribers. */
enum lh_os_module_event_result
{
    lh_os_module_event_result_continue = 0,
    lh_os_module_event_result_handled = 1,
    lh_os_module_event_result_stop = 2
};
/** @copybrief lh_os_module_event_result */
typedef enum lh_os_module_event_result lh_os_module_event_result_t;

#endif /* LH_OS_MODULE_EVENT_RESULT_H */
