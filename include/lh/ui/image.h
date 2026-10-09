/**
 * @file image.h
 * @brief A mask drawn where an entity is: ::lh_ui_image_t.
 *
 * The piece a button was missing: a picture is an entity, so a flow can place it
 * next to a caption and size it (::lh_ui_place_t). An image is a
 * ::lh_ui_mask_t — `lh/ui/mask.h` already says that a baked icon is one, the way
 * a glyph is — painted in one colour through ::lh_ui_canvas_fill_mask, and it
 * **measures as its own size**: on ::lh_ui_entity_event_measure it grows the
 * content bounds by the mask, so `wrap` asks it how long it is and gets the truth
 * rather than zero.
 *
 * A mask is not scaled. An image is as big as its mask, and a flow that gives it
 * more room leaves the rest of the box empty rather than stretching the picture —
 * scaling a one-bit mask is a different piece of work and not this one.
 */

#ifndef LH_UI_IMAGE_H
#define LH_UI_IMAGE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/image/fields.h>
#include <lh/ui/mask.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/void.h>

/**
 * @struct lh_ui_image
 * @typedef lh_ui_image_t
 * @brief An entity, the mask it shows and the colour it shows it in.
 */
struct lh_ui_image
{
    lh_ui_image_fields(lh_ui_entity_t, lh_ui_mask_t, lh_ui_color_t, lh_ui_scalar_t);
};
typedef struct lh_ui_image lh_ui_image_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Class ───────────────────────────────────────────────────────────────── */

/**
 * @brief Class of ::lh_ui_image_t, derived from ::lh_ui_entity_class.
 */
extern const lh_ui_entity_class_t lh_ui_image_class;

/**
 * @brief Event function of ::lh_ui_image_class: on
 *        ::lh_ui_entity_event_draw the mask, on
 *        ::lh_ui_entity_event_measure the size of the mask so a flow can wrap it;
 *        everything else goes to the base class.
 */
lh_void
lh_ui_image_event(const struct lh_ui_entity *self, const struct lh_ui_entity_event *event);

/**
 * @brief Paint the mask of @p self in its tint, at ::lh_ui_image_get_origin.
 *        Nothing without a mask or without a canvas.
 */
lh_void
lh_ui_image_draw(const struct lh_ui_image *self, struct lh_ui_canvas *canvas);

/**
 * @brief On ::lh_ui_entity_event_measure: grow @p bounds by the size of the
 *        mask, which is what a `wrap` place asks an image and has to get back.
 */
lh_void
lh_ui_image_on_measure(const struct lh_ui_image *self, const struct lh_ui_entity_event *event);

/**
 * @brief On ::lh_ui_entity_event_baseline: answer ::lh_ui_image_get_baseline.
 *        Other events are ignored.
 */
lh_void
lh_ui_image_on_baseline(const struct lh_ui_image *self, const struct lh_ui_entity_event *event);

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

/**
 * @brief An image at @p rect showing @p mask (::lh_null for none) in opaque
 *        white.
 */
lh_void
lh_ui_image_init(lh_ui_image_t *self, lh_ui_rect_t rect, const lh_ui_mask_t *mask);

/**
 * @brief Say that @p self's baseline is @p baseline rows below the top of its own
 *        box, so a row can put it on the same line as the text beside it
 *        (::lh_ui_place_align_baseline). -1 is no line, which is the default.
 *
 * A mask carries no baseline of its own — it is cropped to ink and knows nothing
 * about the type it came out of. An icon taken from the font knows exactly, and the
 * number is `-lh_ui_font_get_top(font, code)`: how many rows the baseline sits below
 * the top of *that glyph's* ink, which is `ascent - ink_top` and needs no font metric
 * beyond the one the top already is.
 *
 * The box is the mask and the box's top is where the ink starts
 * (::lh_ui_image_get_origin), so the ink is what has to stand on the line — which is
 * why this is not the mask's height. The two agree for a glyph that sits on the
 * baseline and part company for one that hangs below it: the demo's 'x' is 9 rows of
 * x-height with `top -9`, so both say 9, while a 'p' would put its 13 rows of top and
 * tail on 13, not on however many the crop happens to be.
 *
 * Measured on the demo's Hide panel button, putting it on the caption's baseline
 * (row 200) makes the 'x' 191..199, its x-height line exactly where the letters' is.
 */
lh_void
lh_ui_image_set_baseline(lh_ui_image_t *self, lh_ui_scalar_t baseline);

/**
 * @brief Rows from the top of @p self's box down to its baseline, or -1 when it
 *        has none.
 */
lh_ui_scalar_t
lh_ui_image_get_baseline(const lh_ui_image_t *self);

/**
 * @brief The entity @p self is: what a tree holds and what a hit test returns.
 */
lh_ui_entity_t *
lh_ui_image_as_entity(lh_ui_image_t *self);

/**
 * @brief The image @p entity is, or ::lh_null when it is not one — the class
 *        check is what tells the two apart, the same way ::lh_ui_entity_as_button
 *        does.
 */
lh_ui_image_t *
lh_ui_entity_as_image(lh_ui_entity_t *entity);

/* ── Picture ─────────────────────────────────────────────────────────────── */

/**
 * @brief The mask @p self shows (not owned, ::lh_null for none: then it draws
 *        nothing and measures nothing).
 */
const lh_ui_mask_t *
lh_ui_image_get_mask(const lh_ui_image_t *self);

/**
 * @brief Show @p mask from now on (not owned).
 */
lh_void
lh_ui_image_set_mask(lh_ui_image_t *self, const lh_ui_mask_t *mask);

/**
 * @brief The colour @p self paints its mask in.
 */
const lh_ui_color_t *
lh_ui_image_get_tint(const lh_ui_image_t *self);

/**
 * @brief Paint @p self's mask in @p tint from now on.
 */
lh_void
lh_ui_image_set_tint(lh_ui_image_t *self, const lh_ui_color_t *tint);

/**
 * @brief The size of the mask @p self shows — the picture itself, without the
 *        padding around it. What an app asks before it puts an image somewhere,
 *        and the size ::lh_ui_image_on_measure measures it as.
 */
lh_ui_size_t
lh_ui_image_get_size(const lh_ui_image_t *self);

/**
 * @brief Where the picture sits inside the box of @p self: the top-left corner
 *        of the box, moved in by the padding of its style — the same origin a
 *        label's text gets, and the one place it is worked out. The picture is
 *        not centred and not scaled, so this is the only position it has.
 */
lh_ui_point_t
lh_ui_image_get_origin(const lh_ui_image_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_IMAGE_H */