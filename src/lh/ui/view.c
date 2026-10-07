/**
 * @file view.c
 * @brief Implementation of lh/ui/view.h — entity tree on an lh_ui_canvas_t.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/entity/container.h>
#include <lh/ui/entity/scrollbar.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/ui/view.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

lh_void
lh_ui_view_init(lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    self->canvas = lh_null;
    self->root = lh_null;
    self->clear = lh_null;
    self->grab = lh_null;
    self->grab_offset = lh_ui_scalar(0);
    self->pressed = lh_bool_false;
    self->dragged = lh_bool_false;
}

lh_void
lh_ui_view_deinit(lh_ui_view_t *self)
{
    lh_ui_view_init(self);
}

lh_void
lh_ui_view_set_canvas(lh_ui_view_t *self, lh_ui_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    self->canvas = canvas;
}

lh_ui_canvas_t *
lh_ui_view_get_canvas(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->canvas;
}

lh_void
lh_ui_view_set_root(lh_ui_view_t *self, lh_ui_entity_t *root)
{
    lh_assert_runtime_ref(self);
    self->root = root;
}

lh_ui_entity_t *
lh_ui_view_get_root(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root;
}

lh_void
lh_ui_view_set_clear(lh_ui_view_t *self, const lh_ui_color_t *clear)
{
    lh_assert_runtime_ref(self);
    self->clear = clear;
}

const lh_ui_color_t *
lh_ui_view_get_clear(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clear;
}

lh_void
lh_ui_view_paint(lh_ui_view_t *self)
{
    lh_ui_view_draw(self, lh_null);
}

/* ── Draw ────────────────────────────────────────────────────────────────── */

lh_void
lh_ui_view_fit_canvas(lh_ui_view_t *self)
{
    lh_ui_rect_t rect;

    lh_return_if(lh_null_eq(self->root));
    rect = lh_ui_entity_get_rect(self->root);
    lh_ui_canvas_set_size(self->canvas, *lh_ui_rect_get_size_as_const(lh_addr_of(rect)));
}

lh_void
lh_ui_view_clear(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_return_if(lh_null_eq(self->clear));
    if (lh_null_eq(damage))
    {
        lh_ui_canvas_clear(self->canvas, self->clear);
        return;
    }
    lh_ui_canvas_fill_rect(self->canvas, damage, self->clear);
}

lh_void
lh_ui_view_draw_root(lh_ui_view_t *self)
{
    lh_return_if(lh_null_eq(self->root));
    lh_ui_entity_draw(self->root, self->canvas);
}

lh_void
lh_ui_view_draw_frame(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_ui_point_t zero;

    lh_ui_point_init(lh_addr_of(zero), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_canvas_begin(self->canvas);
    lh_ui_canvas_push(self->canvas, zero, damage);
    lh_ui_view_clear(self, damage);
    lh_ui_view_draw_root(self);
    lh_ui_canvas_pop(self->canvas);
    lh_ui_canvas_end(self->canvas);
}

lh_void
lh_ui_view_draw(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->canvas));
    lh_ui_view_fit_canvas(self);
    lh_ui_canvas_reset_damage(self->canvas);
    lh_ui_view_draw_frame(self, damage);
    lh_ui_canvas_reset_damage(self->canvas);
}

/* ── Hit and click ───────────────────────────────────────────────────────── */

lh_ui_entity_t *
lh_ui_view_hit_test(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->root), lh_null);
    return lh_ui_entity_find_at(self->root, point);
}

lh_ui_entity_t *
lh_ui_view_click(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->root), lh_null);
    return lh_ui_view_click_scrolling(
        self, lh_ui_entity_scrollbar_get_driven(lh_ui_view_hit_test(self, point)), point);
}

lh_ui_entity_t *
lh_ui_view_click_scrolling(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_point_t point)
{
    lh_ui_point_t before;
    lh_ui_entity_t *clicked;

    lh_return_if(lh_null_eq(box), lh_ui_entity_click(self->root, point));
    before = lh_ui_entity_container_get_scroll(box);
    clicked = lh_ui_entity_click(self->root, point);
    (void)lh_ui_view_damage_scroll(self, box, before);
    return clicked;
}

/* ── Scroll damage ───────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_view_damage_scroll(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_point_t before)
{
    const lh_ui_point_t after = lh_ui_entity_container_get_scroll(box);

    lh_return_if(lh_ui_point_equals(lh_addr_of(before), lh_addr_of(after)), lh_bool_false);
    lh_return_if(lh_null_eq(self->canvas), lh_bool_true);
    lh_ui_canvas_reset_damage(self->canvas);
    lh_ui_entity_scrollbar_add_scroll_damage(box, self->canvas);
    return lh_bool_true;
}

/* ── Pointer session ─────────────────────────────────────────────────────── */

lh_void
lh_ui_view_reset_pointer(lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    self->grab = lh_null;
    self->grab_offset = lh_ui_scalar(0);
    self->pressed = lh_bool_false;
    self->dragged = lh_bool_false;
}

lh_ui_scalar_t
lh_ui_view_get_thumb_start_at(const lh_ui_entity_scrollbar_t *bar, lh_ui_point_t point)
{
    const lh_ui_entity_t *entity = lh_ptr_rcast(const lh_ui_entity_t, bar);

    return lh_ui_entity_scrollbar_get_thumb_start_at(bar, lh_ui_entity_to_local(entity, point));
}

lh_void
lh_ui_view_grab_thumb(lh_ui_view_t *self, lh_ui_entity_scrollbar_t *bar, lh_ui_point_t point)
{
    lh_return_if(lh_null_eq(bar));
    lh_return_if(!lh_ui_entity_scrollbar_contains_thumb(
        bar, lh_ui_entity_to_local(lh_ui_entity_scrollbar_as_entity(bar), point)));
    self->grab = bar;
    self->grab_offset = lh_ui_view_get_thumb_start_at(bar, point) - lh_ui_entity_scrollbar_get_thumb_start(bar);
}

lh_void
lh_ui_view_press(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_ui_view_reset_pointer(self);
    self->pressed = lh_bool_true;
    lh_ui_view_grab_thumb(self, lh_ui_entity_as_scrollbar(lh_ui_view_hit_test(self, point)), point);
}

lh_bool_t
lh_ui_view_drag_thumb(lh_ui_view_t *self, lh_ui_entity_scrollbar_t *bar, lh_ui_point_t point)
{
    lh_ui_entity_container_t *box = lh_ui_entity_scrollbar_get_container(bar);
    const lh_ui_point_t before = lh_ui_entity_container_get_scroll(box);

    lh_ui_entity_scrollbar_set_thumb_start(bar, lh_ui_view_get_thumb_start_at(bar, point) - self->grab_offset);
    return lh_ui_view_damage_scroll(self, box, before);
}

lh_bool_t
lh_ui_view_move(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->grab), lh_bool_false);
    self->dragged = lh_bool_true;
    return lh_ui_view_drag_thumb(self, self->grab, point);
}

lh_ui_entity_t *
lh_ui_view_release(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_bool_t clicked;

    lh_assert_runtime_ref(self);
    clicked = self->pressed && !self->dragged ? lh_bool_true : lh_bool_false;
    lh_ui_view_reset_pointer(self);
    lh_return_if(!clicked, lh_null);
    return lh_ui_view_click(self, point);
}

/* ── Wheel ───────────────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_view_scroll_by(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_scalar_t dy)
{
    lh_ui_point_t before;

    lh_return_if(lh_null_eq(box), lh_bool_false);
    before = lh_ui_entity_container_get_scroll(box);
    lh_ui_entity_container_scroll_by(box, lh_ui_scalar(0), dy);
    return lh_ui_view_damage_scroll(self, box, before);
}

lh_bool_t
lh_ui_view_wheel(lh_ui_view_t *self, lh_ui_point_t point, lh_ui_scalar_t dy)
{
    return lh_ui_view_scroll_by(self, lh_ui_entity_scrollbar_find_scrolled(lh_ui_view_hit_test(self, point)), dy);
}
