#include <lh/entity/list.h>
#include <lh/assert.h>
#include <lh/entity/group.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_list_construct(lh_entity_t *self)
{
    lh_entity_list_t *const list = lh_ptr_rcast(lh_entity_list_t, self);
    list->font = lh_addr_of(lh_ui_font_basic);
    list->mode = LH_ENTITY_GROUP_ONE;
    list->row = 22;
}

lh_void
lh_entity_list_pick(lh_entity_list_t *self, lh_int_t index)
{
    lh_int_t i;
    if (index < 0 || index >= self->count)
    {
        return;
    }
    if (self->mode == LH_ENTITY_GROUP_MANY)
    {
        self->marks[index] = self->marks[index] ? 0 : 1;
    }
    else
    {
        for (i = 0; i < self->count; ++i)
        {
            self->marks[i] = i == index ? 1 : 0;
        }
    }
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_void
lh_entity_list_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_list_t *const list = lh_ptr_rcast(lh_entity_list_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    const lh_ui_style_t *style;
    lh_ui_canvas_t *canvas;
    lh_math_rect_t bounds;
    lh_int_t width;
    lh_int_t height;
    lh_int_t y;
    lh_int_t index;
    if (code == LH_ENTITY_EVENT_POINTER_UP)
    {
        const lh_math_vec2_t *const point =
            lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
        if (lh_ptr_is_null(point) ||
            !lh_entity_2d_contains(lh_ptr_rcast(const lh_entity_2d_t, self), *point) ||
            list->row <= 0)
        {
            return;
        }
        bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
        index = list->origin +
                ((lh_int_t)lh_math_vec2_get_y(point) - lh_math_rect_get_y(lh_addr_of(bounds))) /
                    list->row;
        lh_entity_list_pick(list, index);
        lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED, lh_entity_event_get_param(event));
        return;
    }
    if (code != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    style = lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    canvas = lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event));
    if (lh_ptr_is_null(style) || lh_ptr_is_null(list->font) || list->row <= 0)
    {
        return;
    }
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    width = lh_math_rect_get_size_width(lh_addr_of(bounds));
    height = lh_math_rect_get_size_height(lh_addr_of(bounds));
    lh_ui_canvas_fill_rect(canvas, bounds, lh_ui_style_get_bg_color(style));
    for (y = 0; y + list->row <= height; y += list->row)
    {
        const lh_int_t row = list->origin + y / list->row;
        const lh_int_t top = lh_math_rect_get_y(lh_addr_of(bounds)) + y;
        if (row < 0 || row >= list->count || lh_ptr_is_null(list->items[row]))
        {
            continue;
        }
        if (list->marks[row])
        {
            const lh_uint_t argb = lh_ui_color_to_argb(lh_ui_style_get_text_color(style));
            lh_ui_canvas_fill_rect(canvas,
                                   lh_math_rect_make(lh_math_rect_get_x(lh_addr_of(bounds)), top,
                                                     width, list->row),
                                   lh_ui_color_from_argb((48U << 24) | (argb & 0x00FFFFFFU)));
        }
        lh_ui_font_draw(list->font, canvas, lh_math_rect_get_x(lh_addr_of(bounds)) + 6, top + 2,
                        list->items[row], lh_ui_style_get_text_color(style));
    }
}

const lh_entity_class_t lh_entity_list_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_list_t),
                                lh_entity_list_construct, lh_null, lh_entity_list_on_event);

lh_int_t
lh_entity_list_get_count(const lh_entity_list_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_entity_list_set_item(lh_entity_list_t *self, lh_int_t index, const lh_char_t *text)
{
    lh_int_t i;
    lh_assert_runtime_ref(self);
    if (index < 0 || index >= LH_ENTITY_LIST_LIMIT)
    {
        return;
    }
    if (lh_ptr_is_null(text))
    {
        for (i = index; i + 1 < self->count; ++i)
        {
            self->items[i] = self->items[i + 1];
            self->marks[i] = self->marks[i + 1];
        }
        if (self->count > index)
        {
            self->count -= 1;
            self->items[self->count] = lh_null;
            self->marks[self->count] = 0;
        }
    }
    else
    {
        self->items[index] = text;
        if (index >= self->count)
        {
            self->count = index + 1;
        }
    }
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

const lh_char_t *
lh_entity_list_get_item(const lh_entity_list_t *self, lh_int_t index)
{
    lh_assert_runtime_ref(self);
    if (index < 0 || index >= self->count)
    {
        return lh_null;
    }
    return self->items[index];
}

lh_bool_t
lh_entity_list_is_on(const lh_entity_list_t *self, lh_int_t index)
{
    lh_assert_runtime_ref(self);
    if (index < 0 || index >= self->count)
    {
        return lh_bool_false;
    }
    return self->marks[index] ? lh_bool_true : lh_bool_false;
}

lh_void
lh_entity_list_set_mode(lh_entity_list_t *self, lh_int_t mode)
{
    lh_assert_runtime_ref(self);
    self->mode = mode == LH_ENTITY_GROUP_MANY ? LH_ENTITY_GROUP_MANY : LH_ENTITY_GROUP_ONE;
}

lh_void
lh_entity_list_set_origin(lh_entity_list_t *self, lh_int_t origin)
{
    lh_assert_runtime_ref(self);
    self->origin = origin < 0 ? 0 : origin;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_void
lh_entity_list_set_font(lh_entity_list_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_int_t
lh_entity_list_get_row(const lh_entity_list_t *self)
{
    lh_assert_runtime_ref(self);
    return self->row;
}
