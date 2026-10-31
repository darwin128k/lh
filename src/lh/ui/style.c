/**
 * @file style.c
 * @brief Implementation of `lh/ui/style.h`.
 */
#include <lh/assert.h>

#include <lh/ui/style.h>
#include <lh/util/return.h>

lh_void
lh_ui_style_init(lh_ui_style_t *self)
{
    lh_ui_color_t black;

    lh_ui_color_init(lh_addr_of(black), 0, 0, 0, 255);
    lh_ui_paint_init(lh_ui_style_get_fill_mut(self));
    lh_ui_style_set_radius(self, lh_ui_scalar(0));
    lh_ui_radii_init_all(lh_ui_style_get_radii_mut(self), lh_ui_scalar(0));
    lh_ui_style_set_hit_radius(self, lh_ui_scalar(0));
    lh_ui_insets_init_all(lh_ui_style_get_padding_mut(self), lh_ui_scalar(0));
    lh_ui_style_set_font(self, lh_ui_font_get_default());
    lh_ui_paint_init_color(lh_ui_style_get_text_mut(self), lh_addr_of(black));
    /* Text starts at the top left of the padded box, which is what it did before
       alignment was a thing a style could say. */
    lh_ui_style_set_align_h(self, lh_ui_text_align_h_left);
    lh_ui_style_set_align_v(self, lh_ui_text_align_v_top);
    /* Drawn whole and clipped, which is what a label did before it could say more. */
    lh_ui_style_set_overflow(self, lh_ui_text_overflow_clip);
    lh_ui_style_set_wrap(self, lh_ui_text_wrap_none);
    lh_ui_shadow_init(lh_ui_style_get_shadow_mut(self));
    /* No edge until a caller asks for one. A style that grew a border and
       painted it on every existing widget would put a line on things that
       were square on purpose. */
    lh_ui_insets_init_all(lh_ui_style_get_border_mut(self), lh_ui_scalar(0));
    lh_ui_paint_init(lh_ui_style_get_border_fill_mut(self));
    lh_ui_style_set_pressed(self, lh_null);
    /* Every style reached by a switch changes at once unless a caller asks for
       a time of its own. */
    lh_ui_style_set_transition(self, 0);
    lh_ui_style_set_easing(self, lh_ui_easing_ease);
}

const lh_ui_paint_t *
lh_ui_style_get_fill(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->fill);
}

lh_void
lh_ui_style_assign_fill(lh_ui_style_t *self, const lh_ui_paint_t *fill)
{
    lh_ui_paint_init_copy(lh_ui_style_get_fill_mut(self), fill);
}

lh_u16_t
lh_ui_style_get_transition(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->transition;
}

lh_void
lh_ui_style_set_transition(lh_ui_style_t *self, lh_u16_t ms)
{
    lh_assert_runtime_ref(self);
    self->transition = ms;
}

lh_ui_easing_t
lh_ui_style_get_easing(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->easing;
}

lh_void
lh_ui_style_set_easing(lh_ui_style_t *self, lh_ui_easing_t easing)
{
    lh_assert_runtime_ref(self);
    self->easing = easing;
}

lh_ui_scalar_t
lh_ui_style_mix(lh_ui_scalar_t from, lh_ui_scalar_t to, lh_u32_t weight)
{
    return from + ((to - from) * lh_cast_static(lh_ui_scalar_t, weight)) / lh_ui_scalar(255);
}

lh_void
lh_ui_style_mix_color(const lh_ui_color_t *from, const lh_ui_color_t *to, lh_u32_t weight,
                      lh_ui_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_color_t result;

    lh_ui_color_init(lh_addr_of(result),
                     lh_ui_color_blend_channel(lh_ui_color_get_r(to), lh_ui_color_get_r(from),
                                               weight, 255U - weight),
                     lh_ui_color_blend_channel(lh_ui_color_get_g(to), lh_ui_color_get_g(from),
                                               weight, 255U - weight),
                     lh_ui_color_blend_channel(lh_ui_color_get_b(to), lh_ui_color_get_b(from),
                                               weight, 255U - weight),
                     lh_ui_color_blend_channel(lh_ui_color_get_a(to), lh_ui_color_get_a(from),
                                               weight, 255U - weight));
    lh_ptr_deref(out) = result;
}

lh_void
lh_ui_style_mix_paint(lh_ui_paint_t *out, const lh_ui_paint_t *from, const lh_ui_paint_t *to,
                      lh_u32_t weight)
{
    const lh_ui_color_t *a = lh_ui_paint_get_color_v(from);
    const lh_ui_color_t *b = lh_ui_paint_get_color_v(to);
    lh_ui_color_t mixed;

    lh_ui_paint_init_copy(out, to);
    lh_return_if(lh_null_eq(a) || lh_null_eq(b) || weight >= 255U);
    lh_ui_style_mix_color(a, b, weight, lh_addr_of(mixed));
    lh_ui_paint_init_color(out, lh_addr_of(mixed));
}

const lh_ui_color_t *
lh_ui_style_get_fill_color(const lh_ui_style_t *self)
{
    return lh_ui_paint_get_color_v(lh_ui_style_get_fill(self));
}

lh_ui_scalar_t
lh_ui_style_get_radius(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_ui_style_set_radius_v(lh_ui_style_t *self, lh_ui_scalar_t radius)
{
    lh_assert_runtime_if(radius < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    lh_ui_style_set_radius(self, radius);
    lh_ui_radii_init_all(lh_ui_style_get_radii_mut(self), radius);
}

const lh_ui_radii_t *
lh_ui_style_get_radii(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->radii);
}

lh_void
lh_ui_style_set_radii_v(lh_ui_style_t *self, const lh_ui_radii_t *radii)
{
    lh_assert_runtime_ref(radii);
    lh_assert_runtime_if(lh_ui_radii_get_min(radii) < lh_ui_scalar(0),
                         lh_runtime_error_code_invalid_argument);
    lh_ui_style_set_radii(self, lh_ptr_deref(radii));
    lh_ui_style_set_radius(self, lh_ui_radii_get_min(radii));
}

lh_void
lh_ui_style_init_blend(lh_ui_style_t *self, const lh_ui_style_t *from, const lh_ui_style_t *to,
                       lh_u32_t weight)
{

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(from);
    lh_assert_runtime_ref(to);
    lh_ptr_deref(self) = lh_ptr_deref(to);
    lh_return_if(weight >= 255U);
    lh_ui_style_mix_paint(lh_ui_style_get_fill_mut(self), lh_ui_style_get_fill(from),
                          lh_ui_style_get_fill(to), weight);
    lh_ui_style_mix_paint(lh_ui_style_get_text_mut(self), lh_ui_style_get_text(from),
                          lh_ui_style_get_text(to), weight);
    lh_ui_style_mix_paint(lh_ui_style_get_border_fill_mut(self), lh_ui_style_get_border_fill(from),
                          lh_ui_style_get_border_fill(to), weight);
    lh_ui_radii_t radii;
    lh_ui_radii_init(
        lh_addr_of(radii),
        lh_ui_style_mix(lh_ui_radii_get_top_left(lh_ui_style_get_radii(from)),
                        lh_ui_radii_get_top_left(lh_ui_style_get_radii(to)), weight),
        lh_ui_style_mix(lh_ui_radii_get_top_right(lh_ui_style_get_radii(from)),
                        lh_ui_radii_get_top_right(lh_ui_style_get_radii(to)), weight),
        lh_ui_style_mix(lh_ui_radii_get_bottom_right(lh_ui_style_get_radii(from)),
                        lh_ui_radii_get_bottom_right(lh_ui_style_get_radii(to)), weight),
        lh_ui_style_mix(lh_ui_radii_get_bottom_left(lh_ui_style_get_radii(from)),
                        lh_ui_radii_get_bottom_left(lh_ui_style_get_radii(to)), weight));
    lh_ui_style_set_radii_v(self, lh_addr_of(radii));
    const lh_ui_shadow_t *fade_from = lh_ui_style_get_shadow(from);
    const lh_ui_shadow_t *fade_to = lh_ui_style_get_shadow(to);
    if (!lh_ui_shadow_is_empty(fade_from) || !lh_ui_shadow_is_empty(fade_to))
    {
        /* A shadow at one end only grows out of, or melts into, the other end's
           nothing: the same shadow with no colour. */
        const lh_ui_shadow_t *shape = lh_ui_shadow_is_empty(fade_to) ? fade_from : fade_to;
        lh_ui_color_t clear;
        lh_ui_shadow_copy_color(shape, lh_addr_of(clear));
        lh_ui_color_t a;
        lh_ui_color_t b;

        lh_ui_color_set_a(lh_addr_of(clear), 0);
        if (lh_ui_shadow_is_empty(fade_from))
        {
            a = clear;
        }
        else
        {
            lh_ui_shadow_copy_color(fade_from, lh_addr_of(a));
        }
        if (lh_ui_shadow_is_empty(fade_to))
        {
            b = clear;
        }
        else
        {
            lh_ui_shadow_copy_color(fade_to, lh_addr_of(b));
        }
        lh_ui_style_set_shadow(self, lh_ptr_deref(shape));
        lh_ui_color_t color;
        lh_ui_style_mix_color(lh_addr_of(a), lh_addr_of(b), weight, lh_addr_of(color));
        lh_ui_shadow_set_color(lh_ui_style_get_shadow_mut(self), color);
        if (!lh_ui_shadow_is_empty(fade_from) && !lh_ui_shadow_is_empty(fade_to))
        {
            lh_ui_shadow_set_spread_v(lh_ui_style_get_shadow_mut(self),
                                      lh_ui_style_mix(lh_ui_shadow_get_spread(fade_from),
                                                      lh_ui_shadow_get_spread(fade_to), weight));
            lh_ui_shadow_set_offset(lh_ui_style_get_shadow_mut(self),
                                    lh_ui_style_mix(lh_ui_shadow_get_offset_x(fade_from),
                                                    lh_ui_shadow_get_offset_x(fade_to), weight),
                                    lh_ui_style_mix(lh_ui_shadow_get_offset_y(fade_from),
                                                    lh_ui_shadow_get_offset_y(fade_to), weight));
        }
    }
}

lh_ui_scalar_t
lh_ui_style_get_hit_radius(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hit_radius;
}

lh_void
lh_ui_style_set_hit_radius_v(lh_ui_style_t *self, lh_ui_scalar_t radius)
{
    lh_assert_runtime_if(radius < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    lh_ui_style_set_hit_radius(self, radius);
}

const lh_ui_insets_t *
lh_ui_style_get_padding(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->padding);
}

lh_void
lh_ui_style_set_padding_v(lh_ui_style_t *self, const lh_ui_insets_t *padding)
{
    lh_assert_runtime_ref(padding);
    lh_assert_runtime_if(lh_ui_insets_get_left(padding) < lh_ui_scalar(0) ||
                             lh_ui_insets_get_top(padding) < lh_ui_scalar(0) ||
                             lh_ui_insets_get_right(padding) < lh_ui_scalar(0) ||
                             lh_ui_insets_get_bottom(padding) < lh_ui_scalar(0),
                         lh_runtime_error_code_invalid_argument);
    lh_ui_style_set_padding(self, lh_ptr_deref(padding));
}

lh_void
lh_ui_style_set_padding_all(lh_ui_style_t *self, lh_ui_scalar_t padding)
{
    lh_ui_insets_t all;

    lh_ui_insets_init_all(lh_addr_of(all), padding);
    lh_ui_style_set_padding_v(self, lh_addr_of(all));
}

const lh_ui_font_t *
lh_ui_style_get_font(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->font;
}

lh_void
lh_ui_style_set_font(lh_ui_style_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
}

const lh_ui_paint_t *
lh_ui_style_get_text(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_void
lh_ui_style_assign_text(lh_ui_style_t *self, const lh_ui_paint_t *text)
{
    lh_ui_paint_init_copy(lh_ui_style_get_text_mut(self), text);
}

const lh_ui_color_t *
lh_ui_style_get_text_color(const lh_ui_style_t *self)
{
    return lh_ui_paint_get_color_v(lh_ui_style_get_text(self));
}

lh_ui_text_align_h_t
lh_ui_style_get_align_h(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_h;
}

lh_void
lh_ui_style_set_align_h(lh_ui_style_t *self, lh_ui_text_align_h_t align)
{
    lh_assert_runtime_ref(self);
    self->align_h = align;
}

lh_ui_text_align_v_t
lh_ui_style_get_align_v(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_v;
}

lh_void
lh_ui_style_set_align_v(lh_ui_style_t *self, lh_ui_text_align_v_t align)
{
    lh_assert_runtime_ref(self);
    self->align_v = align;
}

lh_ui_text_overflow_t
lh_ui_style_get_overflow(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->overflow;
}

lh_void
lh_ui_style_set_overflow(lh_ui_style_t *self, lh_ui_text_overflow_t overflow)
{
    lh_assert_runtime_ref(self);
    self->overflow = overflow;
}

lh_ui_text_wrap_t
lh_ui_style_get_wrap(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wrap;
}

lh_void
lh_ui_style_set_wrap(lh_ui_style_t *self, lh_ui_text_wrap_t wrap)
{
    lh_assert_runtime_ref(self);
    self->wrap = wrap;
}

const lh_ui_shadow_t *
lh_ui_style_get_shadow(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->shadow);
}

lh_void
lh_ui_style_set_shadow_v(lh_ui_style_t *self, const lh_ui_shadow_t *shadow)
{
    if (lh_null_eq(shadow))
    {
        lh_ui_shadow_init(lh_ui_style_get_shadow_mut(self));
        return;
    }
    lh_ui_style_set_shadow(self, lh_ptr_deref(shadow));
}

const lh_ui_style_t *
lh_ui_style_get_pressed(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

lh_void
lh_ui_style_set_pressed(lh_ui_style_t *self, const lh_ui_style_t *pressed)
{
    lh_assert_runtime_ref(self);
    self->pressed = pressed;
}

const lh_ui_insets_t *
lh_ui_style_get_border(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->border);
}

lh_void
lh_ui_style_set_border_v(lh_ui_style_t *self, const lh_ui_insets_t *border)
{
    if (lh_null_eq(border))
    {
        lh_ui_insets_init_all(lh_ui_style_get_border_mut(self), lh_ui_scalar(0));
        return;
    }
    lh_assert_runtime_if(lh_ui_insets_get_left(border) < lh_ui_scalar(0) ||
                             lh_ui_insets_get_top(border) < lh_ui_scalar(0) ||
                             lh_ui_insets_get_right(border) < lh_ui_scalar(0) ||
                             lh_ui_insets_get_bottom(border) < lh_ui_scalar(0),
                         lh_runtime_error_code_invalid_argument);
    lh_ui_style_set_border(self, lh_ptr_deref(border));
}

const lh_ui_paint_t *
lh_ui_style_get_border_fill(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->border_fill);
}

lh_void
lh_ui_style_assign_border_fill(lh_ui_style_t *self, const lh_ui_paint_t *fill)
{
    lh_ui_paint_init_copy(lh_ui_style_get_border_fill_mut(self), fill);
}

lh_ui_paint_t *
lh_ui_style_get_fill_mut(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->fill);
}

lh_ui_radii_t *
lh_ui_style_get_radii_mut(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->radii);
}

lh_ui_insets_t *
lh_ui_style_get_padding_mut(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->padding);
}

lh_ui_paint_t *
lh_ui_style_get_text_mut(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_ui_shadow_t *
lh_ui_style_get_shadow_mut(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->shadow);
}

lh_ui_insets_t *
lh_ui_style_get_border_mut(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->border);
}

lh_ui_paint_t *
lh_ui_style_get_border_fill_mut(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->border_fill);
}

lh_void
lh_ui_style_set_fill(lh_ui_style_t *self, lh_ui_paint_t fill)
{
    lh_assert_runtime_ref(self);
    self->fill = fill;
}

lh_void
lh_ui_style_set_radii(lh_ui_style_t *self, lh_ui_radii_t radii)
{
    lh_assert_runtime_ref(self);
    self->radii = radii;
}

lh_void
lh_ui_style_set_padding(lh_ui_style_t *self, lh_ui_insets_t padding)
{
    lh_assert_runtime_ref(self);
    self->padding = padding;
}

lh_void
lh_ui_style_set_text(lh_ui_style_t *self, lh_ui_paint_t text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

lh_void
lh_ui_style_set_shadow(lh_ui_style_t *self, lh_ui_shadow_t shadow)
{
    lh_assert_runtime_ref(self);
    self->shadow = shadow;
}

lh_void
lh_ui_style_set_border(lh_ui_style_t *self, lh_ui_insets_t border)
{
    lh_assert_runtime_ref(self);
    self->border = border;
}

lh_void
lh_ui_style_set_border_fill(lh_ui_style_t *self, lh_ui_paint_t border_fill)
{
    lh_assert_runtime_ref(self);
    self->border_fill = border_fill;
}

lh_void
lh_ui_style_set_radius(lh_ui_style_t *self, lh_ui_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius;
}

lh_void
lh_ui_style_set_hit_radius(lh_ui_style_t *self, lh_ui_scalar_t hit_radius)
{
    lh_assert_runtime_ref(self);
    self->hit_radius = hit_radius;
}
