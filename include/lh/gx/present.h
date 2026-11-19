/**
 * @file present.h
 * @brief What showing a frame needs from below: ::lh_gx_present_t.
 *
 * The second contract under the renderer, beside the canvas
 * (`lh/gx/canvas.h`). The canvas **draws** into pixels; the present **shows**
 * the pixels that changed — a window repaints them, a panel on a board is sent
 * them, a test checks them. The renderer hands back the rectangles it drew
 * (::lh_gx_render_draw), and only those go to the present: on a board's bus a
 * button that changed is a few kilobytes, not a frame (`docs/design/gx.md`,
 * section 5.3).
 *
 * The table is filled by the target and holds no state; the target's context
 * goes beside it into every call. ::lh_gx_present_draw is the frame: draw what
 * changed, then show it.
 */

#ifndef LH_GX_PRESENT_H
#define LH_GX_PRESENT_H

#include <lh/gx/present/fields.h>
#include <lh/gx/present/show/fn.h>
#include <lh/gx/render.h>

/**
 * @struct lh_gx_present
 * @brief The slots a present target fills. Members: ::lh_gx_present_fields.
 */
struct lh_gx_present
{
    /** Members: ::lh_gx_present_fields. */
    lh_gx_present_fields(lh_gx_present_show_fn);
};
/** @copybrief lh_gx_present */
typedef struct lh_gx_present lh_gx_present_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The `show` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the table is not filled.
 */
lh_gx_present_show_fn *
lh_gx_present_get_show(const lh_gx_present_t *self);

/**
 * @brief Store the `show` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call.
 */
lh_void
lh_gx_present_set_show(lh_gx_present_t *self, lh_gx_present_show_fn *fn);

/**
 * @brief Empty table: every slot ::lh_null. The target then sets its own.
 *
 * @param self The table; must not be null.
 */
lh_void
lh_gx_present_init(lh_gx_present_t *self);

/**
 * @brief True when every required slot is set: `show`.
 *
 * @param self The table; must not be null.
 */
lh_bool_t
lh_gx_present_is_complete(const lh_gx_present_t *self);

/**
 * @brief Draw what changed under @p render and show it: ::lh_gx_render_draw,
 *        then the `show` slot with the rectangles drawn — nothing is shown when
 *        nothing changed.
 *
 * @param self    The table; must be complete.
 * @param context The target's context, handed to `show`.
 * @param render  The renderer; its canvas draws into @p pixels.
 * @param pixels  The pixels the renderer's canvas draws into.
 * @return ::lh_bool_true when something was drawn and shown.
 */
lh_bool_t
lh_gx_present_draw(const lh_gx_present_t *self, lh_ptr context, lh_gx_render_t *render,
                   const lh_gx_pixmap_t *pixels);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_PRESENT_H */
