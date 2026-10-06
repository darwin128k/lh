/**
 * @file scrollbar.h
 * @brief One scroll axis of a container: ::lh_ui_entity_scrollbar_t.
 *
 * A small separate component: an entity whose rect is the track, an
 * ::lh_ui_axis_t, a show mode, and a pointer to the ::lh_ui_entity_container_t
 * it shows and drives. Link it as a **sibling** of the container (both
 * children of one parent), not as a child, or it scrolls away with the
 * content. One axis per scrollbar; a container with both axes gets two. The
 * container knows nothing about it. The mode
 * (::lh_ui_entity_scrollbar_mode_t, default auto) answers
 * ::lh_ui_entity_event_visible, so ::lh_ui_entity_is_shown decides draw and
 * hit test in the core.
 *
 * Class ::lh_ui_entity_scrollbar_class: draw keeps the base fill (the track,
 * from the entity style), then fills ::lh_ui_entity_scrollbar_get_thumb_rect
 * with the thumb style color and radius. A click off the thumb scrolls one
 * page (the viewport length) toward the click.
 */

#ifndef LH_UI_ENTITY_SCROLLBAR_H
#define LH_UI_ENTITY_SCROLLBAR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/axis.h>
#include <lh/ui/canvas.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/container.h>
#include <lh/ui/entity/scrollbar/fields.h>
#include <lh/ui/entity/scrollbar/mode.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/style.h>
#include <lh/void.h>

/**
 * @def LH_UI_ENTITY_SCROLLBAR_THUMB_MIN
 * @brief Shortest thumb along the track (shorter only when the track is).
 */
#define LH_UI_ENTITY_SCROLLBAR_THUMB_MIN lh_ui_scalar(16)

/**
 * @struct lh_ui_entity_scrollbar
 * @typedef lh_ui_entity_scrollbar_t
 * @brief A track entity, its axis, its show mode, its container, its thumb style.
 */
struct lh_ui_entity_scrollbar
{
    lh_ui_entity_scrollbar_fields(lh_ui_entity_t, lh_ui_axis_t, lh_ui_entity_scrollbar_mode_t,
                                  lh_ui_entity_container_t, lh_ui_style_t);
};
typedef struct lh_ui_entity_scrollbar lh_ui_entity_scrollbar_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Class ───────────────────────────────────────────────────────────────── */

/**
 * @brief Class of ::lh_ui_entity_scrollbar_t, derived from ::lh_ui_entity_class.
 */
extern const lh_ui_entity_class_t lh_ui_entity_scrollbar_class;

/**
 * @brief Event function of ::lh_ui_entity_scrollbar_class: the base class
 *        first, then the draw, click and visible handlers below.
 */
lh_void
lh_ui_entity_scrollbar_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_draw: ::lh_ui_entity_scrollbar_draw_thumb.
 *        Other events are ignored.
 */
lh_void
lh_ui_entity_scrollbar_on_draw(const lh_ui_entity_scrollbar_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_click: ::lh_ui_entity_scrollbar_page_toward.
 *        Other events are ignored.
 */
lh_void
lh_ui_entity_scrollbar_on_click(const lh_ui_entity_scrollbar_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_visible: answer no when
 *        ::lh_ui_entity_scrollbar_is_visible says so. Other events are ignored.
 */
lh_void
lh_ui_entity_scrollbar_on_visible(const lh_ui_entity_scrollbar_t *self, const lh_ui_entity_event_t *event);

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

/**
 * @brief Fill @p self: track @p rect, @p axis, driving @p container (not
 *        owned, must outlive @p self). Mode auto, no style, no thumb style.
 */
lh_void
lh_ui_entity_scrollbar_init(lh_ui_entity_scrollbar_t *self, lh_ui_rect_t rect, lh_ui_axis_t axis,
                            lh_ui_entity_container_t *container);

/**
 * @brief The entity @p self embeds.
 */
lh_ui_entity_t *
lh_ui_entity_scrollbar_as_entity(lh_ui_entity_scrollbar_t *self);

/**
 * @brief The axis @p self shows and drives.
 */
lh_ui_axis_t
lh_ui_entity_scrollbar_get_axis(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief When @p self is shown.
 */
lh_ui_entity_scrollbar_mode_t
lh_ui_entity_scrollbar_get_mode(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief Replace when @p self is shown.
 */
lh_void
lh_ui_entity_scrollbar_set_mode(lh_ui_entity_scrollbar_t *self, lh_ui_entity_scrollbar_mode_t mode);

/**
 * @brief Style the thumb is painted with, or ::lh_null (no thumb painted).
 */
const lh_ui_style_t *
lh_ui_entity_scrollbar_get_thumb_style(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief Point the thumb of @p self at @p style. Not copied; ::lh_null clears.
 */
lh_void
lh_ui_entity_scrollbar_set_thumb_style(lh_ui_entity_scrollbar_t *self, const lh_ui_style_t *style);

/* ── The container on this axis ──────────────────────────────────────────── */

/**
 * @brief Container scroll on the axis of @p self.
 */
lh_ui_scalar_t
lh_ui_entity_scrollbar_get_scroll(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief Container scroll max on the axis of @p self.
 */
lh_ui_scalar_t
lh_ui_entity_scrollbar_get_scroll_max(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief Container viewport length on the axis of @p self: one page.
 */
lh_ui_scalar_t
lh_ui_entity_scrollbar_get_viewport_length(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief Container content length on the axis of @p self.
 */
lh_ui_scalar_t
lh_ui_entity_scrollbar_get_content_length(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief True when the container overflows on the axis of @p self.
 */
lh_bool_t
lh_ui_entity_scrollbar_is_needed(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief The mode's answer: never (hidden), on overflow (auto), always.
 */
lh_bool_t
lh_ui_entity_scrollbar_is_visible(const lh_ui_entity_scrollbar_t *self);

/* ── Thumb ───────────────────────────────────────────────────────────────── */

/**
 * @brief Length of the track (the rect of @p self) on its axis.
 */
lh_ui_scalar_t
lh_ui_entity_scrollbar_get_track_length(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief Thumb length: `track * viewport / content`, at least
 *        ::LH_UI_ENTITY_SCROLLBAR_THUMB_MIN (capped by the track); the whole
 *        track with nothing to scroll.
 */
lh_ui_scalar_t
lh_ui_entity_scrollbar_get_thumb_length(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief Thumb start past the track origin: `(track - length) * scroll / max`;
 *        `0` with nothing to scroll.
 */
lh_ui_scalar_t
lh_ui_entity_scrollbar_get_thumb_start(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief The thumb, in the space of the track rect: start and length on the
 *        axis, the whole track across it.
 */
lh_ui_rect_t
lh_ui_entity_scrollbar_get_thumb_rect(const lh_ui_entity_scrollbar_t *self);

/**
 * @brief Fill the thumb rect on @p canvas with the thumb style color and
 *        radius. Nothing without a canvas, a thumb style or a solid fill.
 */
lh_void
lh_ui_entity_scrollbar_draw_thumb(const lh_ui_entity_scrollbar_t *self, lh_ui_canvas_t *canvas);

/* ── Paging ──────────────────────────────────────────────────────────────── */

/**
 * @brief Scroll step for a click at @p point (track space): minus a page
 *        before the thumb, plus a page after it, `0` on it.
 */
lh_ui_scalar_t
lh_ui_entity_scrollbar_get_page_toward(const lh_ui_entity_scrollbar_t *self, lh_ui_point_t point);

/**
 * @brief Scroll the container by ::lh_ui_entity_scrollbar_get_page_toward.
 */
lh_void
lh_ui_entity_scrollbar_page_toward(const lh_ui_entity_scrollbar_t *self, lh_ui_point_t point);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_SCROLLBAR_H */
