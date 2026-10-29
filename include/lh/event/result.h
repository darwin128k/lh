/**
 * @file result.h
 * @brief Ordered synchronous dispatch results.
 *
 * Ordered synchronous dispatch results.
 */

#ifndef LH_EVENT_RESULT_H
#define LH_EVENT_RESULT_H

/** @brief Continue, acknowledge, or stop delivery to later subscribers. */
enum lh_event_result
{
    lh_event_result_continue = 0,
    lh_event_result_handled = 1,
    lh_event_result_stop = 2
};
/** @copybrief lh_event_result */
typedef enum lh_event_result lh_event_result_t;

#endif /* LH_EVENT_RESULT_H */
