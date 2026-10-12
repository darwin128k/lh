/**
 * @file style.c
 * @brief Implementation of `lh/ui/style.h`.
 */

#include <lh/void.h>
#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/insets.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

lh_void
lh_ui_style_init(lh_ui_style_t *self)
{
    lh_ui_color_t black;

    lh_assert_runtime_ref(self);
    lh_ui_color_init(lh_addr_of(black), 0, 0, 0, 255);
    lh_ui_paint_init(lh_addr_of(self->fill));
    self->radius = lh_ui_scalar(0);
    lh_ui_radii_init_all(lh_addr_of(self->radii), lh_ui_scalar(0));
    self->hit_radius = lh_ui_scalar(0);
    lh_ui_insets_init_all(lh_addr_of(self->padding), lh_ui_scalar(0));
    self->font = lh_ui_font_get_default();
    lh_ui_paint_init_color(lh_addr_of(self->text), lh_addr_of(black));
    /* Text starts at the top left of the padded box, which is what it did before
       alignment was a thing a style could say. */
    self->align_h = lh_ui_text_align_h_left;
    self->align_v = lh_ui_text_align_v_top;
    /* Drawn whole and clipped, which is what a label did before it could say more. */
    self->overflow = lh_ui_text_overflow_clip;
    self->wrap = lh_ui_text_wrap_none;
    lh_ui_shadow_init(lh_addr_of(self->shadow));
    /* No edge until a caller asks for one. A style that grew a border and
       painted it on every existing widget would put a line on things that
       were square on purpose. */
    lh_ui_insets_init_all(lh_addr_of(self->border), lh_ui_scalar(0));
    lh_ui_paint_init(lh_addr_of(self->border_fill));
    self->pressed = lh_null;
    /* Every style reached by a switch changes at once unless a caller asks for
       a time of its own. */
    self->transition = 0;
    self->easing = lh_ui_easing_ease;
}

const lh_ui_paint_t *
lh_ui_style_get_fill(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->fill);
}

lh_void
lh_ui_style_set_fill(lh_ui_style_t *self, const lh_ui_paint_t *fill)
{
    lh_assert_runtime_ref(self);
    lh_ui_paint_init_copy(lh_addr_of(self->fill), fill);
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

/* @p from moved @p weight of the way to @p to, in weights of 255. Written so it means
   the same on both arithmetic paths: `to - from` first, so a step that runs the other
   way stays negative. */
static lh_ui_scalar_t
lh_ui_style_mix(lh_ui_scalar_t from, lh_ui_scalar_t to, lh_u32_t weight)
{
    return from + ((to - from) * (lh_ui_scalar_t)weight) / lh_ui_scalar(255);
}

/* @p from moved @p weight of the way to @p to, every channel and the alpha with them:
   a shadow fading in from nothing is a colour whose alpha grows. */
static lh_ui_color_t
lh_ui_style_mix_color(const lh_ui_color_t *from, const lh_ui_color_t *to, lh_u32_t weight)
{
    lh_ui_color_t out;

    lh_ui_color_init(lh_addr_of(out),
                     lh_ui_color_blend_channel(lh_ui_color_get_r(to), lh_ui_color_get_r(from), weight, 255U - weight),
                     lh_ui_color_blend_channel(lh_ui_color_get_g(to), lh_ui_color_get_g(from), weight, 255U - weight),
                     lh_ui_color_blend_channel(lh_ui_color_get_b(to), lh_ui_color_get_b(from), weight, 255U - weight),
                     lh_ui_color_blend_channel(lh_ui_color_get_a(to), lh_ui_color_get_a(from), weight, 255U - weight));
    return out;
}

/* @p to's paint, or -- when both ends are one colour -- the colour between them. */
static lh_void
lh_ui_style_mix_paint(lh_ui_paint_t *out, const lh_ui_paint_t *from, const lh_ui_paint_t *to, lh_u32_t weight)
{
    const lh_ui_color_t *a = lh_ui_paint_get_color(from);
    const lh_ui_color_t *b = lh_ui_paint_get_color(to);
    lh_ui_color_t mixed;

    lh_ui_paint_init_copy(out, to);
    lh_return_if(lh_null_eq(a) || lh_null_eq(b) || weight >= 255U);
    mixed = lh_ui_style_mix_color(a, b, weight);
    lh_ui_paint_init_color(out, lh_addr_of(mixed));
}

lh_void
lh_ui_style_init_blend(lh_ui_style_t *self, const lh_ui_style_t *from, const lh_ui_style_t *to,
                       lh_u32_t weight)
{
    const lh_ui_shadow_t *fade_from;
    const lh_ui_shadow_t *fade_to;
    lh_ui_radii_t radii;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(from);
    lh_assert_runtime_ref(to);
    *self = *to;
    lh_return_if(weight >= 255U);
    lh_ui_style_mix_paint(lh_addr_of(self->fill), lh_addr_of(from->fill), lh_addr_of(to->fill), weight);
    lh_ui_style_mix_paint(lh_addr_of(self->text), lh_addr_of(from->text), lh_addr_of(to->text), weight);
    lh_ui_style_mix_paint(lh_addr_of(self->border_fill), lh_addr_of(from->border_fill), lh_addr_of(to->border_fill),
                          weight);
    lh_ui_radii_init(lh_addr_of(radii), lh_ui_style_mix(from->radii.top_left, to->radii.top_left, weight),
                     lh_ui_style_mix(from->radii.top_right, to->radii.top_right, weight),
                     lh_ui_style_mix(from->radii.bottom_right, to->radii.bottom_right, weight),
                     lh_ui_style_mix(from->radii.bottom_left, to->radii.bottom_left, weight));
    lh_ui_style_set_radii(self, lh_addr_of(radii));
    fade_from = lh_addr_of(from->shadow);
    fade_to = lh_addr_of(to->shadow);
    if (!lh_ui_shadow_is_empty(fade_from) || !lh_ui_shadow_is_empty(fade_to))
    {
        /* A shadow at one end only grows out of, or melts into, the other end's
           nothing: the same shadow with no colour. */
        const lh_ui_shadow_t *shape = lh_ui_shadow_is_empty(fade_to) ? fade_from : fade_to;
        lh_ui_color_t clear = lh_ui_shadow_get_color(shape);
        lh_ui_color_t a;
        lh_ui_color_t b;

        lh_ui_color_set_a(lh_addr_of(clear), 0);
        a = lh_ui_shadow_is_empty(fade_from) ? clear : lh_ui_shadow_get_color(fade_from);
        b = lh_ui_shadow_is_empty(fade_to) ? clear : lh_ui_shadow_get_color(fade_to);
        self->shadow = *shape;
        lh_ui_shadow_set_color(lh_addr_of(self->shadow), lh_ui_style_mix_color(lh_addr_of(a), lh_addr_of(b), weight));
        if (!lh_ui_shadow_is_empty(fade_from) && !lh_ui_shadow_is_empty(fade_to))
        {
            lh_ui_shadow_set_spread(lh_addr_of(self->shadow),
                                    lh_ui_style_mix(lh_ui_shadow_get_spread(fade_from),
                                                    lh_ui_shadow_get_spread(fade_to), weight));
            lh_ui_shadow_set_offset(lh_addr_of(self->shadow),
                                    lh_ui_style_mix(lh_ui_shadow_get_offset_x(fade_from),
                                                    lh_ui_shadow_get_offset_x(fade_to), weight),
                                    lh_ui_style_mix(lh_ui_shadow_get_offset_y(fade_from),
                                                    lh_ui_shadow_get_offset_y(fade_to), weight));
        }
    }
}

const lh_ui_color_t *
lh_ui_style_get_fill_color(const lh_ui_style_t *self)
{
    return lh_ui_paint_get_color(lh_ui_style_get_fill(self));
}

lh_ui_scalar_t
lh_ui_style_get_radius(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_ui_style_set_radius(lh_ui_style_t *self, lh_ui_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(radius < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    self->radius = radius;
    lh_ui_radii_init_all(lh_addr_of(self->radii), radius);
}

const lh_ui_radii_t *
lh_ui_style_get_radii(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->radii);
}

lh_void
lh_ui_style_set_radii(lh_ui_style_t *self, const lh_ui_radii_t *radii)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(radii);
    lh_assert_runtime_if(lh_ui_radii_get_min(radii) < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    self->radii = *radii;
    self->radius = lh_ui_radii_get_min(radii);
}

lh_ui_scalar_t
lh_ui_style_get_hit_radius(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hit_radius;
}

lh_void
lh_ui_style_set_hit_radius(lh_ui_style_t *self, lh_ui_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(radius < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    self->hit_radius = radius;
}

const lh_ui_insets_t *
lh_ui_style_get_padding(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->padding);
}

lh_void
lh_ui_style_set_padding(lh_ui_style_t *self, lh_ui_scalar_t padding)
{
    lh_ui_insets_t all;

    lh_ui_insets_init_all(lh_addr_of(all), padding);
    lh_ui_style_set_padding_insets(self, lh_addr_of(all));
}

lh_void
lh_ui_style_set_padding_insets(lh_ui_style_t *self, const lh_ui_insets_t *padding)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(padding);
    lh_assert_runtime_if(padding->left < lh_ui_scalar(0) || padding->top < lh_ui_scalar(0) ||
                             padding->right < lh_ui_scalar(0) || padding->bottom < lh_ui_scalar(0),
                         lh_runtime_error_code_invalid_argument);
    self->padding = *padding;
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
lh_ui_style_set_text(lh_ui_style_t *self, const lh_ui_paint_t *text)
{
    lh_assert_runtime_ref(self);
    lh_ui_paint_init_copy(lh_addr_of(self->text), text);
}

const lh_ui_color_t *
lh_ui_style_get_text_color(const lh_ui_style_t *self)
{
    return lh_ui_paint_get_color(lh_ui_style_get_text(self));
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
lh_ui_style_set_shadow(lh_ui_style_t *self, const lh_ui_shadow_t *shadow)
{
    lh_assert_runtime_ref(self);
    if (lh_null_eq(shadow))
    {
        lh_ui_shadow_init(lh_addr_of(self->shadow));
        return;
    }
    self->shadow = *shadow;
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
lh_ui_style_set_border(lh_ui_style_t *self, const lh_ui_insets_t *border)
{
    lh_assert_runtime_ref(self);
    if (lh_null_eq(border))
    {
        lh_ui_insets_init_all(lh_addr_of(self->border), lh_ui_scalar(0));
        return;
    }
    lh_assert_runtime_if(border->left < lh_ui_scalar(0) || border->top < lh_ui_scalar(0) ||
                             border->right < lh_ui_scalar(0) || border->bottom < lh_ui_scalar(0),
                         lh_runtime_error_code_invalid_argument);
    self->border = *border;
}

const lh_ui_paint_t *
lh_ui_style_get_border_fill(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->border_fill);
}

lh_void
lh_ui_style_set_border_fill(lh_ui_style_t *self, const lh_ui_paint_t *fill)
{
    lh_assert_runtime_ref(self);
    lh_ui_paint_init_copy(lh_addr_of(self->border_fill), fill);
}
