/**
 * @file cb.h
 * @brief Pointer type for how an entity draws itself.
 */

#ifndef LH_GL_ENTITY_CLASS_DRAW_CB_H
#define LH_GL_ENTITY_CLASS_DRAW_CB_H

#include <lh/core/util/ptr.h>
#include <lh/gl/entity/class/draw/fn.h>

/**
 * @def lh_gl_entity_class_draw_cb
 * @brief Pointer to ::lh_gl_entity_class_draw_fn.
 */
#define lh_gl_entity_class_draw_cb lh_ptr_of(lh_gl_entity_class_draw_fn)

#endif /* LH_GL_ENTITY_CLASS_DRAW_CB_H */