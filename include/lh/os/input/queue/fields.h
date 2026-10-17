/**
 * @file fields.h
 * @brief Member fields of ::lh_os_input_queue_t.
 */

#ifndef LH_OS_INPUT_QUEUE_FIELDS_H
#define LH_OS_INPUT_QUEUE_FIELDS_H

/**
 * @def lh_os_input_queue_fields(event_type, index_type)
 * @brief The caller's ring of events and where it is read and written.
 *
 * `storage` holds `capacity` events and is the caller's. `head` (where the
 * consumer reads) and `tail` (where the producer writes) count in
 * `[0, 2 * capacity)`, so a full ring and an empty one differ without a slot
 * kept free and without a count both sides write. `dropped` counts events the
 * producer could not store.
 *
 * @param event_type ::lh_os_input_event_t.
 * @param index_type `volatile` ::lh_u16_t: an interrupt writes one side.
 */
#define lh_os_input_queue_fields(event_type, index_type)                                           \
    event_type *storage;                                                                           \
    lh_u16_t capacity;                                                                             \
    index_type head;                                                                               \
    index_type tail;                                                                               \
    lh_u32_t dropped

#endif /* LH_OS_INPUT_QUEUE_FIELDS_H */
