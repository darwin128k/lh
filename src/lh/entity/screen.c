#include <lh/entity/screen.h>
#include <lh/assert.h>
#include <lh/float/round.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

static lh_void
lh_entity_screen_construct(lh_entity_t *self)
{
    lh_entity_rect_set_color(lh_ptr_rcast(lh_entity_rect_t, self), lh_ui_color_make(0, 0, 0, 255));
}

const lh_entity_class_t lh_entity_screen_class =
    lh_entity_class_initializer(&lh_entity_rect_class, sizeof(lh_entity_screen_t),
                                lh_entity_screen_construct, lh_null, lh_null);

lh_void
lh_entity_screen_invalidate_area(lh_entity_screen_t *self, lh_ui_rect_t area)
{
    lh_assert_runtime_ref(self);
    const lh_vec2_t size = lh_entity_rect_get_size(lh_ptr_rcast(const lh_entity_rect_t, self));
    const lh_ui_rect_t screen =
        lh_ui_rect_make(0, 0, lh_float_ceil_to_int(size.x), lh_float_ceil_to_int(size.y));
    lh_ui_rect_t dirty = lh_ui_rect_intersection(lh_addr_of(screen), lh_addr_of(area));
    if (lh_ui_rect_is_empty(lh_addr_of(dirty)))
    {
        return;
    }

    for (lh_usize_t i = 0; i < self->dirty_count; ++i)
    {
        if (lh_ui_rect_intersects(lh_addr_of(self->dirty[i]), lh_addr_of(dirty)))
        {
            self->dirty[i] = lh_ui_rect_union(lh_addr_of(self->dirty[i]), lh_addr_of(dirty));
            return;
        }
    }

    if (self->dirty_count == LH_ENTITY_SCREEN_DIRTY_MAX)
    {
        for (lh_usize_t i = 1; i < self->dirty_count; ++i)
        {
            self->dirty[0] =
                lh_ui_rect_union(lh_addr_of(self->dirty[0]), lh_addr_of(self->dirty[i]));
        }
        self->dirty[0] = lh_ui_rect_union(lh_addr_of(self->dirty[0]), lh_addr_of(dirty));
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

lh_ui_rect_t
lh_entity_screen_get_dirty_area(const lh_entity_screen_t *self, lh_usize_t index)
{
    lh_assert_runtime_ifn(index < lh_entity_screen_get_dirty_count(self),
                          lh_runtime_error_code_out_of_range);
    return self->dirty[index];
}

/* Draw @p entity and its children onto @p canvas within @p clip. */
static lh_void
lh_entity_screen_draw(lh_entity_t *entity, lh_ui_canvas_t *canvas, lh_ui_rect_t clip)
{
    if (lh_entity_has_flags(entity, LH_ENTITY_FLAG_HIDDEN))
    {
        return;
    }

    lh_ui_canvas_set_clip(canvas, clip);
    lh_entity_notify(entity, LH_ENTITY_EVENT_DRAW, canvas);

    lh_ui_rect_t child_clip = clip;
    if (lh_entity_is_instance_of(entity, lh_addr_of(lh_entity_rect_class)) &&
        !lh_entity_has_flags(entity, LH_ENTITY_FLAG_OVERFLOW_VISIBLE))
    {
        const lh_ui_rect_t bounds =
            lh_entity_rect_get_screen_bounds(lh_ptr_rcast(const lh_entity_rect_t, entity));
        child_clip = lh_ui_rect_intersection(lh_addr_of(clip), lh_addr_of(bounds));
        if (lh_ui_rect_is_empty(lh_addr_of(child_clip)))
        {
            return;
        }
    }

    for (lh_entity_t *child = lh_entity_get_first_child(entity); lh_ptr_is_set(child);
         child = lh_entity_get_next_sibling(child))
    {
        lh_entity_screen_draw(child, canvas, child_clip);
    }
}

lh_ui_rect_t
lh_entity_screen_render(lh_entity_screen_t *self, lh_ui_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(canvas);

    lh_ui_rect_t drawn = lh_ui_rect_zero();
    for (lh_usize_t i = 0; i < lh_entity_screen_get_dirty_count(self); ++i)
    {
        const lh_ui_rect_t area = lh_entity_screen_get_dirty_area(self, i);
        lh_entity_screen_draw(lh_ptr_rcast(lh_entity_t, self), canvas, area);
        drawn = lh_ui_rect_union(lh_addr_of(drawn), lh_addr_of(area));
    }
    self->dirty_count = 0;
    return drawn;
}

/* Mark what @p entity and its descendants cover. A rectangle that cuts its
 * children covers them too, so the walk stops there. */
static lh_void
lh_entity_screen_invalidate_tree(lh_entity_screen_t *screen, lh_entity_t *entity)
{
    if (lh_entity_is_instance_of(entity, lh_addr_of(lh_entity_rect_class)))
    {
        lh_entity_screen_invalidate_area(
            screen, lh_entity_rect_get_screen_bounds(lh_ptr_rcast(const lh_entity_rect_t, entity)));
        if (!lh_entity_has_flags(entity, LH_ENTITY_FLAG_OVERFLOW_VISIBLE))
        {
            return;
        }
    }
    for (lh_entity_t *child = lh_entity_get_first_child(entity); lh_ptr_is_set(child);
         child = lh_entity_get_next_sibling(child))
    {
        lh_entity_screen_invalidate_tree(screen, child);
    }
}

lh_void
lh_entity_invalidate(lh_entity_t *self)
{
    lh_assert_runtime_ref(self);
    lh_entity_t *root = self;
    for (lh_entity_t *parent = lh_entity_get_parent(root); lh_ptr_is_set(parent);
         parent = lh_entity_get_parent(parent))
    {
        root = parent;
    }
    if (lh_entity_is_instance_of(root, lh_addr_of(lh_entity_screen_class)))
    {
        lh_entity_screen_invalidate_tree(lh_ptr_rcast(lh_entity_screen_t, root), self);
    }
}
