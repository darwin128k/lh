/**
 * @file surface.c
 * @brief Fixed-storage surface and its borrowed root layer.
 */
#include <lh/gl/surface.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/layer.h>

lh_gl_size_t *
lh_gl_surface_get_size_mut(lh_gl_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

const lh_gl_size_t *
lh_gl_surface_get_size(const lh_gl_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

lh_gl_layer_t *
lh_gl_surface_get_root(const lh_gl_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root;
}

lh_void
lh_gl_surface_set_root(lh_gl_surface_t *self, lh_gl_layer_t *root)
{
    lh_assert_runtime_ref(self);
    self->root = root;
}

lh_gl_rect_t *
lh_gl_surface_get_dirty_mut(lh_gl_surface_t *self, lh_uindex_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime(index < LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX,
                      lh_runtime_error_code_out_of_range);
    return lh_addr_of(self->dirty[index]);
}

lh_usize_t
lh_gl_surface_get_dirty_count(const lh_gl_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dirty_count;
}

lh_void
lh_gl_surface_set_dirty_count(lh_gl_surface_t *self, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->dirty_count = count;
}

const lh_gl_rect_t *
lh_gl_surface_get_dirty(const lh_gl_surface_t *self, lh_uindex_t index)
{
    lh_assert_runtime(index < lh_gl_surface_get_dirty_count(self),
                      lh_runtime_error_code_out_of_range);
    return lh_addr_of(self->dirty[index]);
}

lh_void
lh_gl_surface_clear_dirty(lh_gl_surface_t *self)
{
    lh_gl_surface_set_dirty_count(self, 0);
}

lh_void
lh_gl_surface_init(lh_gl_surface_t *self, lh_gl_size_t size)
{
    lh_ptr_deref(lh_gl_surface_get_size_mut(self)) = size;
    lh_gl_surface_set_root(self, lh_null);
    lh_gl_surface_clear_dirty(self);
}

lh_void
lh_gl_surface_detach(lh_gl_surface_t *self)
{
    lh_gl_layer_t *root = lh_gl_surface_get_root(self);
    if (lh_ptr_is_set(root))
    {
        lh_gl_layer_set_surface(root, lh_null);
        lh_gl_surface_set_root(self, lh_null);
    }
}

lh_void
lh_gl_surface_resize(lh_gl_surface_t *self, lh_gl_size_t size)
{
    lh_ptr_deref(lh_gl_surface_get_size_mut(self)) = size;
    lh_gl_rect_t bounds;
    lh_gl_rect_init(lh_addr_of(bounds), 0, 0, lh_gl_size_get_width(lh_addr_of(size)),
                    lh_gl_size_get_height(lh_addr_of(size)));
    lh_gl_layer_t *root = lh_gl_surface_get_root(self);
    if (lh_ptr_is_set(root))
    {
        lh_gl_entity_set_rect(lh_gl_layer_get_entity_mut(root), bounds);
    }
    lh_gl_surface_clear_dirty(self);
    lh_gl_surface_invalidate(self, lh_addr_of(bounds));
}

lh_void
lh_gl_surface_attach(lh_gl_surface_t *self, lh_gl_layer_t *root)
{
    lh_runtime_check_ifn(lh_gl_entity_is_root(lh_gl_layer_get_entity(root)) &&
                             lh_ptr_is_null(lh_gl_layer_get_surface(root)),
                         lh_runtime_error_code_not_accepted);
    lh_gl_surface_detach(self);
    lh_gl_surface_set_root(self, root);
    lh_gl_layer_set_surface(root, self);
    lh_gl_surface_resize(self, lh_ptr_deref(lh_gl_surface_get_size(self)));
}

lh_void
lh_gl_surface_invalidate(lh_gl_surface_t *self, const lh_gl_rect_t *area)
{
    const lh_gl_size_t *size = lh_gl_surface_get_size(self);
    lh_gl_rect_t clip;
    lh_gl_rect_init(lh_addr_of(clip), 0, 0, lh_gl_size_get_width(size),
                    lh_gl_size_get_height(size));
    lh_gl_rect_t dirty;
    lh_gl_rect_intersection(area, lh_addr_of(clip), lh_addr_of(dirty));
    if (lh_gl_rect_is_empty(lh_addr_of(dirty)))
    {
        return;
    }
    for (lh_uindex_t i = 0; i < lh_gl_surface_get_dirty_count(self);)
    {
        const lh_gl_rect_t *existing = lh_gl_surface_get_dirty(self, i);
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
            lh_usize_t count = lh_gl_surface_get_dirty_count(self) - 1;
            if (i != count)
            {
                lh_ptr_deref(lh_gl_surface_get_dirty_mut(self, i)) =
                    lh_ptr_deref(lh_gl_surface_get_dirty(self, count));
            }
            lh_gl_surface_set_dirty_count(self, count);
            i = 0;
        }
        else
        {
            ++i;
        }
    }
    lh_usize_t count = lh_gl_surface_get_dirty_count(self);
    if (count == LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX)
    {
        for (lh_uindex_t i = 0; i < count; ++i)
        {
            lh_gl_rect_union(lh_gl_surface_get_dirty(self, i), lh_addr_of(dirty),
                             lh_addr_of(dirty));
        }
        count = 0;
    }
    lh_ptr_deref(lh_gl_surface_get_dirty_mut(self, count)) = dirty;
    lh_gl_surface_set_dirty_count(self, count + 1);
}
