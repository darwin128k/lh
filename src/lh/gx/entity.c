/**
 * @file entity.c
 * @brief Implementation of `lh/gx/entity.h`.
 */

#include <lh/gx/entity.h>

#include <lh/core/util/return.h>
#include <lh/gx/class.h>

const lh_gx_node_t *
lh_gx_entity_get_node(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_gx_node_t *
lh_gx_entity_get_node_mut(lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

const lh_gx_rect_t *
lh_gx_entity_get_place(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->place);
}

lh_void
lh_gx_entity_set_place(lh_gx_entity_t *self, lh_gx_rect_t place)
{
    lh_assert_runtime_ref(self);
    self->place = place;
}

lh_bool_t
lh_gx_entity_is_visible(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->visible;
}

lh_void
lh_gx_entity_set_visible(lh_gx_entity_t *self, lh_bool_t visible)
{
    lh_assert_runtime_ref(self);
    self->visible = visible;
}

const struct lh_gx_class *
lh_gx_entity_get_class(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->klass;
}

lh_void
lh_gx_entity_set_class(lh_gx_entity_t *self, const struct lh_gx_class *klass)
{
    lh_assert_runtime_ref(self);
    self->klass = klass;
}

const struct lh_gx_style *
lh_gx_entity_get_style(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_gx_entity_set_style(lh_gx_entity_t *self, const struct lh_gx_style *style)
{
    lh_assert_runtime_ref(self);
    self->style = style;
}

lh_ptr
lh_gx_entity_get_content(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->content;
}

lh_void
lh_gx_entity_set_content(lh_gx_entity_t *self, lh_ptr content)
{
    lh_assert_runtime_ref(self);
    self->content = content;
}

lh_gx_entity_t *
lh_gx_entity_from_node(const lh_gx_node_t *node)
{
    lh_return_if(lh_null_eq(node), lh_null);
    return lh_list_entry(lh_gx_entity_t, node.link, lh_gx_node_get_link(node));
}

lh_void
lh_gx_entity_init(lh_gx_entity_t *self, const struct lh_gx_class *klass,
                  const struct lh_gx_style *style, lh_gx_scalar_t x, lh_gx_scalar_t y,
                  lh_gx_scalar_t width, lh_gx_scalar_t height)
{
    lh_gx_node_init(lh_gx_entity_get_node_mut(self));
    lh_gx_rect_t place;

    lh_gx_rect_init(lh_addr_of(place), x, y, width, height);
    lh_gx_entity_set_place(self, place);
    lh_gx_entity_set_visible(self, lh_bool_true);
    lh_gx_entity_set_class(self, klass);
    lh_gx_entity_set_style(self, style);
    lh_gx_entity_set_content(self, lh_null);
}

lh_gx_entity_t *
lh_gx_entity_get_owner(const lh_gx_entity_t *self)
{
    return lh_gx_entity_from_node(lh_gx_node_get_owner(lh_gx_entity_get_node(self)));
}

lh_gx_entity_t *
lh_gx_entity_get_first_child(const lh_gx_entity_t *self)
{
    return lh_gx_entity_from_node(lh_gx_node_get_first_child(lh_gx_entity_get_node(self)));
}

lh_gx_entity_t *
lh_gx_entity_get_next_sibling(const lh_gx_entity_t *self)
{
    return lh_gx_entity_from_node(lh_gx_node_get_next_sibling(lh_gx_entity_get_node(self)));
}

lh_gx_entity_t *
lh_gx_entity_get_root(const lh_gx_entity_t *self)
{
    return lh_gx_entity_from_node(lh_gx_node_get_root(lh_gx_entity_get_node(self)));
}

lh_gx_entity_t *
lh_gx_entity_get_next_skip(const lh_gx_entity_t *self, const lh_gx_entity_t *top)
{
    return lh_gx_entity_from_node(
        lh_gx_node_get_next_skip(lh_gx_entity_get_node(self), lh_gx_entity_get_node(top)));
}

lh_bool_t
lh_gx_entity_is_shown(const lh_gx_entity_t *self)
{
    for (const lh_gx_entity_t *up = self; lh_null_ne(up); up = lh_gx_entity_get_owner(up))
    {
        lh_return_if(!lh_gx_entity_is_visible(up), lh_bool_false);
    }
    return lh_bool_true;
}

lh_void
lh_gx_entity_get_screen_rect(const lh_gx_entity_t *self, lh_gx_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gx_entity_get_place(self));
    for (const lh_gx_entity_t *up = lh_gx_entity_get_owner(self); lh_null_ne(up);
         up = lh_gx_entity_get_owner(up))
    {
        const lh_gx_point_t *origin = lh_gx_rect_get_origin(lh_gx_entity_get_place(up));
        const lh_gx_rect_t inner = lh_ptr_deref(out);

        lh_gx_rect_offset(lh_addr_of(inner), lh_gx_point_get_x(origin), lh_gx_point_get_y(origin),
                          out);
    }
}

lh_void
lh_gx_entity_report(const lh_gx_entity_t *self, const lh_gx_rect_t *area)
{
    lh_gx_entity_t *root = lh_gx_entity_get_root(self);
    const lh_gx_class_t *klass = lh_gx_entity_get_class(root);

    lh_return_if(lh_null_eq(klass));
    lh_gx_class_report_fn *report = lh_gx_class_get_report(klass);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, report)));
    report(root, area);
}

lh_void
lh_gx_entity_invalidate(const lh_gx_entity_t *self)
{
    lh_return_if(!lh_gx_entity_is_shown(self));
    lh_gx_rect_t area;

    lh_gx_entity_get_screen_rect(self, lh_addr_of(area));
    lh_gx_entity_report(self, lh_addr_of(area));
}

lh_void
lh_gx_entity_get_owner_origin(const lh_gx_entity_t *self, lh_gx_point_t *out)
{
    const lh_gx_entity_t *owner = lh_gx_entity_get_owner(self);

    if (lh_null_eq(owner))
    {
        lh_gx_point_init(out, 0, 0);
        return;
    }
    lh_gx_rect_t place;

    lh_gx_entity_get_screen_rect(owner, lh_addr_of(place));
    lh_ptr_deref(out) = lh_ptr_deref(lh_gx_rect_get_origin(lh_addr_of(place)));
}

lh_void
lh_gx_entity_get_owner_clip(const lh_gx_entity_t *self, const lh_gx_rect_t *area, lh_gx_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(area);
    const lh_gx_entity_t *up = lh_gx_entity_get_owner(self);

    lh_return_if(lh_null_eq(up));
    lh_gx_rect_t on_screen;

    lh_gx_entity_get_screen_rect(up, lh_addr_of(on_screen));
    while (lh_null_ne(up))
    {
        const lh_gx_rect_t clip = lh_ptr_deref(out);

        lh_gx_rect_intersection(lh_addr_of(clip), lh_addr_of(on_screen), out);
        const lh_gx_entity_t *owner = lh_gx_entity_get_owner(up);

        if (lh_null_ne(owner))
        {
            /* The owner's screen position is this one's less this one's own place. */
            const lh_gx_point_t *at = lh_gx_rect_get_origin(lh_addr_of(on_screen));
            const lh_gx_point_t *local = lh_gx_rect_get_origin(lh_gx_entity_get_place(up));
            lh_gx_point_t origin;

            lh_gx_point_init(lh_addr_of(origin), lh_gx_point_get_x(at) - lh_gx_point_get_x(local),
                             lh_gx_point_get_y(at) - lh_gx_point_get_y(local));
            lh_gx_rect_init_origin_size(
                lh_addr_of(on_screen), origin,
                lh_ptr_deref(lh_gx_rect_get_size(lh_gx_entity_get_place(owner))));
        }
        up = owner;
    }
}

lh_void
lh_gx_entity_attach(lh_gx_entity_t *owner, lh_gx_entity_t *child)
{
    lh_gx_node_attach(lh_gx_entity_get_node_mut(owner), lh_gx_entity_get_node_mut(child));
    lh_gx_entity_invalidate(child);
}

lh_void
lh_gx_entity_attach_v(lh_gx_entity_t *owner, lh_gx_entity_t *child)
{
    lh_gx_node_attach_v(lh_gx_entity_get_node_mut(owner), lh_gx_entity_get_node_mut(child));
    lh_gx_entity_invalidate(child);
}

lh_void
lh_gx_entity_detach(lh_gx_entity_t *self)
{
    lh_gx_entity_invalidate(self);
    lh_gx_node_detach(lh_gx_entity_get_node_mut(self));
}

lh_void
lh_gx_entity_move(lh_gx_entity_t *self, lh_gx_rect_t place)
{
    lh_gx_entity_invalidate(self);
    lh_gx_entity_set_place(self, place);
    lh_gx_entity_invalidate(self);
}

lh_void
lh_gx_entity_set_shown(lh_gx_entity_t *self, lh_bool_t visible)
{
    lh_return_if(lh_gx_entity_is_visible(self) == visible);
    /* Hidden, the entity reports nothing: so the area is reported while it is still
       shown, and after it becomes shown. */
    lh_gx_entity_invalidate(self);
    lh_gx_entity_set_visible(self, visible);
    lh_gx_entity_invalidate(self);
}
