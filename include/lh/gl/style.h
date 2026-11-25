/**
 * @file style.h
 * @brief How an entity is painted: ::lh_gl_style_t.
 *
 * Holds a fill ::lh_gl_paint_t by value, a corner radius, a shadow cast under
 * that fill, and how text is drawn: a font (not owned) and a text paint.
 * Entities point at a style, so one style is shared by every entity that looks
 * the same. `pressed` is the one style that is chosen rather than shared: an
 * entity that is pressed paints it instead (::lh_gl_entity_is_pressed), and it
 * may change how the entity looks — fill, radius, shadow, text paint — not its
 * geometry. `border` is painted after the children in `border_fill`; uniform widths
 * follow the fill's corner radii.
 *
 * The fields are kept in four parts, each a struct of its own: the box
 * (`lh/gl/style/box.h`), the typography (`lh/gl/style/typography.h`), how the
 * style answers a pointer (`lh/gl/style/pointer.h`) and how it is reached over
 * time (`lh/gl/style/motion.h`). The accessors below read and write through
 * them, so a caller who works with one field never needs the part it is in.
 */

#ifndef LH_GL_STYLE_H
#define LH_GL_STYLE_H

#include <lh/gl/radius.h>
#include <lh/gl/style/box.h>
#include <lh/gl/style/fields.h>
#include <lh/gl/style/motion.h>
#include <lh/gl/style/pointer.h>
#include <lh/gl/style/typography.h>

/**
 * @struct lh_gl_style
 * @brief Paint recipe for one entity.
 */
struct lh_gl_style
{
    /** Members: ::lh_gl_style_fields. */
    lh_gl_style_fields(lh_gl_style_box_t, lh_gl_style_typography_t, lh_gl_style_pointer_t,
                       lh_gl_style_motion_t);
};
/** @copybrief lh_gl_style */
typedef struct lh_gl_style lh_gl_style_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The box part of @p self, read in place (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_style_box_t *
lh_gl_style_get_box(const lh_gl_style_t *self);

/**
 * @brief The box part of @p self, to change in place (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_style_box_t *
lh_gl_style_get_box_mut(lh_gl_style_t *self);

/**
 * @brief The typography part of @p self, read in place (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_style_typography_t *
lh_gl_style_get_typography(const lh_gl_style_t *self);

/**
 * @brief The typography part of @p self, to change in place (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_style_typography_t *
lh_gl_style_get_typography_mut(lh_gl_style_t *self);

/**
 * @brief How @p self answers a pointer, read in place (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_style_pointer_t *
lh_gl_style_get_pointer(const lh_gl_style_t *self);

/**
 * @brief How @p self answers a pointer, to change in place (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_style_pointer_t *
lh_gl_style_get_pointer_mut(lh_gl_style_t *self);

/**
 * @brief How @p self is reached over time, read in place (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_style_motion_t *
lh_gl_style_get_motion(const lh_gl_style_t *self);

/**
 * @brief How @p self is reached over time, to change in place (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_style_motion_t *
lh_gl_style_get_motion_mut(lh_gl_style_t *self);

/**
 * @brief Fill @p self with the empty fill paint, square corners, the default
 *        font (::lh_gl_font_get_default) and opaque black text.
 */
lh_void
lh_gl_style_init(lh_gl_style_t *self);

/**
 * @brief Fill paint of @p self. Never ::lh_null; may be empty.
 */
const lh_gl_paint_t *
lh_gl_style_get_fill(const lh_gl_style_t *self);

/**
 * @brief Replace the fill of @p self with a copy of @p fill.
 *
 * ::lh_null clears the fill to the empty paint.
 */
lh_void
lh_gl_style_assign_fill(lh_gl_style_t *self, const lh_gl_paint_t *fill);

/** @p from moved @p weight of the way to @p to, in weights of 255. Written so it means
   the same on both arithmetic paths: `to - from` first, so a step that runs the other
   way stays negative. */
lh_gl_scalar_t
lh_gl_style_mix(lh_gl_scalar_t from, lh_gl_scalar_t to, lh_u32_t weight);

/** @p from moved @p weight of the way to @p to, every channel and the alpha with them:
   a shadow fading in from nothing is a colour whose alpha grows. */
lh_void
lh_gl_style_mix_color(const lh_gl_color_t *from, const lh_gl_color_t *to, lh_u32_t weight,
                      lh_gl_color_t *out);

/** @p to's paint, or -- when both ends are one colour -- the colour between them. */
lh_void
lh_gl_style_mix_paint(lh_gl_paint_t *out, const lh_gl_paint_t *from, const lh_gl_paint_t *to,
                      lh_u32_t weight);

/**
 * @brief Solid fill color of @p self, or ::lh_null when the fill is not solid.
 */
const lh_gl_color_t *
lh_gl_style_get_fill_color(const lh_gl_style_t *self);

/**
 * @brief Corner radius of @p self (`0` = square). Not clamped: the canvas
 *        clamps it to the rect it fills.
 */
lh_gl_scalar_t
lh_gl_style_get_radius(const lh_gl_style_t *self);

/**
 * @brief Replace the corner radius of @p self. @p radius must not be negative;
 *        ::LH_GL_RADIUS_CIRCLE asks for the largest one.
 */
lh_void
lh_gl_style_set_radius_v(lh_gl_style_t *self, lh_gl_scalar_t radius);

/**
 * @brief The radius of each corner of the fill of @p self.
 */
const lh_gl_radii_t *
lh_gl_style_get_radii(const lh_gl_style_t *self);

/**
 * @brief Round each corner of the fill of @p self on its own (copied): CSS's
 *        `border-radius: 8px 8px 0 0`. ::lh_gl_style_get_radius is then the smallest
 *        of them, which is the radius a clipping parent cuts its children with and the
 *        one a hit test rounds by; ::lh_gl_style_set_radius_v sets all four again.
 */
lh_void
lh_gl_style_set_radii_v(lh_gl_style_t *self, const lh_gl_radii_t *radii);

/**
 * @brief Corner radius a hit test rounds @p self with (`0` = the whole rect).
 *        Not clamped: the hit test clamps it to the rect, as the canvas does.
 */
lh_gl_scalar_t
lh_gl_style_get_hit_radius(const lh_gl_style_t *self);

/**
 * @brief Replace the corner radius a hit test rounds @p self with.
 *
 * `0` — the default — is the whole rect: a rounded look is pressed by its rect,
 * because the rect is what a pointer aims at and the rounding is a look. Set it
 * where the rect is much larger than the shape (a traffic light, a round icon)
 * so that what sits around the shape is not a press of it.
 *
 * @p radius must not be negative; ::LH_GL_RADIUS_CIRCLE asks for the largest
 * one, which makes the hit shape follow the fill exactly.
 */
lh_void
lh_gl_style_set_hit_radius_v(lh_gl_style_t *self, lh_gl_scalar_t radius);

/**
 * @brief Inner space of @p self, per side (`0` = content touches the edge):
 *        where a label starts its text and a layout its first child, and the
 *        room a container leaves after its last child.
 */
const lh_gl_insets_t *
lh_gl_style_get_padding(const lh_gl_style_t *self);

/**
 * @brief @p padding on every side of @p self. It must not be negative.
 */
lh_void
lh_gl_style_set_padding_all(lh_gl_style_t *self, lh_gl_scalar_t padding);

/**
 * @brief Replace the padding of @p self, per side. No side may be negative.
 */
lh_void
lh_gl_style_set_padding_v(lh_gl_style_t *self, const lh_gl_insets_t *padding);

/**
 * @brief Font text is drawn with, or ::lh_null (no text drawn).
 */
const lh_gl_font_t *
lh_gl_style_get_font(const lh_gl_style_t *self);

/**
 * @brief Point @p self at @p font. Not copied; ::lh_null means no font.
 */
lh_void
lh_gl_style_set_font(lh_gl_style_t *self, const lh_gl_font_t *font);

/**
 * @brief Text paint of @p self. Never ::lh_null; may be empty.
 */
const lh_gl_paint_t *
lh_gl_style_get_text(const lh_gl_style_t *self);

/**
 * @brief Replace the text paint of @p self with a copy of @p text.
 *
 * ::lh_null clears it to the empty paint (no text drawn).
 */
lh_void
lh_gl_style_assign_text(lh_gl_style_t *self, const lh_gl_paint_t *text);

/**
 * @brief Solid text color of @p self, or ::lh_null when the text paint is not
 *        solid.
 */
const lh_gl_color_t *
lh_gl_style_get_text_color(const lh_gl_style_t *self);

/**
 * @brief Where text starts across the padded box of @p self.
 *
 * ::lh_gl_text_align_h_left by default, which is the only thing a style did before
 * alignment existed. ::lh_gl_text_align_h_center and ::lh_gl_text_align_h_right move
 * the text's left edge; text wider than the box starts at the left whatever this
 * says, because there is nowhere else for it to start.
 */
lh_gl_text_align_h_t
lh_gl_style_get_align_h(const lh_gl_style_t *self);

/**
 * @brief Replace the horizontal text alignment of @p self.
 */
lh_void
lh_gl_style_set_align_h(lh_gl_style_t *self, lh_gl_text_align_h_t align);

/**
 * @brief Where text starts down the padded box of @p self.
 *
 * ::lh_gl_text_align_v_top by default. ::lh_gl_text_align_v_center and
 * ::lh_gl_text_align_v_bottom move the text's top edge; text taller than the box
 * starts at the top whatever this says.
 */
lh_gl_text_align_v_t
lh_gl_style_get_align_v(const lh_gl_style_t *self);

/**
 * @brief Replace the vertical text alignment of @p self.
 */
lh_void
lh_gl_style_set_align_v(lh_gl_style_t *self, lh_gl_text_align_v_t align);

/**
 * @brief What a line of text too wide for the padded box draws.
 */
lh_gl_text_overflow_t
lh_gl_style_get_overflow(const lh_gl_style_t *self);

/**
 * @brief Draw a line that does not fit whole and clipped
 *        (::lh_gl_text_overflow_clip, the default) or shortened with an ellipsis
 *        (::lh_gl_text_overflow_ellipsis).
 */
lh_void
lh_gl_style_set_overflow(lh_gl_style_t *self, lh_gl_text_overflow_t overflow);

/**
 * @brief Whether text is broken into lines as wide as the padded box.
 */
lh_gl_text_wrap_t
lh_gl_style_get_wrap(const lh_gl_style_t *self);

/**
 * @brief Break text only where it has line breaks (::lh_gl_text_wrap_none, the
 *        default) or also before a word that would not fit (::lh_gl_text_wrap_words).
 */
lh_void
lh_gl_style_set_wrap(lh_gl_style_t *self, lh_gl_text_wrap_t wrap);

/**
 * @brief Shadow cast under the fill of @p self. Never ::lh_null; the empty one
 *        casts nothing (::lh_gl_shadow_is_empty).
 */
const lh_gl_shadow_t *
lh_gl_style_get_shadow(const lh_gl_style_t *self);

/**
 * @brief Replace the shadow of @p self with a copy of @p shadow.
 *
 * Copied, like the paints. ::lh_null clears it to the empty shadow.
 */
lh_void
lh_gl_style_set_shadow_v(lh_gl_style_t *self, const lh_gl_shadow_t *shadow);

/**
 * @brief Style an entity painted while it is pressed, or ::lh_null for none.
 *
 * Not copied: it is a style like any other, and it has to outlive every entity
 * that points at it — the same rule as ::lh_gl_entity_set_style. It may change
 * how the entity looks (fill, radius, shadow, text paint) and not its geometry:
 * padding, font and alignment are read from the entity's own style, so nothing
 * moves under the pointer.
 */
const lh_gl_style_t *
lh_gl_style_get_pressed(const lh_gl_style_t *self);

/**
 * @brief Replace the pressed style of @p self with @p pressed (not copied).
 *
 * ::lh_null clears it, and an entity with none looks the same pressed as not.
 */
lh_void
lh_gl_style_set_pressed(lh_gl_style_t *self, const lh_gl_style_t *pressed);

/**
 * @brief How far each edge of @p self paints. Zero on a side paints nothing
 *        there.
 */
const lh_gl_insets_t *
lh_gl_style_get_border(const lh_gl_style_t *self);

/**
 * @brief Replace the border widths of @p self. A side must not be negative.
 *        ::lh_null clears every side to 0.
 */
lh_void
lh_gl_style_set_border_v(lh_gl_style_t *self, const lh_gl_insets_t *border);

/**
 * @brief Paint the border is drawn in. Never ::lh_null; may be empty, and an
 *        empty paint draws no border.
 */
const lh_gl_paint_t *
lh_gl_style_get_border_fill(const lh_gl_style_t *self);

/**
 * @brief Replace the border paint of @p self with a copy of @p fill.
 *        ::lh_null clears it to the empty paint.
 */
lh_void
lh_gl_style_assign_border_fill(lh_gl_style_t *self, const lh_gl_paint_t *fill);

/**
 * @brief How long an entity takes to reach @p self from the style it had, in
 *        milliseconds. `0` — the default — means it reaches it at once, which is
 *        how every style behaved before transitions existed.
 *
 * It is read off the style an entity is **switched to**, not the one it leaves,
 * so every style that can be reached by a switch has to carry the time of that
 * switch. A generated screen does this for every look of a row at once, which is
 * why this is one call per style rather than one per entity.
 *
 * A style alone changes nothing: ::lh_gl_entity_set_style_v starts the transition,
 * ::lh_gl_view_advance is what moves it, and ::lh_gl_style_init_blend is the look
 * on screen while it does.
 */
lh_u16_t
lh_gl_style_get_transition(const lh_gl_style_t *self);

/**
 * @brief Set how long an entity takes to reach @p self, in milliseconds
 *        (::lh_gl_style_get_transition).
 */
lh_void
lh_gl_style_set_transition(lh_gl_style_t *self, lh_u16_t ms);

/**
 * @brief The curve an entity moves on towards @p self (::lh_gl_easing_ease, CSS's
 *        default, until a style says otherwise).
 */
lh_gl_easing_t
lh_gl_style_get_easing(const lh_gl_style_t *self);

/**
 * @brief Set the curve an entity moves on towards @p self.
 */
lh_void
lh_gl_style_set_easing(lh_gl_style_t *self, lh_gl_easing_t easing);

/**
 * @brief @p self as the look @p weight of the way from @p from to @p to (weights of
 *        `255`): what is on screen while a transition runs.
 *
 * Everything that has an amount is mixed -- the fill and the text and border paints
 * when both ends are solid, every corner radius, the shadow's colour, fade and
 * shift -- and everything else is @p to's from the first frame: the font, the
 * alignment, the padding and the border's widths (a box does not move its contents
 * by fractions of a pixel while it fades), a gradient, and the pressed look. A paint
 * that is solid at one end only is @p to's at once, as is a shadow cast at one end
 * only fading in from the other's nothing.
 */
lh_void
lh_gl_style_init_blend(lh_gl_style_t *self, const lh_gl_style_t *from, const lh_gl_style_t *to,
                       lh_u32_t weight);

/**
 * @brief The paint the background is filled with, to change in place (level 2).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_paint_t *
lh_gl_style_get_fill_mut(lh_gl_style_t *self);

/**
 * @brief The corner radii, to change in place (level 2).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_radii_t *
lh_gl_style_get_radii_mut(lh_gl_style_t *self);

/**
 * @brief The padding inside the border, to change in place (level 2).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_insets_t *
lh_gl_style_get_padding_mut(lh_gl_style_t *self);

/**
 * @brief The paint the text is drawn with, to change in place (level 2).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_paint_t *
lh_gl_style_get_text_mut(lh_gl_style_t *self);

/**
 * @brief The shadow, to change in place (level 2).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_shadow_t *
lh_gl_style_get_shadow_mut(lh_gl_style_t *self);

/**
 * @brief The border widths, to change in place (level 2).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_insets_t *
lh_gl_style_get_border_mut(lh_gl_style_t *self);

/**
 * @brief The paint the border is drawn with, to change in place (level 2).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_paint_t *
lh_gl_style_get_border_fill_mut(lh_gl_style_t *self);

/**
 * @brief Store the paint the background is filled with (level 2).
 *
 * A setter through the style's part: the value is stored as given and is not
 * validated.
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param fill The new value.
 */
lh_void
lh_gl_style_set_fill(lh_gl_style_t *self, lh_gl_paint_t fill);

/**
 * @brief Store the corner radii (level 2).
 *
 * A setter through the style's part: the value is stored as given and is not
 * validated.
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param radii The new value.
 */
lh_void
lh_gl_style_set_radii(lh_gl_style_t *self, lh_gl_radii_t radii);

/**
 * @brief Store the padding inside the border (level 2).
 *
 * A setter through the style's part: the value is stored as given and is not
 * validated.
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param padding The new value.
 */
lh_void
lh_gl_style_set_padding(lh_gl_style_t *self, lh_gl_insets_t padding);

/**
 * @brief Store the paint the text is drawn with (level 2).
 *
 * A setter through the style's part: the value is stored as given and is not
 * validated.
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param text The new value.
 */
lh_void
lh_gl_style_set_text(lh_gl_style_t *self, lh_gl_paint_t text);

/**
 * @brief Store the shadow (level 2).
 *
 * A setter through the style's part: the value is stored as given and is not
 * validated.
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param shadow The new value.
 */
lh_void
lh_gl_style_set_shadow(lh_gl_style_t *self, lh_gl_shadow_t shadow);

/**
 * @brief Store the border widths (level 2).
 *
 * A setter through the style's part: the value is stored as given and is not
 * validated.
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param border The new value.
 */
lh_void
lh_gl_style_set_border(lh_gl_style_t *self, lh_gl_insets_t border);

/**
 * @brief Store the paint the border is drawn with (level 2).
 *
 * A setter through the style's part: the value is stored as given and is not
 * validated.
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param border_fill The new value.
 */
lh_void
lh_gl_style_set_border_fill(lh_gl_style_t *self, lh_gl_paint_t border_fill);

/**
 * @brief Store the corner radius hits are tested against (level 2).
 *
 * A setter through the style's part: the value is stored as given and is not
 * validated.
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param hit_radius The new value.
 */
lh_void
lh_gl_style_set_hit_radius(lh_gl_style_t *self, lh_gl_scalar_t hit_radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_STYLE_H */
