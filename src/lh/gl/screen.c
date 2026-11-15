/**
 * @file screen.c
 * @brief Open and back on a layer's children. The list is the stack.
 */

#include <lh/gl/screen.h>

#include <lh/core/assert/runtime.h>

lh_gl_layer_t *
lh_gl_screen_get_front(const lh_gl_layer_t *self)
{
    lh_list_node_t *node = lh_list_get_last(lh_gl_layer_get_items(self));

    if (lh_ptr_is_null(node))
    {
        return lh_null;
    }

    lh_gl_entity_t *entity = lh_gl_entity_from_node(node);

    if (!lh_gl_entity_is_layer(entity))
    {
        return lh_null;
    }
    return lh_ptr_cast(lh_gl_layer_t, entity);
}

lh_bool_t
lh_gl_screen_try_open(lh_gl_layer_t *self, lh_gl_layer_t *screen)
{
    lh_assert_runtime_ref(self);
    return lh_gl_layer_try_add(self, lh_gl_layer_get_entity_mut(screen));
}

lh_gl_layer_t *
lh_gl_screen_try_back(lh_gl_layer_t *self)
{
    lh_gl_layer_t *front = lh_gl_screen_get_front(self);

    if (lh_ptr_is_null(front))
    {
        return lh_null;
    }
    if (!lh_gl_layer_try_remove(self, lh_gl_layer_get_entity_mut(front)))
    {
        return lh_null;
    }
    return front;
}

lh_void
lh_gl_screen_open(lh_gl_layer_t *self, lh_gl_layer_t *screen)
{
    lh_runtime_check_ifn(lh_gl_screen_try_open(self, screen),
                         lh_runtime_error_code_not_accepted);
}

lh_gl_layer_t *
lh_gl_screen_back(lh_gl_layer_t *self)
{
    lh_gl_layer_t *screen = lh_gl_screen_try_back(self);

    lh_runtime_check_ifn(lh_ptr_is_set(screen), lh_runtime_error_code_not_accepted);
    return screen;
}
