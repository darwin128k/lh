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

const lh_gx_entity_t *
lh_gx_render_find_cover(const lh_gx_render_t *self, const lh_gx_rect_t *area)
{
    const lh_gx_entity_t *screen = lh_gx_render_get_screen(self);
    const lh_gx_entity_t *best = lh_null;
    lh_gx_point_t origin;

    lh_gx_point_init(lh_addr_of(origin), 0, 0);
    /* Carried to the next step: where the children of this entity start. */
    lh_gx_rect_t place;

    for (const lh_gx_entity_t *at = screen; lh_null_ne(at);)
    {
        lh_bool_t descend = lh_bool_false;

        if (lh_gx_entity_is_visible(at))
        {
            lh_gx_rect_offset(lh_gx_entity_get_place(at), lh_gx_point_get_x(lh_addr_of(origin)),
                              lh_gx_point_get_y(lh_addr_of(origin)), lh_addr_of(place));
            lh_gx_rect_t inside;

            lh_gx_rect_intersection(lh_addr_of(place), area, lh_addr_of(inside));
            /* Its children are drawn inside it: what it does not hold whole, none can. */
            if (lh_gx_rect_eq(lh_addr_of(inside), area))
            {
                const lh_gx_class_t *klass = lh_gx_entity_get_class(at);

                if (lh_null_ne(klass))
                {
                    lh_gx_class_covers_fn *covers = lh_gx_class_get_covers(klass);

                    if (lh_null_ne(lh_ptr_rcast(lh_void, covers)) &&
                        covers(at, lh_addr_of(place), area))
                    {
                        best = at;
                    }
                }
                descend = lh_cast_static(lh_bool_t, lh_null_ne(lh_gx_entity_get_first_child(at)));
            }
        }
        const lh_gx_entity_t *next =
            descend ? lh_gx_entity_get_first_child(at) : lh_gx_entity_get_next_skip(at, screen);

        if (lh_null_eq(next))
        {
            break;
        }
        if (lh_gx_entity_get_owner(next) == at)
        {
            origin = lh_ptr_deref(lh_gx_rect_get_origin(lh_addr_of(place)));
        }
        else if (lh_gx_entity_get_owner(next) != lh_gx_entity_get_owner(at))
        {
            lh_gx_entity_get_owner_origin(next, lh_addr_of(origin));
        }
        at = next;
    }
    return best;
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
lh_gx_render_take_damage(lh_gx_render_t *self, lh_gx_damage_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gx_render_get_damage(self));
    lh_gx_damage_clear(lh_gx_render_get_damage_mut(self));
}

lh_void
lh_gx_render_paint_entity(const lh_gx_render_t *self, const lh_gx_entity_t *entity,
                          const lh_gx_rect_t *place, const lh_gx_rect_t *clip)
{
    const lh_gx_class_t *klass = lh_gx_entity_get_class(entity);

    lh_return_if(lh_null_eq(klass));
    lh_gx_class_paint_fn *paint_fn = lh_gx_class_get_paint(klass);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, paint_fn)));
    lh_gx_paint_t paint;

    lh_gx_paint_init(lh_addr_of(paint), lh_gx_render_get_canvas(self),
                     lh_gx_render_get_canvas_context(self), lh_ptr_deref(clip),
                     lh_ptr_deref(place));
    paint_fn(entity, lh_addr_of(paint));
}

lh_void
lh_gx_render_paint_from(const lh_gx_render_t *self, const lh_gx_rect_t *area,
                        const lh_gx_entity_t *cover)
{
    const lh_gx_entity_t *screen = lh_gx_render_get_screen(self);
    lh_bool_t started = lh_null_eq(cover);
    lh_gx_rect_t clip = lh_ptr_deref(area);
    lh_gx_point_t origin;

    lh_gx_point_init(lh_addr_of(origin), 0, 0);
    /* Carried from one step of the walk to the next: the place and the clip of the
       entity just visited are where its children start. */
    lh_gx_rect_t place;
    lh_gx_rect_t inside;

    for (const lh_gx_entity_t *at = screen; lh_null_ne(at);)
    {
        lh_bool_t descend = lh_bool_false;

        if (lh_gx_entity_is_visible(at))
        {
            if (!started && at == cover)
            {
                started = lh_bool_true;
            }
            /* Before the cover, only the way down to it is walked: anything else is
               under it, whole, with its subtree. */
            if (started ||
                lh_gx_node_is_ancestor_of(lh_gx_entity_get_node(at), lh_gx_entity_get_node(cover)))
            {
                lh_gx_rect_offset(lh_gx_entity_get_place(at), lh_gx_point_get_x(lh_addr_of(origin)),
                                  lh_gx_point_get_y(lh_addr_of(origin)), lh_addr_of(place));
                lh_gx_rect_intersection(lh_addr_of(place), lh_addr_of(clip), lh_addr_of(inside));
                if (!lh_gx_rect_is_empty(lh_addr_of(inside)))
                {
                    if (started)
                    {
                        lh_gx_render_paint_entity(self, at, lh_addr_of(place), lh_addr_of(inside));
                    }
                    descend =
                        lh_cast_static(lh_bool_t, lh_null_ne(lh_gx_entity_get_first_child(at)));
                }
            }
        }
        const lh_gx_entity_t *next =
            descend ? lh_gx_entity_get_first_child(at) : lh_gx_entity_get_next_skip(at, screen);

        if (lh_null_eq(next))
        {
            break;
        }
        if (lh_gx_entity_get_owner(next) == at)
        {
            /* Down: the children draw inside this one. */
            origin = lh_ptr_deref(lh_gx_rect_get_origin(lh_addr_of(place)));
            clip = inside;
        }
        else if (lh_gx_entity_get_owner(next) != lh_gx_entity_get_owner(at))
        {
            /* Up and across: what was carried down is found again from the owners. */
            lh_gx_entity_get_owner_origin(next, lh_addr_of(origin));
            lh_gx_entity_get_owner_clip(next, area, lh_addr_of(clip));
        }
        at = next;
    }
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
    lh_gx_render_paint_from(self, area, lh_gx_render_find_cover(self, area));
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
