/**
 * @file circle.h
 * @brief An entity that draws a filled disc.
 *
 * It is a 2D entity. The disc is inscribed in the box and uses the style
 * background color. The edge is covered per pixel, so a small disc stays
 * round. A press followed by a release over the same disc sends
 * ::LH_ENTITY_EVENT_CLICKED.
 */

#ifndef LH_ENTITY_CIRCLE_H
#define LH_ENTITY_CIRCLE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>

/**
 * @struct lh_entity_circle
 * @brief A 2D entity that paints a disc.
 */
struct lh_entity_circle
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *);
    lh_bool_t pressed;
};
typedef struct lh_entity_circle lh_entity_circle_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_circle_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_circle_class;

/**
 * @brief True while the pointer is down on @p self.
 */
lh_bool_t
lh_entity_circle_is_pressed(const lh_entity_circle_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_CIRCLE_H */
