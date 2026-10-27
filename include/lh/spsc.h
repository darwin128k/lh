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

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/numeric/types.h>
#include <lh/void.h>
#include <lh/ptr.h>
typedef struct lh_spsc lh_spsc_t;
LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Create a queue over borrowed fixed-record storage.
 *
 * Returns ::lh_null for invalid dimensions, allocation failure or non-lock-free indices. Captures
 * the caller runtime allocator; its context must outlive the queue.
 */
lh_spsc_t *
lh_spsc_create(lh_ptr storage, lh_uint_t capacity, lh_usize_t item_size);

/**
 * @brief Release queue metadata without releasing borrowed record storage.
 *
 * A null self is ignored. Producer and consumer must have stopped using the queue.
 */
lh_void
lh_spsc_destroy(lh_spsc_t *self);

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
