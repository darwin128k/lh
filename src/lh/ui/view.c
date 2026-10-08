/**
 * @file view.c
 * @brief Implementation of lh/ui/view.h — entity tree on an lh_ui_canvas_t.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/entity/container.h>
#include <lh/ui/entity/scrollbar.h>
#include <lh/ui/key.h>
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
    self->strip_height = lh_ui_scalar(0);
    self->scrolling = lh_null;
    self->throwing = lh_null;
    self->focus = lh_null;
    lh_ui_point_init(lh_addr_of(self->velocity), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_view_reset_pointer(self);
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
lh_ui_view_set_strip_height(lh_ui_view_t *self, lh_ui_scalar_t height)
{
    lh_assert_runtime_ref(self);
    self->strip_height = height;
}

lh_ui_scalar_t
lh_ui_view_get_strip_height(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->strip_height;
}

lh_bool_t
lh_ui_view_is_stripped(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_gt(self->strip_height, lh_ui_scalar(0)) ? lh_bool_true : lh_bool_false;
}

lh_ui_rect_t
lh_ui_view_get_strip(const lh_ui_view_t *self, lh_ui_scalar_t index)
{
    const lh_ui_size_t size = lh_ui_canvas_get_size(self->canvas);
    const lh_ui_scalar_t whole = lh_ui_size_get_height(lh_addr_of(size));
    const lh_ui_scalar_t height = lh_math_min(self->strip_height, whole);
    const lh_ui_scalar_t top = lh_math_min(index * height, whole - height);
    lh_ui_rect_t strip;

    lh_assert_runtime_ref(self);
    lh_ui_rect_init_empty(lh_addr_of(strip));
    /* Past the last strip, and never a strip taller than the target itself. */
    lh_return_if(height <= lh_ui_scalar(0) || index < lh_ui_scalar(0) || index * height >= whole, strip);
    lh_ui_rect_init(lh_addr_of(strip), lh_ui_scalar(0), top, lh_ui_size_get_width(lh_addr_of(size)), height);
    return strip;
}

lh_void
lh_ui_view_draw_frame_on(lh_ui_view_t *self, const lh_ui_rect_t *area, const lh_ui_rect_t *damage)
{
    const lh_ui_rect_t part = lh_ui_canvas_damage_in(area, damage);
    lh_ui_point_t zero;

    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(part)));
    lh_ui_point_init(lh_addr_of(zero), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_canvas_begin_area(self->canvas, area);
    lh_ui_canvas_push(self->canvas, zero, lh_addr_of(part));
    lh_ui_view_clear(self, lh_addr_of(part));
    lh_ui_view_draw_root(self);
    lh_ui_canvas_pop(self->canvas);
    lh_ui_canvas_end(self->canvas);
}

lh_void
lh_ui_view_draw_strips(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_ui_scalar_t index;

    for (index = lh_ui_scalar(0);; ++index)
    {
        const lh_ui_rect_t area = lh_ui_view_get_strip(self, index);
        const lh_ui_rect_t part = lh_ui_canvas_damage_in(lh_addr_of(area), damage);

        lh_return_if(lh_ui_rect_is_empty(lh_addr_of(area)));
        /* A strip the damage does not reach keeps the pixels already on
           screen: no buffer, no clear, no blit. That is where the time and
           the memory of a partial frame come from. */
        if (!lh_ui_rect_is_empty(lh_addr_of(part)))
        {
            lh_ui_view_draw_frame_on(self, lh_addr_of(area), damage);
        }
    }
}

lh_void
lh_ui_view_draw(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->canvas));
    lh_ui_view_fit_canvas(self);
    lh_ui_canvas_reset_damage(self->canvas);
    if (lh_ui_view_is_stripped(self))
    {
        lh_ui_view_draw_strips(self, damage);
    }
    else
    {
        lh_ui_view_draw_frame(self, damage);
    }
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
    lh_ui_view_end_scroll(self);
    return clicked;
}

/* ── Scroll gestures ─────────────────────────────────────────────────────── */

lh_void
lh_ui_view_begin_scroll(lh_ui_view_t *self, lh_ui_entity_container_t *box)
{
    lh_return_if(self->scrolling == box);
    lh_ui_view_end_scroll(self);
    self->scrolling = box;
    lh_ui_entity_send(lh_ui_entity_container_as_entity(box), lh_ui_entity_event_scroll_begin, lh_null);
}

lh_void
lh_ui_view_end_scroll(lh_ui_view_t *self)
{
    lh_ui_entity_container_t *box = self->scrolling;

    lh_return_if(lh_null_eq(box));
    self->scrolling = lh_null;
    lh_ui_entity_send(lh_ui_entity_container_as_entity(box), lh_ui_entity_event_scroll_end, lh_null);
}

lh_bool_t
lh_ui_view_damage_scroll(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_point_t before)
{
    const lh_ui_point_t after = lh_ui_entity_container_get_scroll(box);

    lh_return_if(lh_ui_point_equals(lh_addr_of(before), lh_addr_of(after)), lh_bool_false);
    lh_ui_view_begin_scroll(self, box);
    lh_ui_entity_send(lh_ui_entity_container_as_entity(box), lh_ui_entity_event_scroll, lh_null);
    lh_return_if(lh_null_eq(self->canvas), lh_bool_true);
    lh_ui_canvas_reset_damage(self->canvas);
    lh_ui_entity_scrollbar_add_scroll_damage(box, self->canvas);
    return lh_bool_true;
}

lh_bool_t
lh_ui_view_scroll_by(lh_ui_view_t *self, lh_ui_entity_container_t *box, lh_ui_scalar_t dx, lh_ui_scalar_t dy)
{
    lh_ui_point_t before;

    lh_return_if(lh_null_eq(box), lh_bool_false);
    before = lh_ui_entity_container_get_scroll(box);
    lh_ui_entity_container_scroll_by(box, dx, dy);
    return lh_ui_view_damage_scroll(self, box, before);
}

lh_bool_t
lh_ui_view_wheel(lh_ui_view_t *self, lh_ui_point_t point, lh_ui_scalar_t dx, lh_ui_scalar_t dy)
{
    lh_bool_t changed;

    lh_ui_view_stop_throw(self);
    changed = lh_ui_view_scroll_by(self, lh_ui_entity_scrollbar_find_scrolled(lh_ui_view_hit_test(self, point)),
                                   dx, dy);
    lh_ui_view_end_scroll(self);
    return changed;
}

/* ── Throw ───────────────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_view_is_slow(lh_ui_point_t velocity, lh_ui_scalar_t limit)
{
    return lh_math_abs(lh_ui_point_get_x(lh_addr_of(velocity))) < limit &&
                   lh_math_abs(lh_ui_point_get_y(lh_addr_of(velocity))) < limit
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_view_stop_throw(lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_point_init(lh_addr_of(self->velocity), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_return_if(lh_null_eq(self->throwing));
    self->throwing = lh_null;
    lh_ui_view_end_scroll(self);
}

lh_void
lh_ui_view_start_throw(lh_ui_view_t *self, lh_ui_entity_container_t *box)
{
    if (lh_ui_view_is_slow(self->velocity, LH_UI_VIEW_THROW_MIN))
    {
        lh_ui_view_end_scroll(self);
        return;
    }
    self->throwing = box;
}

lh_bool_t
lh_ui_view_is_gliding(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_null_ne(self->throwing) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_ui_view_tick(lh_ui_view_t *self)
{
    lh_bool_t moved;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->throwing), lh_bool_false);
    self->velocity = lh_ui_view_slow_down(self->velocity);
    moved = lh_ui_view_scroll_by(self, self->throwing, lh_ui_point_get_x(lh_addr_of(self->velocity)),
                                 lh_ui_point_get_y(lh_addr_of(self->velocity)));
    lh_return_if(moved && !lh_ui_view_is_slow(self->velocity, lh_ui_scalar(1)), moved);
    lh_ui_view_stop_throw(self);
    return moved;
}

lh_ui_point_t
lh_ui_view_slow_down(lh_ui_point_t velocity)
{
    lh_ui_point_t slower;

    lh_ui_point_init(lh_addr_of(slower), lh_ui_point_get_x(lh_addr_of(velocity)) * LH_UI_VIEW_THROW_KEEP / 100,
                     lh_ui_point_get_y(lh_addr_of(velocity)) * LH_UI_VIEW_THROW_KEEP / 100);
    return slower;
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
    self->target = lh_null;
    self->drag = lh_null;
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

lh_ui_entity_container_t *
lh_ui_view_get_drag_box(lh_ui_entity_t *hit)
{
    lh_return_if(lh_null_ne(lh_ui_entity_as_scrollbar(hit)), lh_null);
    return lh_ui_entity_find_container(hit);
}

lh_void
lh_ui_view_start_pointer(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_ui_view_stop_throw(self);
    lh_ui_view_reset_pointer(self);
    self->pressed = lh_bool_true;
    self->press_point = point;
    self->last_point = point;
}

lh_void
lh_ui_view_press_on(lh_ui_view_t *self, lh_ui_entity_t *hit, lh_ui_point_t point)
{
    self->target = hit;
    lh_ui_entity_send_pointer(hit, lh_ui_entity_event_press, point);
    lh_ui_view_set_focus(self, lh_ui_entity_find_focusable(hit));
    lh_ui_view_grab_thumb(self, lh_ui_entity_as_scrollbar(hit), point);
    self->drag = lh_null_eq(self->grab) ? lh_ui_view_get_drag_box(hit) : lh_null;
}

lh_void
lh_ui_view_press(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_ui_view_start_pointer(self, point);
    lh_ui_view_press_on(self, lh_ui_view_hit_test(self, point), point);
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
lh_ui_view_is_past_threshold(const lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_ui_point_t moved;

    lh_ui_point_init(lh_addr_of(moved),
                     lh_ui_point_get_x(lh_addr_of(point)) - lh_ui_point_get_x(lh_addr_of(self->press_point)),
                     lh_ui_point_get_y(lh_addr_of(point)) - lh_ui_point_get_y(lh_addr_of(self->press_point)));
    return !lh_ui_view_is_slow(moved, LH_UI_VIEW_DRAG_THRESHOLD);
}

lh_void
lh_ui_view_track(lh_ui_view_t *self, lh_ui_point_t point)
{
    const lh_ui_scalar_t dx = lh_ui_point_get_x(lh_addr_of(self->last_point)) - lh_ui_point_get_x(lh_addr_of(point));
    const lh_ui_scalar_t dy = lh_ui_point_get_y(lh_addr_of(self->last_point)) - lh_ui_point_get_y(lh_addr_of(point));

    /* The content follows the pointer; the glide keeps half the old speed. */
    lh_ui_point_init(lh_addr_of(self->velocity), (lh_ui_point_get_x(lh_addr_of(self->velocity)) + dx) / 2,
                     (lh_ui_point_get_y(lh_addr_of(self->velocity)) + dy) / 2);
    self->last_point = point;
}

lh_bool_t
lh_ui_view_drag_content(lh_ui_view_t *self, lh_ui_point_t point)
{
    const lh_ui_point_t from = self->last_point;

    lh_return_if(!self->dragged && !lh_ui_view_is_past_threshold(self, point), lh_bool_false);
    self->dragged = lh_bool_true;
    lh_ui_view_track(self, point);
    return lh_ui_view_scroll_by(self, self->drag,
                                lh_ui_point_get_x(lh_addr_of(from)) - lh_ui_point_get_x(lh_addr_of(point)),
                                lh_ui_point_get_y(lh_addr_of(from)) - lh_ui_point_get_y(lh_addr_of(point)));
}

lh_bool_t
lh_ui_view_move(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    if (lh_null_ne(self->grab))
    {
        self->dragged = lh_bool_true;
        return lh_ui_view_drag_thumb(self, self->grab, point);
    }
    lh_return_if(!self->pressed || lh_null_eq(self->drag), lh_bool_false);
    return lh_ui_view_drag_content(self, point);
}

lh_void
lh_ui_view_finish_gesture(lh_ui_view_t *self)
{
    if (lh_null_ne(self->drag) && self->dragged)
    {
        lh_ui_view_start_throw(self, self->drag);
        return;
    }
    lh_ui_view_end_scroll(self);
}

lh_ui_entity_t *
lh_ui_view_release(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_bool_t clicked;

    lh_assert_runtime_ref(self);
    lh_ui_entity_send_pointer(self->target, lh_ui_entity_event_release, point);
    lh_ui_view_finish_gesture(self);
    clicked = self->pressed && !self->dragged ? lh_bool_true : lh_bool_false;
    lh_ui_view_reset_pointer(self);
    lh_return_if(!clicked, lh_null);
    return lh_ui_view_click(self, point);
}

/* ── Focus and keys ──────────────────────────────────────────────────────── */

lh_ui_entity_t *
lh_ui_view_get_focus(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->focus;
}

lh_void
lh_ui_view_set_focus(lh_ui_view_t *self, lh_ui_entity_t *entity)
{
    lh_assert_runtime_ref(self);
    lh_return_if(self->focus == entity);
    if (lh_null_ne(self->focus))
    {
        lh_ui_entity_send(self->focus, lh_ui_entity_event_defocus, lh_null);
    }
    self->focus = entity;
    lh_return_if(lh_null_eq(entity));
    lh_ui_entity_send(entity, lh_ui_entity_event_focus, lh_null);
}

lh_bool_t
lh_ui_view_visit_focus(const lh_ui_entity_t *entity, lh_ptr context)
{
    lh_ui_view_focus_walk_t *walk = lh_ptr_rcast(lh_ui_view_focus_walk_t, context);

    lh_return_if(!lh_ui_entity_is_focusable(entity), lh_bool_true);
    if (lh_null_eq(walk->first))
    {
        walk->first = entity;
    }
    lh_return_if(walk->passed && lh_null_eq(walk->next), (walk->next = entity, lh_bool_false));
    walk->passed = entity == walk->current ? lh_bool_true : walk->passed;
    return lh_bool_true;
}

lh_ui_entity_t *
lh_ui_view_get_next_focus(const lh_ui_view_t *self)
{
    lh_ui_view_focus_walk_t walk = {lh_null, lh_bool_false, lh_null, lh_null};

    lh_return_if(lh_null_eq(self->root), lh_null);
    walk.current = self->focus;
    walk.passed = lh_null_eq(self->focus) ? lh_bool_true : lh_bool_false;
    (void)lh_ui_entity_walk(self->root, lh_ui_view_visit_focus, lh_addr_of(walk));
    return lh_ptr_rcast(lh_ui_entity_t, lh_null_ne(walk.next) ? walk.next : walk.first);
}

lh_bool_t
lh_ui_view_send_input(lh_ui_view_t *self, const lh_ui_key_input_t *input)
{
    lh_ui_entity_container_t *box = lh_ui_entity_as_container(self->focus);
    lh_ui_point_t before;
    lh_bool_t moved;

    lh_return_if(lh_null_eq(self->focus), lh_bool_false);
    lh_ui_point_init(lh_addr_of(before), lh_ui_scalar(0), lh_ui_scalar(0));
    before = lh_null_ne(box) ? lh_ui_entity_container_get_scroll(box) : before;
    lh_ui_entity_send(self->focus, lh_ui_entity_event_key, lh_ptr_rcast(lh_void, input));
    lh_return_if(lh_null_eq(box), lh_bool_false);
    moved = lh_ui_view_damage_scroll(self, box, before);
    lh_ui_view_end_scroll(self);
    return moved;
}

lh_bool_t
lh_ui_view_key(lh_ui_view_t *self, lh_key_t key, lh_bool_t pressed)
{
    lh_ui_key_input_t input;

    lh_assert_runtime_ref(self);
    if (key == lh_key_tab && pressed)
    {
        lh_ui_view_set_focus(self, lh_ui_view_get_next_focus(self));
        return lh_bool_false;
    }
    lh_ui_key_input_init_key(lh_addr_of(input), key, pressed);
    return lh_ui_view_send_input(self, lh_addr_of(input));
}

lh_bool_t
lh_ui_view_text(lh_ui_view_t *self, lh_u32_t code)
{
    lh_ui_key_input_t input;

    lh_assert_runtime_ref(self);
    lh_ui_key_input_init_text(lh_addr_of(input), code);
    return lh_ui_view_send_input(self, lh_addr_of(input));
}
