/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_memory_range_allocated_t.
 */

#ifndef LH_MEMORY_RANGE_ALLOCATED_INITIALIZER_H
#define LH_MEMORY_RANGE_ALLOCATED_INITIALIZER_H

#include <lh/memory/range/initializer.h>

/**
 * @def lh_memory_range_allocated_initializer()
 * @brief Produces a brace-enclosed “empty” initializer
 *        for ::lh_memory_range_allocated_t.
 *
 * ::lh_memory_range_allocated_t is a typedef of ::lh_memory_range_t;
 * this macro forwards to ::lh_memory_range_empty_initializer
 * (both endpoints null, uninitialized pattern).
 *
 * @see lh_memory_range_allocated_t
 * @see lh_memory_range_empty_initializer
 */
#define lh_memory_range_allocated_initializer() lh_memory_range_empty_initializer()

#endif // LH_MEMORY_RANGE_ALLOCATED_INITIALIZER_H
