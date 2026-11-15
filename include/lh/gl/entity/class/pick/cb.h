/**
 * @file cb.h
 * @brief Pointer type for how an entity answers a press.
 */

#ifndef LH_GL_ENTITY_CLASS_PICK_CB_H
#define LH_GL_ENTITY_CLASS_PICK_CB_H

#include <lh/core/util/ptr.h>
#include <lh/gl/entity/class/pick/fn.h>

/**
 * @def lh_gl_entity_class_pick_cb
 * @brief Pointer to ::lh_gl_entity_class_pick_fn.
 */
#define lh_gl_entity_class_pick_cb lh_ptr_of(lh_gl_entity_class_pick_fn)

#endif /* LH_GL_ENTITY_CLASS_PICK_CB_H */
