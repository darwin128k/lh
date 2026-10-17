/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_os_input_queue_t.
 */

#ifndef LH_OS_INPUT_QUEUE_INITIALIZER_H
#define LH_OS_INPUT_QUEUE_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_os_input_queue_initializer(storage, capacity, head, tail, dropped)
 * @brief Produces a brace-enclosed initializer for ::lh_os_input_queue_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_os_input_queue_fields.
 *
 * @param storage  The caller's array of @p capacity events.
 * @param capacity How many events @p storage holds (1..32767).
 * @param head     Read position, `0` for an empty queue.
 * @param tail     Write position, `0` for an empty queue.
 * @param dropped  Events lost so far, `0`.
 *
 * Example usage:
 * @code{.c}
 * static lh_os_input_event_t events[16];
 * static lh_os_input_queue_t queue = lh_os_input_queue_initializer(events,  // storage
 *                                                                  16U,     // capacity
 *                                                                  0U,      // head
 *                                                                  0U,      // tail
 *                                                                  0U);     // dropped
 * @endcode
 *
 * @see lh_initializer
 * @see lh_os_input_queue_t
 */
#define lh_os_input_queue_initializer(storage, capacity, head, tail, dropped)                      \
    lh_initializer(storage, capacity, head, tail, dropped)

/**
 * @def lh_os_input_queue_empty_initializer(storage, capacity)
 * @brief Initializer of an empty queue over @p storage of @p capacity events,
 *        the same state ::lh_os_input_queue_init leaves.
 *
 * Example usage:
 * @code{.c}
 * static lh_os_input_event_t events[16];
 * static lh_os_input_queue_t queue = lh_os_input_queue_empty_initializer(events, 16U);
 * @endcode
 *
 * @see lh_os_input_queue_initializer
 */
#define lh_os_input_queue_empty_initializer(storage, capacity)                                     \
    lh_os_input_queue_initializer(storage, capacity, 0U, 0U, 0U)

#endif /* LH_OS_INPUT_QUEUE_INITIALIZER_H */
