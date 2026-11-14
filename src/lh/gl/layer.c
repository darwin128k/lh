/**
 * @file layer.c
 * @brief Implementation of lh/gl/layer.h.
 *
 * Only the level-1 accessors name a field. The children are an ::lh_list_t of
 * nodes that live in the entities; linking allocates nothing.
 * ::lh_gl_layer_deinit unlinks them and clears each parent.
 */

#include <lh/gl/layer.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/entity/class/initializer.h>

/* -- Accessors ------------------------------------------------------------- */

lh_gl_entity_t *
lh_gl_layer_get_entity_mut(lh_gl_layer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_entity_t *
lh_gl_layer_get_entity(const lh_gl_layer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_list_t *
lh_gl_layer_get_items_mut(lh_gl_layer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->items);
}

const lh_list_t *
lh_gl_layer_get_items(const lh_gl_layer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->items);
}

lh_gl_rect_t *
lh_gl_layer_get_dirty_mut(lh_gl_layer_t *self, lh_uindex_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime(index < LH_LIBRARY_OPTION_GL_LAYER_DIRTY_MAX,
                      lh_runtime_error_code_out_of_range);
    return lh_addr_of(self->dirty[index]);
}

const lh_gl_rect_t *
lh_gl_layer_get_dirty(const lh_gl_layer_t *self, lh_uindex_t index)
{
    lh_assert_runtime(index < lh_gl_layer_get_dirty_count(self),
                      lh_runtime_error_code_out_of_range);
    return lh_addr_of(self->dirty[index]);
}

lh_usize_t
lh_gl_layer_get_dirty_count(const lh_gl_layer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dirty_count;
}

lh_void
lh_gl_layer_set_dirty_count(lh_gl_layer_t *self, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->dirty_count = count;
}

lh_void
lh_gl_layer_clear_dirty(lh_gl_layer_t *self)
{
    lh_gl_layer_set_dirty_count(self, 0);
}

lh_void
lh_gl_layer_invalidate(lh_gl_layer_t *self, const lh_gl_rect_t *area)
{
    const lh_gl_entity_t *entity = lh_gl_layer_get_entity(self);
    const lh_gl_rect_t *bounds = lh_gl_entity_get_rect(entity);
    const lh_gl_size_t *size = lh_gl_rect_get_size(bounds);
    lh_gl_rect_t clip;

    lh_gl_rect_init(lh_addr_of(clip), 0, 0, lh_gl_size_get_width(size),
                    lh_gl_size_get_height(size));
    lh_gl_rect_t dirty;
    lh_gl_rect_intersection(area, lh_addr_of(clip), lh_addr_of(dirty));
    if (lh_gl_rect_is_empty(lh_addr_of(dirty)))
    {
        return;
    }
    lh_gl_layer_t *parent = lh_gl_entity_get_parent(entity);
    if (lh_ptr_is_set(parent))
    {
        const lh_gl_point_t *origin = lh_gl_rect_get_origin(bounds);
        lh_gl_rect_offset(lh_addr_of(dirty), lh_gl_point_get_x(origin), lh_gl_point_get_y(origin),
                          lh_addr_of(dirty));
        lh_gl_layer_invalidate(parent, lh_addr_of(dirty));
        return;
    }
    for (lh_uindex_t i = 0; i < lh_gl_layer_get_dirty_count(self);)
    {
        const lh_gl_rect_t *existing = lh_gl_layer_get_dirty(self, i);
        lh_gl_rect_t united;
        lh_gl_rect_union(existing, lh_addr_of(dirty), lh_addr_of(united));
        if (lh_gl_rect_eq(existing, lh_addr_of(united)))
        {
            return;
        }
        const lh_gl_size_t *a = lh_gl_rect_get_size(existing);
        const lh_gl_size_t *b = lh_gl_rect_get_size(lh_addr_of(dirty));
        const lh_gl_size_t *u = lh_gl_rect_get_size(lh_addr_of(united));
        lh_ullong_t area_a = lh_cast_static(lh_ullong_t, lh_gl_size_get_width(a)) *
                             lh_cast_static(lh_ullong_t, lh_gl_size_get_height(a));
        lh_ullong_t area_b = lh_cast_static(lh_ullong_t, lh_gl_size_get_width(b)) *
                             lh_cast_static(lh_ullong_t, lh_gl_size_get_height(b));
        lh_ullong_t area_u = lh_cast_static(lh_ullong_t, lh_gl_size_get_width(u)) *
                             lh_cast_static(lh_ullong_t, lh_gl_size_get_height(u));
        if (area_u <= area_a + area_b)
        {
            dirty = united;
            lh_usize_t count = lh_gl_layer_get_dirty_count(self) - 1;
            if (i != count)
            {
                lh_ptr_deref(lh_gl_layer_get_dirty_mut(self, i)) =
                    lh_ptr_deref(lh_gl_layer_get_dirty(self, count));
            }
            lh_gl_layer_set_dirty_count(self, count);
            i = 0;
        }
        else
        {
            ++i;
        }
    }
    lh_usize_t count = lh_gl_layer_get_dirty_count(self);
    if (count == LH_LIBRARY_OPTION_GL_LAYER_DIRTY_MAX)
    {
        for (lh_uindex_t i = 0; i < count; ++i)
        {
            lh_gl_rect_union(lh_gl_layer_get_dirty(self, i), lh_addr_of(dirty), lh_addr_of(dirty));
        }
        count = 0;
    }
    lh_ptr_deref(lh_gl_layer_get_dirty_mut(self, count)) = dirty;
    lh_gl_layer_set_dirty_count(self, count + 1);
}

/* -- Init and queries, by level -------------------------------------------- */

const lh_gl_entity_class_t *
lh_gl_layer_get_class(void)
{
    static const lh_gl_entity_class_t layer_class =
        lh_gl_entity_class_initializer(lh_gl_entity_class_kind_layer, lh_null);

    return lh_addr_of(layer_class);
}

lh_usize_t
lh_gl_layer_get_count(const lh_gl_layer_t *self)
{
    return lh_list_get_size(lh_gl_layer_get_items(self));
}

lh_void
lh_gl_layer_init(lh_gl_layer_t *self, lh_gl_rect_t rect)
{
    lh_gl_entity_init(lh_gl_layer_get_entity_mut(self), rect, lh_gl_layer_get_class());
    lh_list_init(lh_gl_layer_get_items_mut(self));
    lh_gl_layer_clear_dirty(self);
}

lh_bool_t
lh_gl_layer_is_empty(const lh_gl_layer_t *self)
{
    return lh_list_is_empty(lh_gl_layer_get_items(self));
}

lh_bool_t
lh_gl_layer_try_find(const lh_gl_layer_t *self, const lh_gl_entity_t *entity,
                     lh_uindex_t *out_index)
{
    lh_assert_runtime_ref(entity);
    lh_assert_runtime_ref(out_index);

    const lh_uindex_t index =
        lh_list_index_of(lh_gl_layer_get_items(self), lh_gl_entity_get_node(entity));

    if (index == LH_LIST_INVALID)
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out_index) = index;
    return lh_bool_true;
}

lh_bool_t
lh_gl_layer_is_within(const lh_gl_layer_t *self, const lh_gl_entity_t *other)
{
    const lh_gl_entity_t *current = lh_gl_layer_get_entity(self);

    lh_assert_runtime_ref(other);
    for (;;)
    {
        if (lh_ptr_eq(current, other))
        {
            return lh_bool_true;
        }
        const lh_gl_layer_t *parent = lh_gl_entity_get_parent(current);

        if (lh_ptr_is_null(parent))
        {
            return lh_bool_false;
        }
        current = lh_gl_layer_get_entity(parent);
    }
}

lh_void
lh_gl_layer_deinit(lh_gl_layer_t *self)
{
    lh_list_t *items = lh_gl_layer_get_items_mut(self);

    for (lh_list_node_t *node = lh_list_get_first(items); lh_ptr_is_set(node);
         node = lh_list_get_next(items, node))
    {
        lh_gl_entity_set_parent(lh_gl_entity_from_node(node), lh_null);
    }
    lh_list_clear(items);
}

lh_bool_t
lh_gl_layer_is_valid_index(const lh_gl_layer_t *self, lh_uindex_t index)
{
    return index < lh_gl_layer_get_count(self);
}

lh_bool_t
lh_gl_layer_try_add(lh_gl_layer_t *self, lh_gl_entity_t *entity)
{
    if (!lh_gl_entity_is_root(entity) || lh_gl_layer_is_within(self, entity))
    {
        return lh_bool_false;
    }
    lh_list_push_back(lh_gl_layer_get_items_mut(self), lh_gl_entity_get_node_mut(entity));
    lh_gl_entity_set_parent(entity, self);
    return lh_bool_true;
}

lh_gl_entity_t *
lh_gl_layer_get_item(const lh_gl_layer_t *self, lh_uindex_t index)
{
    lh_assert_runtime(lh_gl_layer_is_valid_index(self, index), lh_runtime_error_code_out_of_range);
    return lh_gl_entity_from_node(lh_list_get_at(lh_gl_layer_get_items(self), index));
}

lh_void
lh_gl_layer_add(lh_gl_layer_t *self, lh_gl_entity_t *entity)
{
    lh_runtime_check_ifn(lh_gl_layer_try_add(self, entity), lh_runtime_error_code_not_accepted);
}

lh_void
lh_gl_layer_remove_at(lh_gl_layer_t *self, lh_uindex_t index)
{
    lh_gl_entity_t *entity = lh_gl_layer_get_item(self, index);

    lh_list_node_unlink(lh_gl_entity_get_node_mut(entity));
    lh_gl_entity_set_parent(entity, lh_null);
}

lh_bool_t
lh_gl_layer_try_remove(lh_gl_layer_t *self, lh_gl_entity_t *entity)
{
    lh_uindex_t index = 0;

    if (!lh_gl_layer_try_find(self, entity, lh_addr_of(index)))
    {
        return lh_bool_false;
    }
    lh_gl_layer_remove_at(self, index);
    return lh_bool_true;
}
