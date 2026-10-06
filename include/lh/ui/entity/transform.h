/**
 * @file transform.h
 * @brief How an entity places its children: ::lh_ui_entity_transform_t.
 *
 * The answer to ::lh_ui_entity_event_children. ::lh_ui_entity_get_children_transform
 * fills it with the defaults (no offset, no clip), sends the event, and both
 * ::lh_ui_entity_draw and ::lh_ui_entity_find_at use the result. A class that
 * moves or cuts its children (a scrolling container) sets the fields.
 */

#ifndef LH_UI_ENTITY_TRANSFORM_H
#define LH_UI_ENTITY_TRANSFORM_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/entity/transform/fields.h>
#include <lh/ui/point.h>
#include <lh/void.h>

/**
 * @struct lh_ui_entity_transform
 * @typedef lh_ui_entity_transform_t
 * @brief Offset added to the children, and the clip flag.
 */
struct lh_ui_entity_transform
{
    lh_ui_entity_transform_fields(lh_ui_point_t, lh_bool_t);
};
typedef struct lh_ui_entity_transform lh_ui_entity_transform_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the defaults: offset `(0, 0)`, no clip.
 */
lh_void
lh_ui_entity_transform_init(lh_ui_entity_transform_t *self);

/**
 * @brief Offset added to every child rect when drawn; subtracted from the
 *        point when the children are hit-tested.
 */
lh_ui_point_t
lh_ui_entity_transform_get_offset(const lh_ui_entity_transform_t *self);

/**
 * @brief Replace the offset of @p self.
 */
lh_void
lh_ui_entity_transform_set_offset(lh_ui_entity_transform_t *self, lh_ui_point_t offset);

/**
 * @brief True when the children are cut to the entity's rect.
 */
lh_bool_t
lh_ui_entity_transform_is_clip(const lh_ui_entity_transform_t *self);

/**
 * @brief Cut the children to the entity's rect, or not.
 */
lh_void
lh_ui_entity_transform_set_clip(lh_ui_entity_transform_t *self, lh_bool_t clip);

/**
 * @brief True when @p self changes nothing: offset `(0, 0)` and no clip.
 */
lh_bool_t
lh_ui_entity_transform_is_identity(const lh_ui_entity_transform_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_TRANSFORM_H */
