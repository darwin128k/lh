/**
 * @file style.c
 * @brief Implementation of `lh/gl/style.h`.
 */
#include <lh/core/assert.h>

#include <lh/gl/style.h>
#include <lh/core/util/return.h>
const lh_gl_style_box_t *
lh_gl_style_get_box(const lh_gl_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

lh_gl_style_box_t *
lh_gl_style_get_box_mut(lh_gl_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

const lh_gl_style_typography_t *
lh_gl_style_get_typography(const lh_gl_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->typography);
}

lh_gl_style_typography_t *
lh_gl_style_get_typography_mut(lh_gl_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->typography);
}

const lh_gl_style_pointer_t *
lh_gl_style_get_pointer(const lh_gl_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pointer);
}

lh_gl_style_pointer_t *
lh_gl_style_get_pointer_mut(lh_gl_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pointer);
}

const lh_gl_style_motion_t *
lh_gl_style_get_motion(const lh_gl_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->motion);
}

lh_gl_style_motion_t *
lh_gl_style_get_motion_mut(lh_gl_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->motion);
}

lh_gl_scalar_t
lh_gl_style_mix(lh_gl_scalar_t from, lh_gl_scalar_t to, lh_u32_t weight)
{
    return from + ((to - from) * lh_cast_static(lh_gl_scalar_t, weight)) / lh_gl_scalar(255);
}

lh_void
lh_gl_style_mix_color(const lh_gl_color_t *from, const lh_gl_color_t *to, lh_u32_t weight,
                      lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_color_t result;

    lh_gl_color_init(lh_addr_of(result),
                     lh_gl_color_blend_channel(lh_gl_color_get_r(to), lh_gl_color_get_r(from),
                                               weight, 255U - weight),
                     lh_gl_color_blend_channel(lh_gl_color_get_g(to), lh_gl_color_get_g(from),
                                               weight, 255U - weight),
                     lh_gl_color_blend_channel(lh_gl_color_get_b(to), lh_gl_color_get_b(from),
                                               weight, 255U - weight),
                     lh_gl_color_blend_channel(lh_gl_color_get_a(to), lh_gl_color_get_a(from),
                                               weight, 255U - weight));
    lh_ptr_deref(out) = result;
}

lh_void
lh_gl_style_get_shadow_fade_color(const lh_gl_shadow_t *end, const lh_gl_shadow_t *shape,
                                  lh_gl_color_t *out)
{
    if (!lh_gl_shadow_is_empty(end))
    {
        lh_gl_shadow_copy_color(end, out);
        return;
    }
    lh_gl_shadow_copy_color(shape, out);
    lh_gl_color_set_a(out, 0);
}

lh_void
lh_gl_style_mix_paint(lh_gl_paint_t *out, const lh_gl_paint_t *from, const lh_gl_paint_t *to,
                      lh_u32_t weight)
{
    const lh_gl_color_t *a = lh_gl_paint_get_color_v(from);
    const lh_gl_color_t *b = lh_gl_paint_get_color_v(to);

    lh_gl_paint_init_copy(out, to);
    lh_return_if(lh_null_eq(a) || lh_null_eq(b) || weight >= 255U);
    lh_gl_color_t mixed;
    lh_gl_style_mix_color(a, b, weight, lh_addr_of(mixed));
    lh_gl_paint_init_color(out, lh_addr_of(mixed));
}

lh_void
lh_gl_style_mix_radii(lh_gl_radii_t *out, const lh_gl_radii_t *from, const lh_gl_radii_t *to,
                      lh_u32_t weight)
{
    lh_gl_radii_init(
        out, lh_gl_style_mix(lh_gl_radii_get_top_left(from), lh_gl_radii_get_top_left(to), weight),
        lh_gl_style_mix(lh_gl_radii_get_top_right(from), lh_gl_radii_get_top_right(to), weight),
        lh_gl_style_mix(lh_gl_radii_get_bottom_right(from), lh_gl_radii_get_bottom_right(to),
                        weight),
        lh_gl_style_mix(lh_gl_radii_get_bottom_left(from), lh_gl_radii_get_bottom_left(to),
                        weight));
}

lh_void
lh_gl_style_mix_shadow(lh_gl_shadow_t *out, const lh_gl_shadow_t *from, const lh_gl_shadow_t *to,
                       lh_u32_t weight)
{
    const lh_bool_t from_casts = lh_type_cast(lh_bool_t, !lh_gl_shadow_is_empty(from));
    const lh_bool_t to_casts = lh_type_cast(lh_bool_t, !lh_gl_shadow_is_empty(to));
    const lh_gl_shadow_t *shape = to_casts ? to : from;

    if (!from_casts && !to_casts)
    {
        lh_ptr_deref(out) = lh_ptr_deref(to);
        return;
    }
    lh_ptr_deref(out) = lh_ptr_deref(shape);
    lh_gl_color_t a;
    lh_gl_color_t b;
    lh_gl_color_t color;

    lh_gl_style_get_shadow_fade_color(from, shape, lh_addr_of(a));
    lh_gl_style_get_shadow_fade_color(to, shape, lh_addr_of(b));
    lh_gl_style_mix_color(lh_addr_of(a), lh_addr_of(b), weight, lh_addr_of(color));
    lh_gl_shadow_set_color(out, color);
    lh_return_if(!from_casts || !to_casts);
    lh_gl_shadow_set_spread_v(
        out, lh_gl_style_mix(lh_gl_shadow_get_spread(from), lh_gl_shadow_get_spread(to), weight));
    lh_gl_shadow_set_offset(
        out,
        lh_gl_style_mix(lh_gl_shadow_get_offset_x(from), lh_gl_shadow_get_offset_x(to), weight),
        lh_gl_style_mix(lh_gl_shadow_get_offset_y(from), lh_gl_shadow_get_offset_y(to), weight));
}

lh_void
lh_gl_style_init(lh_gl_style_t *self)
{
    lh_gl_style_box_init(lh_gl_style_get_box_mut(self));
    lh_gl_style_typography_init(lh_gl_style_get_typography_mut(self));
    lh_gl_style_pointer_init(lh_gl_style_get_pointer_mut(self));
    lh_gl_style_motion_init(lh_gl_style_get_motion_mut(self));
}

const lh_gl_paint_t *
lh_gl_style_get_fill(const lh_gl_style_t *self)
{
    return lh_gl_style_box_get_fill(lh_gl_style_get_box(self));
}

lh_gl_paint_t *
lh_gl_style_get_fill_mut(lh_gl_style_t *self)
{
    return lh_gl_style_box_get_fill_mut(lh_gl_style_get_box_mut(self));
}

lh_void
lh_gl_style_set_fill(lh_gl_style_t *self, lh_gl_paint_t fill)
{
    lh_gl_style_box_set_fill(lh_gl_style_get_box_mut(self), fill);
}

const lh_gl_radii_t *
lh_gl_style_get_radii(const lh_gl_style_t *self)
{
    return lh_gl_style_box_get_radii(lh_gl_style_get_box(self));
}

lh_gl_radii_t *
lh_gl_style_get_radii_mut(lh_gl_style_t *self)
{
    return lh_gl_style_box_get_radii_mut(lh_gl_style_get_box_mut(self));
}

lh_void
lh_gl_style_set_radii(lh_gl_style_t *self, lh_gl_radii_t radii)
{
    lh_gl_style_box_set_radii(lh_gl_style_get_box_mut(self), radii);
}

const lh_gl_insets_t *
lh_gl_style_get_padding(const lh_gl_style_t *self)
{
    return lh_gl_style_box_get_padding(lh_gl_style_get_box(self));
}

lh_gl_insets_t *
lh_gl_style_get_padding_mut(lh_gl_style_t *self)
{
    return lh_gl_style_box_get_padding_mut(lh_gl_style_get_box_mut(self));
}

lh_void
lh_gl_style_set_padding(lh_gl_style_t *self, lh_gl_insets_t padding)
{
    lh_gl_style_box_set_padding(lh_gl_style_get_box_mut(self), padding);
}

const lh_gl_shadow_t *
lh_gl_style_get_shadow(const lh_gl_style_t *self)
{
    return lh_gl_style_box_get_shadow(lh_gl_style_get_box(self));
}

lh_gl_shadow_t *
lh_gl_style_get_shadow_mut(lh_gl_style_t *self)
{
    return lh_gl_style_box_get_shadow_mut(lh_gl_style_get_box_mut(self));
}

lh_void
lh_gl_style_set_shadow(lh_gl_style_t *self, lh_gl_shadow_t shadow)
{
    lh_gl_style_box_set_shadow(lh_gl_style_get_box_mut(self), shadow);
}

const lh_gl_insets_t *
lh_gl_style_get_border(const lh_gl_style_t *self)
{
    return lh_gl_style_box_get_border(lh_gl_style_get_box(self));
}

lh_gl_insets_t *
lh_gl_style_get_border_mut(lh_gl_style_t *self)
{
    return lh_gl_style_box_get_border_mut(lh_gl_style_get_box_mut(self));
}

lh_void
lh_gl_style_set_border(lh_gl_style_t *self, lh_gl_insets_t border)
{
    lh_gl_style_box_set_border(lh_gl_style_get_box_mut(self), border);
}

const lh_gl_paint_t *
lh_gl_style_get_border_fill(const lh_gl_style_t *self)
{
    return lh_gl_style_box_get_border_fill(lh_gl_style_get_box(self));
}

lh_gl_paint_t *
lh_gl_style_get_border_fill_mut(lh_gl_style_t *self)
{
    return lh_gl_style_box_get_border_fill_mut(lh_gl_style_get_box_mut(self));
}

lh_void
lh_gl_style_set_border_fill(lh_gl_style_t *self, lh_gl_paint_t border_fill)
{
    lh_gl_style_box_set_border_fill(lh_gl_style_get_box_mut(self), border_fill);
}

const lh_gl_font_t *
lh_gl_style_get_font(const lh_gl_style_t *self)
{
    return lh_gl_style_typography_get_font(lh_gl_style_get_typography(self));
}

lh_void
lh_gl_style_set_font(lh_gl_style_t *self, const lh_gl_font_t *font)
{
    lh_gl_style_typography_set_font(lh_gl_style_get_typography_mut(self), font);
}

const lh_gl_paint_t *
lh_gl_style_get_text(const lh_gl_style_t *self)
{
    return lh_gl_style_typography_get_paint(lh_gl_style_get_typography(self));
}

lh_gl_paint_t *
lh_gl_style_get_text_mut(lh_gl_style_t *self)
{
    return lh_gl_style_typography_get_paint_mut(lh_gl_style_get_typography_mut(self));
}

lh_void
lh_gl_style_set_text(lh_gl_style_t *self, lh_gl_paint_t text)
{
    lh_gl_style_typography_set_paint(lh_gl_style_get_typography_mut(self), text);
}

lh_gl_text_align_h_t
lh_gl_style_get_align_h(const lh_gl_style_t *self)
{
    return lh_gl_style_typography_get_align_h(lh_gl_style_get_typography(self));
}

lh_void
lh_gl_style_set_align_h(lh_gl_style_t *self, lh_gl_text_align_h_t align)
{
    lh_gl_style_typography_set_align_h(lh_gl_style_get_typography_mut(self), align);
}

lh_gl_text_align_v_t
lh_gl_style_get_align_v(const lh_gl_style_t *self)
{
    return lh_gl_style_typography_get_align_v(lh_gl_style_get_typography(self));
}

lh_void
lh_gl_style_set_align_v(lh_gl_style_t *self, lh_gl_text_align_v_t align)
{
    lh_gl_style_typography_set_align_v(lh_gl_style_get_typography_mut(self), align);
}

lh_gl_text_overflow_t
lh_gl_style_get_overflow(const lh_gl_style_t *self)
{
    return lh_gl_style_typography_get_overflow(lh_gl_style_get_typography(self));
}

lh_void
lh_gl_style_set_overflow(lh_gl_style_t *self, lh_gl_text_overflow_t overflow)
{
    lh_gl_style_typography_set_overflow(lh_gl_style_get_typography_mut(self), overflow);
}

lh_gl_text_wrap_t
lh_gl_style_get_wrap(const lh_gl_style_t *self)
{
    return lh_gl_style_typography_get_wrap(lh_gl_style_get_typography(self));
}

lh_void
lh_gl_style_set_wrap(lh_gl_style_t *self, lh_gl_text_wrap_t wrap)
{
    lh_gl_style_typography_set_wrap(lh_gl_style_get_typography_mut(self), wrap);
}

lh_gl_scalar_t
lh_gl_style_get_hit_radius(const lh_gl_style_t *self)
{
    return lh_gl_style_pointer_get_hit_radius(lh_gl_style_get_pointer(self));
}

lh_void
lh_gl_style_set_hit_radius(lh_gl_style_t *self, lh_gl_scalar_t hit_radius)
{
    lh_gl_style_pointer_set_hit_radius(lh_gl_style_get_pointer_mut(self), hit_radius);
}

const lh_gl_style_t *
lh_gl_style_get_pressed(const lh_gl_style_t *self)
{
    return lh_gl_style_pointer_get_pressed(lh_gl_style_get_pointer(self));
}

lh_void
lh_gl_style_set_pressed(lh_gl_style_t *self, const lh_gl_style_t *pressed)
{
    lh_gl_style_pointer_set_pressed(lh_gl_style_get_pointer_mut(self), pressed);
}

lh_u16_t
lh_gl_style_get_transition(const lh_gl_style_t *self)
{
    return lh_gl_style_motion_get_transition(lh_gl_style_get_motion(self));
}

lh_void
lh_gl_style_set_transition(lh_gl_style_t *self, lh_u16_t ms)
{
    lh_gl_style_motion_set_transition(lh_gl_style_get_motion_mut(self), ms);
}

lh_gl_easing_t
lh_gl_style_get_easing(const lh_gl_style_t *self)
{
    return lh_gl_style_motion_get_easing(lh_gl_style_get_motion(self));
}

lh_void
lh_gl_style_set_easing(lh_gl_style_t *self, lh_gl_easing_t easing)
{
    lh_gl_style_motion_set_easing(lh_gl_style_get_motion_mut(self), easing);
}

lh_void
lh_gl_style_assign_fill(lh_gl_style_t *self, const lh_gl_paint_t *fill)
{
    lh_gl_paint_init_copy(lh_gl_style_get_fill_mut(self), fill);
}

const lh_gl_color_t *
lh_gl_style_get_fill_color(const lh_gl_style_t *self)
{
    return lh_gl_paint_get_color_v(lh_gl_style_get_fill(self));
}

lh_gl_scalar_t
lh_gl_style_get_radius(const lh_gl_style_t *self)
{
    return lh_gl_radii_get_min(lh_gl_style_get_radii(self));
}

lh_void
lh_gl_style_set_radius_v(lh_gl_style_t *self, lh_gl_scalar_t radius)
{
    lh_assert_runtime_if(radius < lh_gl_scalar(0), lh_runtime_error_code_invalid_argument);
    lh_gl_radii_init_all(lh_gl_style_get_radii_mut(self), radius);
}

lh_void
lh_gl_style_set_radii_v(lh_gl_style_t *self, const lh_gl_radii_t *radii)
{
    lh_assert_runtime_ref(radii);
    lh_assert_runtime_if(lh_gl_radii_get_min(radii) < lh_gl_scalar(0),
                         lh_runtime_error_code_invalid_argument);
    lh_gl_style_set_radii(self, lh_ptr_deref(radii));
}

lh_void
lh_gl_style_set_hit_radius_v(lh_gl_style_t *self, lh_gl_scalar_t radius)
{
    lh_assert_runtime_if(radius < lh_gl_scalar(0), lh_runtime_error_code_invalid_argument);
    lh_gl_style_set_hit_radius(self, radius);
}

lh_void
lh_gl_style_set_padding_v(lh_gl_style_t *self, const lh_gl_insets_t *padding)
{
    lh_assert_runtime_ref(padding);
    lh_assert_runtime_if(lh_gl_insets_get_left(padding) < lh_gl_scalar(0) ||
                             lh_gl_insets_get_top(padding) < lh_gl_scalar(0) ||
                             lh_gl_insets_get_right(padding) < lh_gl_scalar(0) ||
                             lh_gl_insets_get_bottom(padding) < lh_gl_scalar(0),
                         lh_runtime_error_code_invalid_argument);
    lh_gl_style_set_padding(self, lh_ptr_deref(padding));
}

lh_void
lh_gl_style_assign_text(lh_gl_style_t *self, const lh_gl_paint_t *text)
{
    lh_gl_paint_init_copy(lh_gl_style_get_text_mut(self), text);
}

const lh_gl_color_t *
lh_gl_style_get_text_color(const lh_gl_style_t *self)
{
    return lh_gl_paint_get_color_v(lh_gl_style_get_text(self));
}

lh_void
lh_gl_style_set_shadow_v(lh_gl_style_t *self, const lh_gl_shadow_t *shadow)
{
    if (lh_null_eq(shadow))
    {
        lh_gl_shadow_init(lh_gl_style_get_shadow_mut(self));
        return;
    }
    lh_gl_style_set_shadow(self, lh_ptr_deref(shadow));
}

lh_void
lh_gl_style_set_border_v(lh_gl_style_t *self, const lh_gl_insets_t *border)
{
    if (lh_null_eq(border))
    {
        lh_gl_insets_init_all(lh_gl_style_get_border_mut(self), lh_gl_scalar(0));
        return;
    }
    lh_assert_runtime_if(lh_gl_insets_get_left(border) < lh_gl_scalar(0) ||
                             lh_gl_insets_get_top(border) < lh_gl_scalar(0) ||
                             lh_gl_insets_get_right(border) < lh_gl_scalar(0) ||
                             lh_gl_insets_get_bottom(border) < lh_gl_scalar(0),
                         lh_runtime_error_code_invalid_argument);
    lh_gl_style_set_border(self, lh_ptr_deref(border));
}

lh_void
lh_gl_style_assign_border_fill(lh_gl_style_t *self, const lh_gl_paint_t *fill)
{
    lh_gl_paint_init_copy(lh_gl_style_get_border_fill_mut(self), fill);
}

lh_void
lh_gl_style_set_padding_all(lh_gl_style_t *self, lh_gl_scalar_t padding)
{
    lh_gl_insets_t all;

    lh_gl_insets_init_all(lh_addr_of(all), padding);
    lh_gl_style_set_padding_v(self, lh_addr_of(all));
}

lh_void
lh_gl_style_init_blend(lh_gl_style_t *self, const lh_gl_style_t *from, const lh_gl_style_t *to,
                       lh_u32_t weight)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(from);
    lh_assert_runtime_ref(to);
    lh_ptr_deref(self) = lh_ptr_deref(to);
    lh_return_if(weight >= 255U);
    lh_gl_style_mix_paint(lh_gl_style_get_fill_mut(self), lh_gl_style_get_fill(from),
                          lh_gl_style_get_fill(to), weight);
    lh_gl_style_mix_paint(lh_gl_style_get_text_mut(self), lh_gl_style_get_text(from),
                          lh_gl_style_get_text(to), weight);
    lh_gl_style_mix_paint(lh_gl_style_get_border_fill_mut(self), lh_gl_style_get_border_fill(from),
                          lh_gl_style_get_border_fill(to), weight);
    lh_gl_radii_t radii;

    lh_gl_style_mix_radii(lh_addr_of(radii), lh_gl_style_get_radii(from), lh_gl_style_get_radii(to),
                          weight);
    lh_gl_style_set_radii_v(self, lh_addr_of(radii));
    lh_gl_style_mix_shadow(lh_gl_style_get_shadow_mut(self), lh_gl_style_get_shadow(from),
                           lh_gl_style_get_shadow(to), weight);
}
