/**
 * @file container.h
 * @brief A scrollable content box in the entity tree: ::lh_ui_container_t.
 *
 * Embeds an ::lh_ui_entity_t. Use ::lh_ui_container_as_entity for rect,
 * style, class, children, and draw. The rect is the viewport; the children
 * are the content, laid out in the same space with scroll `(0, 0)`. Class
 * ::lh_ui_container_class extends ::lh_ui_entity_class, keeps the base
 * fill, and answers ::lh_ui_entity_event_children with offset `-scroll` and a
 * clip, so draw and hit test move and cut the children in the core.
 *
 * Scrollbars are separate components (::lh_ui_scrollbar_t) that point
 * at a container; the container knows nothing about them.
 */

#ifndef LH_UI_CONTAINER_H
#define LH_UI_CONTAINER_H

#include <lh/ui/container/fields.h>
#include <lh/ui/layout.h>
#include <lh/ui/key.h>

/**
 * @struct lh_ui_entity_container
 * @brief An entity used as a scrollable content box.
 */
struct lh_ui_entity_container
{
    /** Members: ::lh_ui_container_fields. */
    lh_ui_container_fields(lh_ui_entity_t, lh_ui_point_t, lh_ui_layout_t, lh_bool_t);
};
/** @copybrief lh_ui_entity_container */
typedef struct lh_ui_entity_container lh_ui_container_t;

/**
 * @def LH_UI_CONTAINER_KEY_STEP
 * @brief Scroll step of one arrow key press, in content units.
 */
#define LH_UI_CONTAINER_KEY_STEP lh_ui_scalar(40)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_ui_container_t, derived from ::lh_ui_entity_class.
 */
extern const lh_ui_entity_class_t lh_ui_container_class;

/**
 * @brief Event function of ::lh_ui_container_class.
 *
 * Answers ::lh_ui_entity_event_children through
 * ::lh_ui_container_place_children; every other event goes to the base
 * class (the fill is kept). A derived class may call it directly.
 */
lh_void
lh_ui_container_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief ::lh_ui_entity_event_children: offset `-scroll`, clip on.
 */
lh_void
lh_ui_container_on_children(const lh_ui_container_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief ::lh_ui_entity_event_focusable: yes (a container scrolls by keys).
 */
lh_void
lh_ui_container_on_focusable(const lh_ui_entity_event_t *event);

/**
 * @brief ::lh_ui_entity_event_key: scroll by ::lh_ui_container_get_key_delta.
 */
lh_void
lh_ui_container_on_key(lh_ui_container_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief How far @p key moves the scroll on its axis: arrows
 *        ::LH_UI_CONTAINER_KEY_STEP, page keys one viewport height,
 *        Home / End to the top / bottom; `0` for any other key.
 */
lh_ui_scalar_t
lh_ui_container_get_key_step(const lh_ui_container_t *self, lh_os_key_t key);

/**
 * @brief The scroll change @p input asks for: left / right across, every
 *        other key along y; zero for text and key-up.
 */
lh_void
lh_ui_container_get_key_delta(const lh_ui_container_t *self, const lh_ui_key_input_t *input,
                              lh_ui_point_t *out);

/**
 * @brief Answer for the children of @p self: place them by
 *        ::lh_ui_container_get_layout, then offset `-scroll`, clip on.
 *
 * The placement is the container's own and happens here, so a container that
 * moved carries its children with it — the rects a flow writes are absolute, and
 * nothing else in the tree is in a position to rewrite them.
 */
lh_void
lh_ui_container_place_children(const lh_ui_container_t *self, lh_ui_entity_transform_t *transform);

/**
 * @brief The flow @p self places its children by, or ::lh_null when it has none
 *        and its children keep the rects they were given.
 */
const lh_ui_layout_t *
lh_ui_container_get_layout(const lh_ui_container_t *self);

/**
 * @brief Place the children of @p self by @p layout from now on: axis, gap and
 *        where the leftovers go. Not owned — the rule belongs to the caller, like
 *        a style, and ::lh_null takes it away again.
 */
lh_void
lh_ui_container_set_layout(lh_ui_container_t *self, const lh_ui_layout_t *layout);

/**
 * @brief Fill @p self so it covers @p rect with no style, no children, and
 *        scroll `(0, 0)`.
 */
lh_void
lh_ui_container_init(lh_ui_container_t *self, lh_ui_rect_t rect);

/**
 * @brief The entity @p self embeds.
 */
lh_ui_entity_t *
lh_ui_container_as_entity(lh_ui_container_t *self);

/**
 * @brief @p entity as a container when its class is
 *        ::lh_ui_container_class or extends it (e.g. a label), else
 *        ::lh_null. The entity must be the first field of the container.
 */
lh_ui_container_t *
lh_ui_entity_as_container(lh_ui_entity_t *entity);

/**
 * @brief Nearest container on @p entity or an ancestor, or ::lh_null.
 */
lh_ui_container_t *
lh_ui_entity_find_container(lh_ui_entity_t *entity);

/**
 * @brief Size of the viewport: the size of the rect of @p self.
 */
lh_void
lh_ui_container_get_viewport_size(const lh_ui_container_t *self, lh_ui_size_t *out);

/**
 * @brief @p far moved out by the right and bottom padding of @p self: the room
 *        left after the last child, as CSS scroll size counts it.
 */
lh_void
lh_ui_container_pad_far(const lh_ui_container_t *self, lh_ui_point_t far, lh_ui_point_t *out);

/**
 * @brief Far corner of the content (::lh_ui_entity_get_content_bounds) plus
 *        the padding, or the origin of @p self when the content is empty.
 */
lh_void
lh_ui_container_get_content_far(const lh_ui_container_t *self, lh_ui_point_t *out);

/**
 * @brief Size of the content: from the container origin to
 *        ::lh_ui_container_get_content_far, never negative.
 *
 * Computed on each call, not stored.
 */
lh_void
lh_ui_container_get_content_size(const lh_ui_container_t *self, lh_ui_size_t *out);

/**
 * @brief Largest scroll per axis: content minus viewport, never below `0`.
 */
lh_void
lh_ui_container_get_scroll_max(const lh_ui_container_t *self, lh_ui_point_t *out);

/**
 * @brief @p scroll clamped per axis to `0 .. @p max`: the one scroll clamp.
 *        For a caller that already holds the max, so content is not measured
 *        again.
 */
lh_void
lh_ui_container_clamp_scroll_to(lh_ui_point_t scroll, lh_ui_point_t max, lh_ui_point_t *out);

/**
 * @brief @p scroll clamped per axis to `0 .. max`
 *        (::lh_ui_container_get_scroll_max).
 */
lh_void
lh_ui_container_clamp_scroll(const lh_ui_container_t *self, lh_ui_point_t scroll,
                             lh_ui_point_t *out);

/**
 * @brief The stored scroll of @p self clamped to @p max, which the caller got
 *        from ::lh_ui_container_get_scroll_max (no second measure).
 */
lh_void
lh_ui_container_get_scroll_within(const lh_ui_container_t *self, lh_ui_point_t max,
                                  lh_ui_point_t *out);

/**
 * @brief How far the content of @p self is scrolled, clamped on read: when the
 *        content shrank since the last set, this is already the new max.
 */
lh_void
lh_ui_container_get_clamped_scroll(const lh_ui_container_t *self, lh_ui_point_t *out);

/**
 * @brief Scroll @p self to @p scroll, clamped with
 *        ::lh_ui_container_clamp_scroll.
 */
lh_void
lh_ui_container_set_scroll_v(lh_ui_container_t *self, lh_ui_point_t scroll);

/**
 * @brief Scroll @p self by `(@p dx, @p dy)`, clamped like
 *        ::lh_ui_container_set_scroll_v.
 */
lh_void
lh_ui_container_scroll_by(lh_ui_container_t *self, lh_ui_scalar_t dx, lh_ui_scalar_t dy);

/**
 * @brief The entity the container is built on (level 1).
 *
 * @param self The container; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_entity_t *
lh_ui_container_get_entity(const lh_ui_container_t *self);

/**
 * @brief The entity the container is built on, to change in place (level 1).
 *
 * @param self The container; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_entity_t *
lh_ui_container_get_entity_mut(lh_ui_container_t *self);

/**
 * @brief Store the entity the container is built on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The container; checked by ::lh_assert_runtime_ref.
 * @param entity The new value.
 */
lh_void
lh_ui_container_set_entity(lh_ui_container_t *self, lh_ui_entity_t entity);

/**
 * @brief How far the content is scrolled, to change in place (level 1).
 *
 * @param self The container; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_point_t *
lh_ui_container_get_scroll_mut(lh_ui_container_t *self);

/**
 * @brief Whether the container is placing its children now (level 1).
 *
 * @param self The container; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_container_is_placing(const lh_ui_container_t *self);

/**
 * @brief Store whether the container is placing its children now (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The container; checked by ::lh_assert_runtime_ref.
 * @param placing The new value.
 */
lh_void
lh_ui_container_set_placing(lh_ui_container_t *self, lh_bool_t placing);

/**
 * @brief How far the content is scrolled (level 1).
 *
 * @param self The container; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_point_t *
lh_ui_container_get_scroll(const lh_ui_container_t *self);

/**
 * @brief Store how far the content is scrolled (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The container; checked by ::lh_assert_runtime_ref.
 * @param scroll The new value.
 */
lh_void
lh_ui_container_set_scroll(lh_ui_container_t *self, lh_ui_point_t scroll);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CONTAINER_H */
