#include <lh/entity/screen.h>
#include <lh/entity/flex.h>
#include <lh/assert.h>
#include <lh/ui/effect.h>
#include <lh/float/round.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* Shared default: opaque black. A screen points at it until given another. */
static const lh_ui_style_t lh_entity_screen_background = {{0, 0, 0, 255}};

static lh_void
lh_entity_screen_construct(lh_entity_t *self)
{
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, self), lh_addr_of(lh_entity_screen_background));
}

const lh_entity_class_t lh_entity_screen_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_screen_t),
                                lh_entity_screen_construct, lh_null, lh_null);

lh_void
lh_entity_screen_invalidate_area(lh_entity_screen_t *self, lh_math_rect_t area)
{
    lh_assert_runtime_ref(self);
    const lh_math_vec2_t size = lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_math_rect_t screen =
        lh_math_rect_make(0, 0, lh_float_ceil_to_int(lh_math_vec2_get_x(lh_addr_of(size))), lh_float_ceil_to_int(lh_math_vec2_get_y(lh_addr_of(size))));
    lh_math_rect_t dirty = lh_math_rect_intersection(lh_addr_of(screen), lh_addr_of(area));
    if (lh_math_rect_is_empty(lh_addr_of(dirty)))
    {
        return;
    }

    for (lh_usize_t i = 0; i < self->dirty_count; ++i)
    {
        if (lh_math_rect_intersects(lh_addr_of(self->dirty[i]), lh_addr_of(dirty)))
        {
            self->dirty[i] = lh_math_rect_union(lh_addr_of(self->dirty[i]), lh_addr_of(dirty));
            return;
        }
    }

    if (self->dirty_count == LH_ENTITY_SCREEN_DIRTY_MAX)
    {
        for (lh_usize_t i = 1; i < self->dirty_count; ++i)
        {
            self->dirty[0] =
                lh_math_rect_union(lh_addr_of(self->dirty[0]), lh_addr_of(self->dirty[i]));
        }
        self->dirty[0] = lh_math_rect_union(lh_addr_of(self->dirty[0]), lh_addr_of(dirty));
        self->dirty_count = 1;
        return;
    }
    self->dirty[self->dirty_count++] = dirty;
}

lh_usize_t
lh_entity_screen_get_dirty_count(const lh_entity_screen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dirty_count;
}

lh_math_rect_t
lh_entity_screen_get_dirty_area(const lh_entity_screen_t *self, lh_usize_t index)
{
    lh_assert_runtime_ifn(index < lh_entity_screen_get_dirty_count(self),
                          lh_runtime_error_code_out_of_range);
    return self->dirty[index];
}

/* @p entity when its box cuts children to itself, or null. No box (size is
 * not positive) is only a transform: it does not cut. */
static const lh_entity_2d_t *
lh_entity_screen_clip_box(const lh_entity_t *entity)
{
    lh_return_if(lh_entity_has_flags(entity, lh_entity_flags_overflow_visible), lh_null);
    const lh_entity_2d_t *const spatial = lh_entity_cast(entity, lh_addr_of(lh_entity_2d_class));
    lh_return_ifn(spatial, lh_null);
    const lh_math_vec2_t size = lh_entity_2d_get_size(spatial);
    lh_return_if(lh_math_vec2_get_x(lh_addr_of(size)) <= 0.0f ||
                     lh_math_vec2_get_y(lh_addr_of(size)) <= 0.0f,
                 lh_null);
    return spatial;
}

/* Draw @p entity and its children onto @p canvas within @p clip.
 * Background first (the style), then DRAW for anything on top of it, then
 * children. A box that misses @p clip is skipped with the children it cuts:
 * that is the partial redraw, the same walk LVGL does over a dirty area. */
static lh_void
lh_entity_screen_draw(lh_entity_t *entity, lh_ui_canvas_t *canvas, lh_math_rect_t clip)
{
    if (lh_entity_has_flags(entity, lh_entity_flags_hidden))
    {
        return;
    }

    const lh_entity_2d_t *const clip_box = lh_entity_screen_clip_box(entity);
    lh_math_rect_t bounds = lh_math_rect_make_empty();
    if (lh_ptr_is_set(clip_box))
    {
        bounds = lh_entity_2d_get_screen_bounds(clip_box);
        if (!lh_math_rect_intersects(lh_addr_of(clip), lh_addr_of(bounds)))
        {
            return;
        }
    }

    lh_ui_canvas_set_clip(canvas, clip);
    const lh_entity_2d_t *const spatial = lh_entity_cast(entity, lh_addr_of(lh_entity_2d_class));
    if (lh_ptr_is_set(spatial))
    {
        const lh_ui_effect_t *const effect = lh_entity_2d_get_effect(spatial);
        if (lh_ptr_is_set(effect))
        {
            const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(spatial);
            const lh_math_vec4_t origin = lh_math_mat4_get_column(lh_addr_of(world), 3);
            lh_ui_canvas_set_draw_z(canvas, lh_math_vec4_get_z(lh_addr_of(origin)));
            lh_ui_effect_draw(effect, canvas, lh_entity_2d_get_screen_bounds(spatial), 0);
        }
        if (!lh_entity_has_flags(entity, lh_entity_flags_own_background))
        {
            lh_entity_2d_draw_background(spatial, canvas);
        }
    }
    lh_entity_notify(entity, LH_ENTITY_EVENT_DRAW, canvas);

    lh_math_rect_t child_clip = clip;
    if (lh_ptr_is_set(clip_box))
    {
        child_clip = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(bounds));
        if (lh_math_rect_is_empty(lh_addr_of(child_clip)))
        {
            return;
        }
    }

    lh_entity_foreach_child(child, entity)
    {
        lh_entity_screen_draw(child, canvas, child_clip);
    }
}

lh_math_rect_t
lh_entity_screen_render(lh_entity_screen_t *self, lh_ui_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(canvas);

    lh_math_rect_t drawn = lh_math_rect_make_empty();
    lh_entity_flex_layout_tree(lh_ptr_rcast(lh_entity_t, self));
    for (lh_usize_t i = 0; i < lh_entity_screen_get_dirty_count(self); ++i)
    {
        const lh_math_rect_t area = lh_entity_screen_get_dirty_area(self, i);
        lh_ui_canvas_clear_depth(canvas, area);
        lh_entity_screen_draw(lh_ptr_rcast(lh_entity_t, self), canvas, area);
        drawn = lh_math_rect_union(lh_addr_of(drawn), lh_addr_of(area));
    }
    self->dirty_count = 0;
    return drawn;
}

/* Mark what @p entity and its descendants cover. A box that cuts its
 * children covers them too, so the walk stops there. */
static lh_void
lh_entity_screen_invalidate_tree(lh_entity_screen_t *screen, lh_entity_t *entity)
{
    const lh_entity_2d_t *const spatial = lh_entity_cast(entity, lh_addr_of(lh_entity_2d_class));
    if (lh_ptr_is_set(spatial))
    {
        const lh_math_vec2_t size = lh_entity_2d_get_size(spatial);
        if (lh_math_vec2_get_x(lh_addr_of(size)) > 0.0f && lh_math_vec2_get_y(lh_addr_of(size)) > 0.0f)
        {
            lh_math_rect_t bounds = lh_entity_2d_get_screen_bounds(spatial);
            const lh_int_t pad = lh_ui_effect_outset(lh_entity_2d_get_effect(spatial));
            if (pad > 0)
            {
                const lh_int_t x = lh_math_rect_get_x(lh_addr_of(bounds)) - pad;
                const lh_int_t y = lh_math_rect_get_y(lh_addr_of(bounds)) - pad;
                const lh_int_t width = lh_math_rect_get_size_width(lh_addr_of(bounds)) + pad * 2;
                const lh_int_t height = lh_math_rect_get_size_height(lh_addr_of(bounds)) + pad * 2;
                bounds = lh_math_rect_make(x, y, width, height);
            }
            lh_entity_screen_invalidate_area(screen, bounds);
        }
    }
    lh_return_if(lh_ptr_is_set(lh_entity_screen_clip_box(entity)));
    lh_entity_foreach_child(child, entity)
    {
        lh_entity_screen_invalidate_tree(screen, child);
    }
}

lh_void
lh_entity_invalidate(lh_entity_t *self)
{
    lh_entity_screen_t *const screen =
        lh_entity_cast(lh_entity_get_root(self), lh_addr_of(lh_entity_screen_class));
    if (lh_ptr_is_set(screen))
    {
        lh_entity_screen_invalidate_tree(screen, self);
    }
}

lh_void
lh_entity_screen_clear_pressed(lh_entity_screen_t *self, lh_entity_t *entity)
{
    lh_assert_runtime_ref(self);
    if (self->pressed == entity)
    {
        self->pressed = lh_null;
    }
}

lh_entity_t *
lh_entity_screen_get_pressed(const lh_entity_screen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

lh_entity_t *
lh_entity_screen_send_pointer(lh_entity_screen_t *self, lh_uint_t code, lh_math_vec2_t point)
{
    lh_assert_runtime_ref(self);
    lh_entity_t *const target = lh_entity_2d_find_at(lh_ptr_rcast(lh_entity_t, self), point);
    lh_entity_t *const held = self->pressed;
    if (code == LH_ENTITY_EVENT_POINTER_UP)
    {
        self->pressed = lh_null;
        if (lh_ptr_is_set(held) && held != target)
        {
            lh_entity_send_event(held, code, lh_addr_of(point));
        }
    }
    else if (code == LH_ENTITY_EVENT_POINTER_MOVE && lh_ptr_is_set(held))
    {
        lh_entity_send_event(held, code, lh_addr_of(point));
        return held;
    }
    if (lh_ptr_is_set(target))
    {
        lh_entity_send_event(target, code, lh_addr_of(point));
    }
    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        self->pressed = target;
    }
    return target;
}

lh_entity_t *
lh_entity_screen_send_wheel(lh_entity_screen_t *self, lh_math_vec2_t point, lh_math_vec2_t delta)
{
    lh_entity_t *entity;
    lh_assert_runtime_ref(self);
    entity = lh_entity_2d_find_at(lh_ptr_rcast(lh_entity_t, self), point);
    /* Notify and not send: the walk up is the loop, and a receiver that lets
       the wheel past must not have it delivered twice by the bubble flag. */
    while (lh_ptr_is_set(entity))
    {
        if (lh_entity_notify(entity, LH_ENTITY_EVENT_WHEEL, lh_addr_of(delta)) != lh_bool_false)
        {
            return entity;
        }
        entity = lh_entity_get_parent(entity);
    }
    return lh_null;
}

lh_entity_t *
lh_entity_screen_get_focus(const lh_entity_screen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->focus;
}

lh_void
lh_entity_screen_set_focus(lh_entity_screen_t *self, lh_entity_t *entity)
{
    lh_entity_t *previous;
    lh_assert_runtime_ref(self);
    previous = self->focus;
    self->focus = entity;
    if (lh_ptr_is_set(previous))
    {
        lh_entity_invalidate(previous);
    }
    if (lh_ptr_is_set(entity))
    {
        lh_entity_invalidate(entity);
    }
}

lh_void
lh_entity_screen_send_key(lh_entity_screen_t *self, lh_uint_t code)
{
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_set(self->focus))
    {
        lh_entity_send_event(self->focus, LH_ENTITY_EVENT_KEY, lh_addr_of(code));
    }
}

lh_void
lh_entity_screen_send_tick(lh_entity_screen_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_set(self->pressed))
    {
        lh_entity_send_event(self->pressed, LH_ENTITY_EVENT_TICK, lh_null);
    }
}
