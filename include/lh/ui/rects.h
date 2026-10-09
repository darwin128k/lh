/**
 * @file rects.h
 * @brief UI rects: ::lh_ui_rects_t, a fixed list of rects that stay a region.
 *
 * One rect is the wrong shape for a region. `lh_ui_rect_union` answers with the
 * hull of two, and the hull of two cuts on the same strip is the rectangle
 * spanning the space between them: a stretch of pixels that belongs to neither
 * cut. A backend that presents its drawn region as one rect therefore presents
 * that gap too, and the bytes it holds are whatever a DIB it just reallocated
 * gave back.
 *
 * This is the shape that does not have that problem: rects that touch are
 * merged, rects that do not are kept apart, and the result is read one rect at
 * a time. The table is fixed and never reallocated (::LH_UI_RECTS_MAX), because
 * a drawing path that allocates is a drawing path that cannot be stepped
 * through.
 *
 * @see `lh_ui_rect_t`, `lh_ui_canvas_clip_t` (one cut),
 *      `lh_os_render_backend_gdi_end` (the present that wanted this).
 */

#ifndef LH_UI_RECTS_H
#define LH_UI_RECTS_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

/**
 * @def LH_UI_RECTS_MAX
 * @brief How many rects ::lh_ui_rects_t keeps apart at most.
 *
 * `8`, which is more than one strip of any scene so far holds: rects that
 * touch collapse into one, so what a strip really needs is the number of
 * widgets standing apart in it, not the number drawn.
 *
 * @note Past this the list stops being the region exactly: the added rect is
 *       merged into whichever stored one the pair wastes the fewest pixels on
 *       (::lh_ui_rects_squeeze). It never drops a rect.
 */
#define LH_UI_RECTS_MAX 8

#include <lh/ui/rects/fields.h>

/**
 * @struct lh_ui_rects
 * @typedef lh_ui_rects_t
 * @brief A region as up to ::LH_UI_RECTS_MAX rects.
 */
struct lh_ui_rects
{
    lh_ui_rects_fields(lh_ui_rect_t, lh_u32_t);
};
typedef struct lh_ui_rects lh_ui_rects_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p self with no rects in it.
 */
lh_void
lh_ui_rects_init(lh_ui_rects_t *self);

/**
 * @brief Nothing in @p self.
 */
lh_bool_t
lh_ui_rects_is_empty(const lh_ui_rects_t *self);

/**
 * @brief How many rects @p self keeps apart.
 */
lh_u32_t
lh_ui_rects_get_count(const lh_ui_rects_t *self);

/**
 * @brief Rect @p index of @p self, ::lh_null when there is no such rect.
 */
const lh_ui_rect_t *
lh_ui_rects_get_as_const(const lh_ui_rects_t *self, lh_u32_t index);

/**
 * @brief Whether @p a and @p b can become one rect without covering a pixel
 *        neither of them covers: they overlap, or they share an edge.
 */
lh_bool_t
lh_ui_rects_touch(const lh_ui_rect_t *a, const lh_ui_rect_t *b);

/**
 * @brief Pixels @p rect holds, width times height.
 */
lh_ui_scalar_t
lh_ui_rects_area(const lh_ui_rect_t *rect);

/**
 * @brief Pixels @p a and @p b together cover that neither covers on its own:
 *        what merging them would spend.
 */
lh_ui_scalar_t
lh_ui_rects_waste(const lh_ui_rect_t *a, const lh_ui_rect_t *b);

/**
 * @brief Add @p rect to @p self, merging everything it touches.
 *
 * Nothing for an empty @p rect. A full ::lh_ui_rects_t squeezes instead
 * (::lh_ui_rects_squeeze).
 */
lh_void
lh_ui_rects_add(lh_ui_rects_t *self, const lh_ui_rect_t *rect);

/**
 * @brief Merge every pair of @p self that touches, until none does.
 *
 * ::lh_ui_rects_add does this for the caller; it is here because a list that
 * was filled by hand is not a region until it is closed.
 */
lh_void
lh_ui_rects_close(lh_ui_rects_t *self);

/**
 * @brief One step of ::lh_ui_rects_close: merge the first pair of @p self that
 *        touches, and say whether there was one.
 *
 * False means no pair of @p self touches, which is the same thing as saying the
 * list is a region.
 */
lh_bool_t
lh_ui_rects_join_once(lh_ui_rects_t *self);

/**
 * @brief Remove rect @p index of @p self and close the gap it leaves: what
 *        came after it moves down.
 */
lh_void
lh_ui_rects_drop(lh_ui_rects_t *self, lh_u32_t index);

/**
 * @brief A full ::lh_ui_rects_t: put @p rect into the stored one the pair
 *        wastes the fewest pixels on (::lh_ui_rects_waste), then
 *        ::lh_ui_rects_close.
 */
lh_void
lh_ui_rects_squeeze(lh_ui_rects_t *self, const lh_ui_rect_t *rect);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_RECTS_H */
