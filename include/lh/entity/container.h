/**
 * @file container.h
 * @brief A box that holds child entities and does not place them.
 *
 * The children are this entity's children. A flex container is this box
 * plus the layout that moves them.
 */

#ifndef LH_ENTITY_CONTAINER_H
#define LH_ENTITY_CONTAINER_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>

/**
 * @struct lh_entity_container
 * @brief A ::lh_entity_2d_t whose job is the children inside it.
 */
struct lh_entity_container
{
    lh_entity_2d_t box;
};
typedef struct lh_entity_container lh_entity_container_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_container_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_container_class;

/**
 * @brief How many direct children @p self holds.
 */
lh_int_t
lh_entity_container_get_count(const lh_entity_container_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_CONTAINER_H */
