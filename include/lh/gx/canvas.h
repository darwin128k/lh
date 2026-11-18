/**
 * @file backend.h
 * @brief What drawing needs from below: ::lh_gx_canvas_t.
 *
 * The canvas is a contract, not a thing: a table of the shapes a backend can
 * draw, filled by the backend — `sw` into a pixmap (`lh/gx/backend/sw.h`), and
 * later the browser, a DMA2D, a GPU. It knows **how** to draw, not where (that is
 * the backend's context) and not what was drawn before: it keeps nothing — no
 * clip stack, no damage, no scene. The clip and the target come with every
 * call; the renderer above keeps the state of the frame and the damage
 * (`docs/design/gx.md`, section 5.1).
 *
 * Required: `fill_rect`, which every other shape can come down to. Optional:
 * `fill_mask` (glyphs, anti-aliased edges); each optional slot's type says
 * what happens without it. More slots are added when a drawing needs them.
 */

#ifndef LH_GX_CANVAS_H
#define LH_GX_CANVAS_H

#include <lh/gx/canvas/fields.h>
#include <lh/gx/canvas/fill_mask/fn.h>
#include <lh/gx/canvas/fill_rect/fn.h>

/**
 * @struct lh_gx_canvas
 * @brief The slots a canvas backend fills.
 */
struct lh_gx_canvas
{
    /** Members: ::lh_gx_canvas_fields. */
    lh_gx_canvas_fields(lh_gx_canvas_fill_rect_fn, lh_gx_canvas_fill_mask_fn);
};
/** @copybrief lh_gx_canvas */
typedef struct lh_gx_canvas lh_gx_canvas_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The `fill_rect` slot of @p self (level 1): fill a rectangle with a colour.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_gx_canvas_fill_rect_fn *
lh_gx_canvas_get_fill_rect(const lh_gx_canvas_t *self);

/**
 * @brief Store the `fill_rect` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_gx_canvas_set_fill_rect(lh_gx_canvas_t *self, lh_gx_canvas_fill_rect_fn *fn);

/**
 * @brief The `fill_mask` slot of @p self (level 1): lay a colour through a coverage mask.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_gx_canvas_fill_mask_fn *
lh_gx_canvas_get_fill_mask(const lh_gx_canvas_t *self);

/**
 * @brief Store the `fill_mask` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_gx_canvas_set_fill_mask(lh_gx_canvas_t *self, lh_gx_canvas_fill_mask_fn *fn);

/**
 * @brief Empty table: every slot ::lh_null.
 *
 * A backend then sets the slots it has. The table is the backend's and is
 * usually a constant it hands out; nothing here owns it.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gx_canvas_init(lh_gx_canvas_t *self);

/**
 * @brief True when every required slot of @p self is set: `fill_rect`.
 *
 * The other slots are optional, and each one's type says what happens
 * without it.
 *
 * @param self The table; must not be null.
 * @return True when the table can be used.
 */
lh_bool_t
lh_gx_canvas_is_complete(const lh_gx_canvas_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CANVAS_H */
