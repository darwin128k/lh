/**
 * @file damage.c
 * @brief Implementation of `lh/gl/scrollbar.h`: the damage a moved bar or a scrolled container
 * leaves.
 */

#include <lh/gl/scrollbar.h>

#include <lh/core/util/return.h>

lh_void
lh_gl_scrollbar_add_bound_damage(lh_gl_entity_t *entity, const lh_gl_container_t *container,
                                 lh_gl_canvas_t *canvas)
{
    lh_gl_scrollbar_t *bar = lh_gl_scrollbar_get_bound(entity, container);

    lh_return_if(lh_null_eq(bar));
    lh_gl_entity_add_damage(lh_gl_scrollbar_as_entity(bar), canvas);
}

lh_void
lh_gl_scrollbar_add_scroll_damage(lh_gl_container_t *container, lh_gl_canvas_t *canvas)
{
    lh_gl_entity_t *entity = lh_gl_container_as_entity(container);
    lh_gl_entity_t *parent = lh_gl_entity_get_parent(entity);
    lh_gl_entity_t *child;

    lh_gl_entity_add_damage(entity, canvas);
    lh_return_if(lh_null_eq(parent));
    for (child = lh_gl_entity_get_first_child(parent); lh_null_ne(child);
         child = lh_gl_entity_get_next_child(parent, child))
    {
        lh_gl_scrollbar_add_bound_damage(child, container, canvas);
    }
}
