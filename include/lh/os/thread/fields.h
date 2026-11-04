/**
 * @file fields.h
 * @brief Member fields of ::lh_os_thread_t.
 */

#ifndef LH_OS_THREAD_FIELDS_H
#define LH_OS_THREAD_FIELDS_H

/**
 * @def lh_os_thread_fields(handle_type, allocator_type)
 * @brief Native resource handle owned by the object, and the allocator its
 *        native storage came from.
 *
 * The handle is the address of that storage; the allocator is a copy of the
 * runtime allocator taken when the native object was created, so the storage
 * goes back to the heap it came from even if the runtime allocator changes.
 *
 * @param handle_type    Native handle type (::lh_os_system_thread_handle_t).
 * @param allocator_type ::lh_memory_allocator_t.
 */
#define lh_os_thread_fields(handle_type, allocator_type)                                           \
    handle_type handle;                                                                            \
    allocator_type allocator

#endif /* LH_OS_THREAD_FIELDS_H */
