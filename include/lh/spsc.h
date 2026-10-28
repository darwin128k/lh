/**
 * @file spsc.h
 * @brief Bounded single-producer/single-consumer fixed-record queue.
 *
 * Create/destroy require exclusive access. Storage is borrowed, must hold
 * capacity * item_size bytes and outlive the queue. Push copies without waiting
 * or allocation; overflow rejects the newest item. Only the producer calls push.
 * Only the consumer calls peek/release; a peek pointer remains valid until release.
 * Release requires a successful peek. Capacity must be a power of two, at most
 * LH_UINT_T_MAX / 2; item_size must be nonzero. Create fails if indices are not lock-free.
 */

#ifndef LH_SPSC_H
#define LH_SPSC_H

#include <lh/memory/allocator.h>

typedef struct lh_spsc lh_spsc_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The allocator copy that owns the queue's metadata.
 *
 * Level-1 accessor: captured from the runtime allocator by ::lh_spsc_create and
 * used by ::lh_spsc_destroy.
 */
lh_memory_allocator_t *
lh_spsc_get_allocator_mut(lh_spsc_t *self);

/** @brief The borrowed record storage: capacity * item_size bytes. */
lh_byte_t *
lh_spsc_get_storage(const lh_spsc_t *self);

/** @brief Point the queue at @p storage. Level-1 setter: no validation. */
lh_void
lh_spsc_set_storage(lh_spsc_t *self, lh_byte_t *storage);

/** @brief Number of record slots; a power of two. */
lh_uint_t
lh_spsc_get_capacity(const lh_spsc_t *self);

/** @brief Store the slot count. Level-1 setter: no validation. */
lh_void
lh_spsc_set_capacity(lh_spsc_t *self, lh_uint_t capacity);

/** @brief Bytes of one record. */
lh_usize_t
lh_spsc_get_item_size(const lh_spsc_t *self);

/** @brief Store the record size. Level-1 setter: no validation. */
lh_void
lh_spsc_set_item_size(lh_spsc_t *self, lh_usize_t item_size);

/**
 * @brief Records pushed so far (wraps), read with acquire order.
 *
 * Acquire pairs with the release in ::lh_spsc_set_written: a consumer that sees
 * the count also sees the record bytes written before it.
 */
lh_uint_t
lh_spsc_get_written(lh_spsc_t *self);

/** @brief Publish the pushed count with release order. Only the producer calls it. */
lh_void
lh_spsc_set_written(lh_spsc_t *self, lh_uint_t written);

/**
 * @brief Records released so far (wraps), read with acquire order.
 *
 * Acquire pairs with the release in ::lh_spsc_set_read: a producer that sees a
 * slot released may overwrite it.
 */
lh_uint_t
lh_spsc_get_read(lh_spsc_t *self);

/** @brief Publish the released count with release order. Only the consumer calls it. */
lh_void
lh_spsc_set_read(lh_spsc_t *self, lh_uint_t read);

/** @brief Start the pushed count at zero; before the queue is shared. */
lh_void
lh_spsc_init_written(lh_spsc_t *self);

/** @brief Start the released count at zero; before the queue is shared. */
lh_void
lh_spsc_init_read(lh_spsc_t *self);

/** @brief Whether the pushed count is a lock-free atomic on this target. */
lh_bool_t
lh_spsc_is_written_lock_free(const lh_spsc_t *self);

/** @brief Whether the released count is a lock-free atomic on this target. */
lh_bool_t
lh_spsc_is_read_lock_free(const lh_spsc_t *self);

/**
 * @brief Whether @p storage, @p capacity and @p item_size can form a queue.
 *
 * Storage non-null; capacity a non-zero power of two up to LH_UINT_T_MAX / 2;
 * item_size non-zero with capacity * item_size within ::lh_usize_t.
 */
lh_bool_t
lh_spsc_is_valid_shape(lh_ptr storage, lh_uint_t capacity, lh_usize_t item_size);

/** @brief Start both counters at zero; before the queue is shared. */
lh_void
lh_spsc_init_counters(lh_spsc_t *self);

/** @brief Whether both counters are lock-free atomics on this target. */
lh_bool_t
lh_spsc_is_lock_free(const lh_spsc_t *self);

/**
 * @brief Address of the slot that counter value @p position maps to.
 *
 * `storage + (position mod capacity) * item_size`; the one formula push and peek
 * share.
 */
lh_byte_t *
lh_spsc_get_slot(const lh_spsc_t *self, lh_uint_t position);

/**
 * @brief Release queue metadata without releasing borrowed record storage.
 *
 * A null self is ignored. Producer and consumer must have stopped using the queue.
 */
lh_void
lh_spsc_destroy(lh_spsc_t *self);

/**
 * @brief Create a queue over borrowed fixed-record storage.
 *
 * Returns ::lh_null for invalid dimensions, allocation failure or non-lock-free indices. Captures
 * the caller runtime allocator; its context must outlive the queue.
 */
lh_spsc_t *
lh_spsc_create(lh_ptr storage, lh_uint_t capacity, lh_usize_t item_size);

/**
 * @brief Copy a record into the next available slot.
 *
 * self and item must be non-null. Only the producer calls this function. Returns ::lh_bool_false
 * when full; no allocation or waiting occurs.
 */
lh_bool_t
lh_spsc_push(lh_spsc_t *self, const lh_void *item);

/**
 * @brief Borrow the oldest record without removing it.
 *
 * self must be non-null. Only the consumer calls this function. Returns ::lh_null when empty; the
 * pointer remains valid until release.
 */
const lh_void *
lh_spsc_peek(lh_spsc_t *self);

/**
 * @brief Release the record previously returned by peek.
 *
 * self must be non-null and a record must be available. Only the consumer calls this function; the
 * returned slot becomes writable by the producer.
 */
lh_void
lh_spsc_release(lh_spsc_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SPSC_H */
