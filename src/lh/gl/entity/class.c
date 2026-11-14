/**
 * @file class.c
 * @brief Implementation of lh/gl/entity/class.h.
 *
 * Only the four level-1 accessors name a field; everything else goes through
 * them.
 */

#include <lh/core/assert/runtime.h>
#include <lh/gl/entity/class.h>

/* -- Accessors ------------------------------------------------------------- */

lh_gl_entity_class_kind_t
lh_gl_entity_class_get_kind(const lh_gl_entity_class_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_gl_entity_class_set_kind(lh_gl_entity_class_t *self, lh_gl_entity_class_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_gl_entity_class_draw_cb
lh_gl_entity_class_get_draw_cb(const lh_gl_entity_class_t *self)
{
    lh_assert_runtime_ref(self);
    return self->draw_cb;
}

lh_void
lh_gl_entity_class_set_draw_cb(lh_gl_entity_class_t *self, lh_gl_entity_class_draw_cb draw_cb)
{
    lh_assert_runtime_ref(self);
    self->draw_cb = draw_cb;
}

/* -- Init and queries, by level -------------------------------------------- */

lh_void
lh_gl_entity_class_init(lh_gl_entity_class_t *self, lh_gl_entity_class_kind_t kind,
                        lh_gl_entity_class_draw_cb draw_cb)
{
    lh_gl_entity_class_set_kind(self, kind);
    lh_gl_entity_class_set_draw_cb(self, draw_cb);
}

lh_bool_t
lh_gl_entity_class_is_layer(const lh_gl_entity_class_t *self)
{
    return lh_gl_entity_class_get_kind(self) == lh_gl_entity_class_kind_layer;
}