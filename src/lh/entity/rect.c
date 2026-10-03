#include <lh/entity/rect.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

const lh_entity_class_t lh_entity_rect_class = lh_entity_class_initializer(
    &lh_entity_2d_class, sizeof(lh_entity_rect_t), lh_null, lh_null, lh_null);

lh_vec2_t
lh_entity_rect_get_size(const lh_entity_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_void
lh_entity_rect_set_size(lh_entity_rect_t *self, lh_vec2_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

lh_bool_t
lh_entity_rect_contains(const lh_entity_rect_t *self, lh_vec2_t point)
{
    lh_mat4_t to_local;
    if (!lh_mat4_inverse(lh_entity_2d_get_world_matrix(lh_ptr_rcast(const lh_entity_2d_t, self)),
                         lh_addr_of(to_local)))
    {
        return lh_bool_false; /* scaled to nothing: covers no area */
    }

    const lh_vec3_t local = lh_mat4_transform_point(to_local, lh_vec3_make(point.x, point.y, 0.0f));
    const lh_vec2_t size = lh_entity_rect_get_size(self);
    return local.x >= 0.0f && local.x < size.x && local.y >= 0.0f && local.y < size.y;
}

lh_entity_t *
lh_entity_rect_find_at(lh_entity_t *root, lh_vec2_t point)
{
    /* Children first, the youngest hit winning; the entity itself only when
     * no child is hit: that is the drawing order read backwards. */
    lh_entity_t *hit = lh_null;
    for (lh_entity_t *child = lh_entity_get_first_child(root); lh_ptr_is_set(child);
         child = lh_entity_get_next_sibling(child))
    {
        lh_entity_t *const child_hit = lh_entity_rect_find_at(child, point);
        if (lh_ptr_is_set(child_hit))
        {
            hit = child_hit;
        }
    }
    if (lh_ptr_is_set(hit))
    {
        return hit;
    }

    if (lh_entity_is_instance_of(root, lh_addr_of(lh_entity_rect_class)) &&
        lh_entity_rect_contains(lh_ptr_rcast(const lh_entity_rect_t, root), point))
    {
        return root;
    }
    return lh_null;
}
