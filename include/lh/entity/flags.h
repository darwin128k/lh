/**
 * @file flags.h
 * @brief Bitmask of how an ::lh_entity_t behaves (::lh_entity_add_flags).
 */

#ifndef LH_ENTITY_FLAGS_H
#define LH_ENTITY_FLAGS_H

#include <lh/numeric/types.h>
#include <lh/util/bit.h>

/**
 * @def lh_entity_flags_none
 * @brief No flag set: what a new entity starts with.
 */
#define lh_entity_flags_none 0U

/**
 * @def lh_entity_flags_event_bubble
 * @brief Events reaching this entity continue to its parent afterwards.
 */
#define lh_entity_flags_event_bubble lh_bit_mask(0)

/**
 * @def lh_entity_flags_hidden
 * @brief Neither this entity nor its children are drawn or hit by the
 *        pointer (::lh_entity_rect_find_at).
 */
#define lh_entity_flags_hidden lh_bit_mask(1)

/**
 * @def lh_entity_flags_overflow_visible
 * @brief Children of this rectangle are drawn and hit outside it too; by
 *        default they are cut to it.
 */
#define lh_entity_flags_overflow_visible lh_bit_mask(2)

/**
 * @def lh_entity_flags_user
 * @brief First bit free for the application; the bits below it and the top
 *        bit are lh's.
 */
#define lh_entity_flags_user lh_bit_mask(16)

/**
 * @typedef lh_entity_flags_t
 * @brief Bitmask of `lh_entity_flags_*` values.
 */
typedef lh_uint_t lh_entity_flags_t;

#endif /* LH_ENTITY_FLAGS_H */
