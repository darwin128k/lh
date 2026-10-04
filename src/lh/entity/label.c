#include <lh/entity/label.h>
#include <lh/assert.h>
#include <lh/bit.h>
#include <lh/cast/static.h>
#include <lh/entity.h>
#include <lh/entity/screen.h>
#include <lh/float/round.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_label_fit(lh_entity_label_t *self)
{
    const lh_math_rect_t ink =
        lh_ptr_is_set(self->font) ? lh_ui_font_ink(self->font, self->text) : lh_math_rect_make_empty();
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self),
                          lh_math_vec2_make(lh_cast_static(lh_float_t,
                                                           lh_math_rect_get_size_width(lh_addr_of(ink))),
                                            lh_cast_static(lh_float_t,
                                                           lh_math_rect_get_size_height(lh_addr_of(ink)))));
}

lh_int_t
lh_entity_label_shift(lh_int_t align, lh_int_t room)
{
    if (align == LH_ENTITY_LABEL_END)
    {
        return room;
    }
    if (align == LH_ENTITY_LABEL_CENTER)
    {
        return room / 2;
    }
    return 0;
}

const lh_entity_2d_t *
lh_entity_label_host(const lh_entity_label_t *self)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_math_vec2_t place = lh_entity_2d_get_position(box);
    const lh_entity_t *parent;
    lh_int_t count;
    if (lh_math_vec2_get_x(lh_addr_of(place)) != 0.0f ||
        lh_math_vec2_get_y(lh_addr_of(place)) != 0.0f)
    {
        return lh_null;
    }
    parent = lh_entity_get_parent(lh_ptr_rcast(const lh_entity_t, self));
    if (lh_ptr_is_null(parent))
    {
        return lh_null;
    }
    count = 0;
    lh_entity_foreach_child(child, parent)
    {
        count += 1;
        if (count > 1)
        {
            return lh_null;
        }
    }
    return lh_ptr_rcast(const lh_entity_2d_t, parent);
}

lh_math_rect_t
lh_entity_label_frame(const lh_entity_label_t *self)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_entity_2d_t *const host = lh_entity_label_host(self);
    const lh_math_rect_t own = lh_entity_2d_get_screen_bounds(box);
    lh_math_rect_t bounds;
    if (lh_ptr_is_null(host))
    {
        return own;
    }
    bounds = lh_entity_2d_get_screen_bounds(host);
    if (lh_math_rect_get_size_width(lh_addr_of(bounds)) <= 0 ||
        lh_math_rect_get_size_height(lh_addr_of(bounds)) <= 0)
    {
        return own;
    }
    return bounds;
}

lh_void
lh_entity_label_adopt(lh_entity_label_t *self)
{
    const lh_entity_2d_t *const host = lh_entity_label_host(self);
    lh_math_vec2_t have;
    lh_math_vec2_t want;
    if (lh_ptr_is_null(host))
    {
        return;
    }
    want = lh_entity_2d_get_size(host);
    if (lh_math_vec2_get_x(lh_addr_of(want)) <= 0.0f ||
        lh_math_vec2_get_y(lh_addr_of(want)) <= 0.0f)
    {
        return;
    }
    have = lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self));
    if (lh_math_vec2_get_x(lh_addr_of(have)) == lh_math_vec2_get_x(lh_addr_of(want)) &&
        lh_math_vec2_get_y(lh_addr_of(have)) == lh_math_vec2_get_y(lh_addr_of(want)))
    {
        return;
    }
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self), want);
}

lh_void
lh_entity_label_construct(lh_entity_t *self)
{
    lh_entity_label_t *const label = lh_ptr_rcast(lh_entity_label_t, self);
    label->font = lh_ui_font_get_default();
    label->align_x = LH_ENTITY_LABEL_CENTER;
    label->align_y = LH_ENTITY_LABEL_CENTER;
}

lh_void
lh_entity_label_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_label_t *const label = lh_ptr_rcast(lh_entity_label_t, self);
    const lh_ui_style_t *style;
    lh_ui_color_t color;
    lh_math_mat4_t world;
    lh_math_vec4_t origin;
    lh_math_rect_t clip;
    lh_math_rect_t bounds;
    lh_math_rect_t kept;

    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    lh_entity_label_adopt(label);
    style = lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    if (lh_ptr_is_null(style) || lh_ptr_is_null(label->font) || lh_ptr_is_null(label->text))
    {
        return;
    }
    color = lh_ui_style_get_text_color(style);
    if (lh_bit_shr(lh_ui_color_to_argb(color), 24) == 0U)
    {
        return;
    }

    world = lh_entity_2d_get_world_matrix(lh_ptr_rcast(const lh_entity_2d_t, self));
    origin = lh_math_mat4_get_column(lh_addr_of(world), 3);
    clip = lh_ui_canvas_get_clip(lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
    bounds = lh_entity_label_frame(label);
    kept = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(bounds));
    lh_ui_canvas_set_clip(lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)), kept);
    lh_ui_canvas_set_draw_z(lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)),
                            lh_math_vec4_get_z(lh_addr_of(origin)));
    {
        const lh_math_rect_t ink = lh_ui_font_ink(label->font, label->text);
        const lh_int_t room_x = lh_math_rect_get_size_width(lh_addr_of(bounds)) -
                                lh_math_rect_get_size_width(lh_addr_of(ink));
        const lh_int_t room_y = lh_math_rect_get_size_height(lh_addr_of(bounds)) -
                                lh_math_rect_get_size_height(lh_addr_of(ink));
        lh_ui_font_draw(label->font, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)),
                        lh_math_rect_get_x(lh_addr_of(bounds)) +
                            lh_entity_label_shift(label->align_x, room_x) -
                            lh_math_rect_get_x(lh_addr_of(ink)),
                        lh_math_rect_get_y(lh_addr_of(bounds)) +
                            lh_entity_label_shift(label->align_y, room_y) -
                            lh_math_rect_get_y(lh_addr_of(ink)),
                        label->text, color);
    }
    lh_ui_canvas_set_clip(lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)), clip);
}

const lh_entity_class_t lh_entity_label_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_label_t),
                                lh_entity_label_construct, lh_null, lh_entity_label_on_event);

const lh_ui_font_t *
lh_entity_label_get_font(const lh_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->font;
}

lh_void
lh_entity_label_set_font(lh_entity_label_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
    lh_entity_label_fit(self);
}

const lh_char_t *
lh_entity_label_get_text(const lh_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_entity_label_set_text(lh_entity_label_t *self, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
    lh_entity_label_fit(self);
}

lh_int_t
lh_entity_label_keep(lh_int_t align)
{
    if (align == LH_ENTITY_LABEL_CENTER || align == LH_ENTITY_LABEL_END)
    {
        return align;
    }
    return LH_ENTITY_LABEL_START;
}

lh_int_t
lh_entity_label_get_align_x(const lh_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_x;
}

lh_int_t
lh_entity_label_get_align_y(const lh_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_y;
}

lh_void
lh_entity_label_set_align(lh_entity_label_t *self, lh_int_t align_x, lh_int_t align_y)
{
    lh_assert_runtime_ref(self);
    self->align_x = lh_entity_label_keep(align_x);
    self->align_y = lh_entity_label_keep(align_y);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}
