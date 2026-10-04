/**
 * @file knob.h
 * @brief A dial. The same value as a trackbar, turned by the pointer.
 *
 * Create it with ::lh_entity_knob_class. The record starts with an
 * ::lh_entity_range_t. The track is a rounded ring in the style background.
 * The swept part of that ring, and a round handle, are the text color.
 * A change sends ::LH_ENTITY_EVENT_CLICKED.
 */

#ifndef LH_ENTITY_KNOB_H
#define LH_ENTITY_KNOB_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/range.h>

/**
 * @struct lh_entity_knob
 * @brief A dial. The first member is the range.
 */
struct lh_entity_knob
{
    lh_entity_range_t range;
    lh_bool_t dragging;
};
typedef struct lh_entity_knob lh_entity_knob_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_knob_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_knob_class;

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_KNOB_H */
