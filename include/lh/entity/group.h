/**
 * @file group.h
 * @brief How a set of options selects: one, or several.
 *
 * A check, a switch and a toggle look at their parent. When that parent
 * is a group in ::LH_ENTITY_GROUP_ONE, turning one on turns the others
 * off. ::LH_ENTITY_GROUP_MANY leaves them independent. A list uses the
 * same two values for its rows.
 */

#ifndef LH_ENTITY_GROUP_H
#define LH_ENTITY_GROUP_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>

/**
 * @def LH_ENTITY_GROUP_ONE
 * @brief At most one child option stays on.
 */
#define LH_ENTITY_GROUP_ONE 0

/**
 * @def LH_ENTITY_GROUP_MANY
 * @brief Each child option is independent.
 */
#define LH_ENTITY_GROUP_MANY 1

/**
 * @struct lh_entity_group
 * @brief A parent that decides how its options select.
 */
struct lh_entity_group
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_int_t mode;
};
typedef struct lh_entity_group lh_entity_group_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_group_t, derived from ::lh_entity_2d_class.
 *
 * A new group is ::LH_ENTITY_GROUP_ONE. It does not paint; give it a size
 * only when it should clip its children.
 */
extern const lh_entity_class_t lh_entity_group_class;

/**
 * @brief ::LH_ENTITY_GROUP_ONE or ::LH_ENTITY_GROUP_MANY.
 */
lh_int_t
lh_entity_group_get_mode(const lh_entity_group_t *self);

/**
 * @brief Set whether one child or several may stay on.
 */
lh_void
lh_entity_group_set_mode(lh_entity_group_t *self, lh_int_t mode);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_GROUP_H */
