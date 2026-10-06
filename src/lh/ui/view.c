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

lh_void
lh_ui_view_draw(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_ui_canvas_t *canvas;
    lh_ui_point_t zero;
    lh_ui_rect_t root_rect;
    lh_ui_size_t size;

    lh_assert_runtime_ref(self);
    canvas = self->canvas;
    if (lh_null_eq(canvas))
    {
        return;
    }
    if (lh_null_ne(self->root))
    {
        root_rect = lh_ui_entity_get_rect(self->root);
        size = *lh_ui_rect_get_size_as_const(lh_addr_of(root_rect));
        lh_ui_canvas_set_size(canvas, size);
    }
    lh_ui_canvas_reset_damage(canvas);
    lh_ui_canvas_begin(canvas);
    lh_ui_point_init(lh_addr_of(zero), lh_ui_scalar(0), lh_ui_scalar(0));
    if (lh_null_ne(damage))
    {
        lh_ui_canvas_push(canvas, zero, damage);
        if (lh_null_ne(self->clear))
        {
            lh_ui_canvas_fill_rect(canvas, damage, self->clear);
        }
    }
    else if (lh_null_ne(self->clear))
    {
        lh_ui_canvas_clear(canvas, self->clear);
    }
    if (lh_null_ne(self->root))
    {
        lh_ui_entity_draw(self->root, canvas);
    }
    if (lh_null_ne(damage))
    {
        lh_ui_canvas_pop(canvas);
    }
    lh_ui_canvas_end(canvas);
    lh_ui_canvas_reset_damage(canvas);
}

lh_ui_entity_t *
lh_ui_view_hit_test(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    if (lh_null_eq(self->root))
    {
        return lh_null;
    }
    return lh_ui_entity_find_at(self->root, point);
}

lh_ui_entity_t *
lh_ui_view_click(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_ui_entity_t *hit;
    lh_ui_entity_scrollbar_t *bar;
    lh_ui_entity_container_t *box;
    lh_ui_rect_t thumb_before;
    lh_ui_scalar_t scroll_before;
    lh_ui_entity_t *clicked;

    lh_assert_runtime_ref(self);
    if (lh_null_eq(self->root))
    {
        return lh_null;
    }
    hit = lh_ui_entity_find_at(self->root, point);
    bar = lh_ui_entity_as_scrollbar(hit);
    if (lh_null_ne(bar))
    {
        box = lh_ui_entity_scrollbar_get_container(bar);
        thumb_before = lh_ui_entity_scrollbar_get_thumb_rect(bar);
        scroll_before = lh_ui_entity_scrollbar_get_scroll(bar);
        clicked = lh_ui_entity_click(self->root, point);
        if (lh_ui_entity_scrollbar_get_scroll(bar) != scroll_before)
        {
            lh_ui_canvas_reset_damage(self->canvas);
            lh_ui_entity_scrollbar_add_scroll_damage(bar, self->canvas, lh_addr_of(thumb_before));
        }
        return clicked;
    }
    return lh_ui_entity_click(self->root, point);
}

lh_void
lh_ui_view_press(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_ui_entity_t *hit;
    lh_ui_entity_scrollbar_t *bar;

    lh_assert_runtime_ref(self);
    self->grab = lh_null;
    self->grab_offset = lh_ui_scalar(0);
    self->pressed = lh_bool_true;
    self->dragged = lh_bool_false;
    hit = lh_ui_view_hit_test(self, point);
    bar = lh_ui_entity_as_scrollbar(hit);
    if (lh_null_eq(bar) || !lh_ui_entity_scrollbar_contains_thumb(bar, point))
    {
        return;
    }
    self->grab = bar;
    self->grab_offset = lh_ui_entity_scrollbar_get_thumb_start_at(bar, point) -
                        lh_ui_entity_scrollbar_get_thumb_start(bar);
}

lh_bool_t
lh_ui_view_move(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_ui_entity_scrollbar_t *bar;
    lh_ui_entity_container_t *box;
    lh_ui_rect_t thumb_before;
    lh_ui_scalar_t before;
    lh_ui_scalar_t after;

    lh_assert_runtime_ref(self);
    bar = self->grab;
    if (lh_null_eq(bar))
    {
        return lh_bool_false;
    }
    box = lh_ui_entity_scrollbar_get_container(bar);
    thumb_before = lh_ui_entity_scrollbar_get_thumb_rect(bar);
    before = lh_ui_entity_scrollbar_get_scroll(bar);
    lh_ui_entity_scrollbar_set_thumb_start(
        bar, lh_ui_entity_scrollbar_get_thumb_start_at(bar, point) - self->grab_offset);
    self->dragged = lh_bool_true;
    after = lh_ui_entity_scrollbar_get_scroll(bar);
    if (before == after)
    {
        return lh_bool_false;
    }
    lh_ui_canvas_reset_damage(self->canvas);
    lh_ui_entity_scrollbar_add_scroll_damage(bar, self->canvas, lh_addr_of(thumb_before));
    return lh_bool_true;
}

lh_ui_entity_t *
lh_ui_view_release(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_bool_t clicked;

    lh_assert_runtime_ref(self);
    clicked = self->pressed && !self->dragged;
    self->grab = lh_null;
    self->grab_offset = lh_ui_scalar(0);
    self->pressed = lh_bool_false;
    self->dragged = lh_bool_false;
    if (!clicked)
    {
        return lh_null;
    }
    return lh_ui_view_click(self, point);
}

lh_bool_t
lh_ui_view_wheel(lh_ui_view_t *self, lh_ui_point_t point, lh_ui_scalar_t dy)
{
    lh_ui_entity_t *hit;
    lh_ui_entity_t *parent;
    lh_ui_entity_t *child;
    lh_ui_entity_scrollbar_t *bar;
    lh_ui_entity_scrollbar_t *sibling;
    lh_ui_entity_container_t *box;
    lh_ui_rect_t thumb_before;
    lh_ui_point_t before;
    lh_ui_point_t after;

    lh_assert_runtime_ref(self);
    hit = lh_ui_view_hit_test(self, point);
    bar = lh_ui_entity_as_scrollbar(hit);
    box = lh_null_ne(bar) ? lh_ui_entity_scrollbar_get_container(bar)
                          : lh_ui_entity_find_container(hit);
    if (lh_null_eq(box))
    {
        return lh_bool_false;
    }
    if (lh_null_eq(bar))
    {
        parent = lh_ui_entity_get_parent(lh_ui_entity_container_as_entity(box));
        child = lh_null_ne(parent) ? lh_ui_entity_get_first_child(parent) : lh_null;
        while (lh_null_ne(child))
        {
            sibling = lh_ui_entity_as_scrollbar(child);
            if (lh_null_ne(sibling) &&
                lh_ui_entity_scrollbar_get_container(sibling) == box)
            {
                bar = sibling;
                break;
            }
            child = lh_ui_entity_get_next_child(parent, child);
        }
    }
    if (lh_null_ne(bar))
    {
        thumb_before = lh_ui_entity_scrollbar_get_thumb_rect(bar);
    }
    before = lh_ui_entity_container_get_scroll(box);
    lh_ui_entity_container_scroll_by(box, lh_ui_scalar(0), dy);
    after = lh_ui_entity_container_get_scroll(box);
    if (lh_ui_point_equals(lh_addr_of(before), lh_addr_of(after)))
    {
        return lh_bool_false;
    }
    lh_ui_canvas_reset_damage(self->canvas);
    if (lh_null_ne(bar))
    {
        lh_ui_entity_scrollbar_add_scroll_damage(bar, self->canvas, lh_addr_of(thumb_before));
    }
    else
    {
        lh_ui_entity_add_damage(lh_ui_entity_container_as_entity(box), self->canvas);
    }
    return lh_bool_true;
}
