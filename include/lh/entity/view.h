/**
 * @file view.h
 * @brief A clip with a scroll offset. A long page sits inside it.
 *
 * The view cuts its children to its box. ::lh_entity_view_set_content names
 * the child that moves; the offset is that child's position, negated, so
 * the page slides under the clip the way a document does. A scrollbar is a
 * separate entity: its value is what you pass here.
 */

#ifndef LH_ENTITY_VIEW_H
#define LH_ENTITY_VIEW_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>

/**
 * @struct lh_entity_view
 * @brief A window onto a larger child.
 */
struct lh_entity_view
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_entity_t *content;
    lh_int_t x;
    lh_int_t y;
};
typedef struct lh_entity_view lh_entity_view_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_view_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_view_class;

/**
 * @brief The child @p self scrolls. Not owned beyond being a child.
 */
lh_void
lh_entity_view_set_content(lh_entity_view_t *self, lh_entity_t *content);

/**
 * @brief Slide the content so @p x, @p y of it sits at the view's origin.
 */
lh_void
lh_entity_view_set_offset(lh_entity_view_t *self, lh_int_t x, lh_int_t y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_VIEW_H */
