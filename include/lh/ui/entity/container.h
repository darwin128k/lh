/**
 * @file container.h
 * @brief A scrollable content box in the entity tree: ::lh_ui_entity_container_t.
 *
 * Embeds an ::lh_ui_entity_t. Use ::lh_ui_entity_container_as_entity for rect,
 * style, class, children, and draw. The rect is the viewport; the children
 * are the content, laid out in the same space with scroll `(0, 0)`. Class
 * ::lh_ui_entity_container_class extends ::lh_ui_entity_class, keeps the base
 * fill, and answers ::lh_ui_entity_event_children with offset `-scroll` and a
 * clip, so draw and hit test move and cut the children in the core.
 *
 * Scrollbars are separate components (::lh_ui_entity_scrollbar_t) that point
 * at a container; the container knows nothing about them.
 */

#ifndef LH_UI_ENTITY_CONTAINER_H
#define LH_UI_ENTITY_CONTAINER_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/container/fields.h>
#include <lh/ui/entity/transform.h>
#include <lh/ui/point.h>
#include <lh/ui/size.h>
#include <lh/void.h>

/**
 * @struct lh_ui_entity_container
 * @typedef lh_ui_entity_container_t
 * @brief An entity used as a scrollable content box.
 */
struct lh_ui_entity_container
{
    lh_ui_entity_container_fields(lh_ui_entity_t, lh_ui_point_t);
};
typedef struct lh_ui_entity_container lh_ui_entity_container_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_ui_entity_container_t, derived from ::lh_ui_entity_class.
 */
extern const lh_ui_entity_class_t lh_ui_entity_container_class;

/**
 * @brief Event function of ::lh_ui_entity_container_class.
 *
 * Answers ::lh_ui_entity_event_children through
 * ::lh_ui_entity_container_place_children; every other event goes to the base
 * class (the fill is kept). A derived class may call it directly.
 */
lh_void
lh_ui_entity_container_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief Answer for the children of @p self: offset `-scroll`, clip on.
 */
lh_void
lh_ui_entity_container_place_children(const lh_ui_entity_container_t *self,
                                      lh_ui_entity_transform_t *transform);

/**
 * @brief Fill @p self so it covers @p rect with no style, no children, and
 *        scroll `(0, 0)`.
 */
lh_void
lh_ui_entity_container_init(lh_ui_entity_container_t *self, lh_ui_rect_t rect);

/**
 * @brief The entity @p self embeds.
 */
lh_ui_entity_t *
lh_ui_entity_container_as_entity(lh_ui_entity_container_t *self);

/**
 * @brief Size of the viewport: the size of the rect of @p self.
 */
lh_ui_size_t
lh_ui_entity_container_get_viewport_size(const lh_ui_entity_container_t *self);

/**
 * @brief Far corner of the content (::lh_ui_entity_get_content_bounds), or
 *        the origin of @p self when the content is empty.
 */
lh_ui_point_t
lh_ui_entity_container_get_content_far(const lh_ui_entity_container_t *self);

/**
 * @brief Size of the content: from the container origin to
 *        ::lh_ui_entity_container_get_content_far, never negative.
 *
 * Computed on each call, not stored.
 */
lh_ui_size_t
lh_ui_entity_container_get_content_size(const lh_ui_entity_container_t *self);

/**
 * @brief Largest scroll per axis: content minus viewport, never below `0`.
 */
lh_ui_point_t
lh_ui_entity_container_get_scroll_max(const lh_ui_entity_container_t *self);

/**
 * @brief @p scroll clamped per axis to `0 .. max`
 *        (::lh_ui_entity_container_get_scroll_max): the one scroll clamp.
 */
lh_ui_point_t
lh_ui_entity_container_clamp_scroll(const lh_ui_entity_container_t *self, lh_ui_point_t scroll);

/**
 * @brief How far the content of @p self is scrolled, clamped on read: when the
 *        content shrank since the last set, this is already the new max.
 */
lh_ui_point_t
lh_ui_entity_container_get_scroll(const lh_ui_entity_container_t *self);

/**
 * @brief Scroll @p self to @p scroll, clamped with
 *        ::lh_ui_entity_container_clamp_scroll.
 */
lh_void
lh_ui_entity_container_set_scroll(lh_ui_entity_container_t *self, lh_ui_point_t scroll);

/**
 * @brief Scroll @p self by `(@p dx, @p dy)`, clamped like
 *        ::lh_ui_entity_container_set_scroll.
 */
lh_void
lh_ui_entity_container_scroll_by(lh_ui_entity_container_t *self, lh_ui_scalar_t dx, lh_ui_scalar_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_CONTAINER_H */
