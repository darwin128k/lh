/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_entity_transform_t.
 */

#ifndef LH_UI_ENTITY_TRANSFORM_INITIALIZER_H
#define LH_UI_ENTITY_TRANSFORM_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_entity_transform_initializer(offset, clip)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_entity_transform_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_entity_transform_fields.
 *
 * @param offset Initializer of the ::lh_ui_point_t the children move by.
 * @param clip   Whether the children are cut to the rect of the entity.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_entity_transform_t transform = lh_ui_entity_transform_initializer(
 *     lh_initializer(0, 0),  // offset
 *     lh_bool_true);         // clip
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_entity_transform_t
 */
#define lh_ui_entity_transform_initializer(offset, clip) lh_initializer(offset, clip)

#endif /* LH_UI_ENTITY_TRANSFORM_INITIALIZER_H */
