/**
 * @file render.c
 * @brief Implementation of `lh/gx/render.h`.
 */

#include <lh/gx/render.h>

#include <lh/core/util/return.h>
#include <lh/gx/class/initializer.h>

const lh_gx_class_t lh_gx_render_screen_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_gx_render_report);
const lh_gx_entity_t *
lh_gx_render_get_screen(const lh_gx_render_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->screen);
}

lh_gx_entity_t *
lh_gx_render_get_screen_mut(lh_gx_render_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->screen);
}

const lh_gx_damage_t *
lh_gx_render_get_damage(const lh_gx_render_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->damage);
}

lh_gx_damage_t *
lh_gx_render_get_damage_mut(lh_gx_render_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->damage);
}

const lh_gx_canvas_t *
lh_gx_render_get_canvas(const lh_gx_render_t *self)
{
    lh_assert_runtime_ref(self);
    return self->canvas;
}

lh_void
lh_gx_render_set_canvas(lh_gx_render_t *self, const lh_gx_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    self->canvas = canvas;
}

lh_ptr
lh_gx_render_get_canvas_context(const lh_gx_render_t *self)
{
    lh_assert_runtime_ref(self);
    return self->canvas_context;
}

lh_void
lh_gx_render_set_canvas_context(lh_gx_render_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->canvas_context = context;
}

lh_gx_render_t *
lh_gx_render_from_screen(lh_gx_entity_t *screen)
{
    lh_assert_runtime_ref(screen);
    return lh_ptr_sub_by_offset(lh_gx_render_t, screen, lh_offset_of(lh_gx_render_t, screen));
}

lh_void
lh_gx_render_find_cover(const lh_gx_entity_t *entity, lh_gx_point_t origin,
                        const lh_gx_rect_t *area, const lh_gx_entity_t **best)
{
    lh_return_if(!lh_gx_entity_is_visible(entity));
    lh_gx_rect_t place;

    lh_gx_rect_offset(lh_gx_entity_get_place(entity), lh_gx_point_get_x(lh_addr_of(origin)),
                      lh_gx_point_get_y(lh_addr_of(origin)), lh_addr_of(place));
    lh_gx_rect_t inside;

    lh_gx_rect_intersection(lh_addr_of(place), area, lh_addr_of(inside));
    /* Its children are drawn inside it: what it does not hold whole, none of them can. */
    lh_return_if(!lh_gx_rect_eq(lh_addr_of(inside), area));
    const lh_gx_class_t *klass = lh_gx_entity_get_class(entity);

    if (lh_null_ne(klass))
    {
        lh_gx_class_covers_fn *covers = lh_gx_class_get_covers(klass);

        if (lh_null_ne(lh_ptr_rcast(lh_void, covers)) && covers(entity, lh_addr_of(place), area))
        {
            lh_ptr_deref(best) = entity;
        }
    }
    for (const lh_gx_entity_t *child = lh_gx_entity_get_first_child(entity); lh_null_ne(child);
         child = lh_gx_entity_get_next_sibling(child))
    {
        lh_gx_render_find_cover(child, lh_ptr_deref(lh_gx_rect_get_origin(lh_addr_of(place))), area,
                                best);
    }
}

lh_void
lh_gx_render_add_damage(lh_gx_render_t *self, const lh_gx_rect_t *area)
{
    lh_gx_rect_t on_screen;

    lh_gx_rect_intersection(lh_gx_entity_get_place(lh_gx_render_get_screen(self)), area,
                            lh_addr_of(on_screen));
    lh_return_if(lh_gx_rect_is_empty(lh_addr_of(on_screen)));
    lh_gx_damage_add(lh_gx_render_get_damage_mut(self), lh_addr_of(on_screen));
}

lh_bool_t
lh_gx_render_has_damage(const lh_gx_render_t *self)
{
    return lh_cast_static(lh_bool_t, !lh_gx_damage_is_empty(lh_gx_render_get_damage(self)));
}

lh_void
lh_gx_render_paint_subtree(const lh_gx_render_t *self, const lh_gx_entity_t *entity,
                           lh_gx_point_t origin, const lh_gx_rect_t *clip,
                           const lh_gx_entity_t *cover, lh_bool_t *started)
{
    lh_return_if(!lh_gx_entity_is_visible(entity));
    if (!lh_ptr_deref(started) && entity == cover)
    {
        lh_ptr_deref(started) = lh_bool_true;
    }
    /* Before the cover, only the way down to it is walked: an entity that is not its
       owner is hidden by it whole, with its subtree. */
    lh_return_if(
        !lh_ptr_deref(started) &&
        !lh_gx_node_is_ancestor_of(lh_gx_entity_get_node(entity), lh_gx_entity_get_node(cover)));
    lh_gx_rect_t place;

    lh_gx_rect_offset(lh_gx_entity_get_place(entity), lh_gx_point_get_x(lh_addr_of(origin)),
                      lh_gx_point_get_y(lh_addr_of(origin)), lh_addr_of(place));
    lh_gx_rect_t inside;

    lh_gx_rect_intersection(lh_addr_of(place), clip, lh_addr_of(inside));
    lh_return_if(lh_gx_rect_is_empty(lh_addr_of(inside)));
    const lh_gx_class_t *klass = lh_gx_entity_get_class(entity);

    if (lh_ptr_deref(started) && lh_null_ne(klass))
    {
        lh_gx_class_paint_fn *paint_fn = lh_gx_class_get_paint(klass);

        if (lh_null_ne(lh_ptr_rcast(lh_void, paint_fn)))
        {
            lh_gx_paint_t paint;

            lh_gx_paint_init(lh_addr_of(paint), lh_gx_render_get_canvas(self),
                             lh_gx_render_get_canvas_context(self), inside, place);
            paint_fn(entity, lh_addr_of(paint));
        }
    }
    for (const lh_gx_entity_t *child = lh_gx_entity_get_first_child(entity); lh_null_ne(child);
         child = lh_gx_entity_get_next_sibling(child))
    {
        lh_gx_render_paint_subtree(self, child,
                                   lh_ptr_deref(lh_gx_rect_get_origin(lh_addr_of(place))),
                                   lh_addr_of(inside), cover, started);
    }
}

lh_void
lh_gx_render_take_damage(lh_gx_render_t *self, lh_gx_damage_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gx_render_get_damage(self));
    lh_gx_damage_clear(lh_gx_render_get_damage_mut(self));
}

lh_void
lh_gx_render_report(lh_gx_entity_t *root, const lh_gx_rect_t *area)
{
    lh_gx_render_add_damage(lh_gx_render_from_screen(root), area);
}

lh_void
lh_gx_render_init(lh_gx_render_t *self, const lh_gx_canvas_t *canvas, lh_ptr context,
                  lh_gx_scalar_t width, lh_gx_scalar_t height)
{
    lh_gx_entity_init(lh_gx_render_get_screen_mut(self), lh_addr_of(lh_gx_render_screen_class),
                      lh_null, 0, 0, width, height);
    lh_gx_damage_init(lh_gx_render_get_damage_mut(self));
    lh_gx_render_set_canvas(self, canvas);
    lh_gx_render_set_canvas_context(self, context);
    lh_gx_render_add_damage(self, lh_gx_entity_get_place(lh_gx_render_get_screen(self)));
}

lh_void
lh_gx_render_paint_area(const lh_gx_render_t *self, const lh_gx_rect_t *area)
{
    const lh_gx_entity_t *screen = lh_gx_render_get_screen(self);
    lh_gx_point_t origin;

    lh_gx_point_init(lh_addr_of(origin), 0, 0);
    const lh_gx_entity_t *cover = lh_null;

    lh_gx_render_find_cover(screen, origin, area, lh_addr_of(cover));
    lh_bool_t started = lh_null_eq(cover);

    lh_gx_render_paint_subtree(self, screen, origin, area, cover, lh_addr_of(started));
}

lh_void
lh_gx_render_draw(lh_gx_render_t *self, lh_gx_damage_t *out)
{
    lh_gx_render_take_damage(self, out);
    for (lh_u32_t i = 0U; i < lh_gx_damage_get_count(out); ++i)
    {
        lh_gx_render_paint_area(self, lh_gx_damage_get_rect(out, i));
    }
}
