/**
 * @file fields.h
 * @brief Member fields of ::lh_random_t.
 */

#ifndef LH_RANDOM_FIELDS_H
#define LH_RANDOM_FIELDS_H

/**
 * @def lh_random_fields(u32_type)
 * @brief The generator state; read and write it only through the accessors.
 *
 *   - `state` — the xorshift32 state; never 0 after ::lh_random_init.
 *
 * @param u32_type Type of `state` (::lh_u32_t).
 */
#define lh_random_fields(u32_type) u32_type state

#endif /* LH_RANDOM_FIELDS_H */
