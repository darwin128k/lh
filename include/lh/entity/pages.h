/**
 * @file pages.h
 * @brief One child visible at a time. The others are hidden.
 *
 * The page entities are the children,
 * in order. ::lh_entity_pages_set_index shows one and hides the rest.
 */

#ifndef LH_ENTITY_PAGES_H
#define LH_ENTITY_PAGES_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>

/**
 * @struct lh_entity_pages
 * @brief A stack of children, one of them shown.
 */
struct lh_entity_pages
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_int_t index;
};
typedef struct lh_entity_pages lh_entity_pages_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_pages_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_pages_class;

/**
 * @brief The child that is shown. Others are hidden.
 */
lh_int_t
lh_entity_pages_get_index(const lh_entity_pages_t *self);

/**
 * @brief Show child @p index and hide the others.
 */
lh_void
lh_entity_pages_set_index(lh_entity_pages_t *self, lh_int_t index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_PAGES_H */
