/**
 * @file cb.h
 * @brief Pointer to ::lh_gl_entity_visit_fn.
 */

#ifndef LH_GL_ENTITY_VISIT_CB_H
#define LH_GL_ENTITY_VISIT_CB_H

#include <lh/gl/entity/visit/fn.h>
#include <lh/core/util/ptr.h>

/**
 * @def lh_gl_entity_visit_cb
 * @brief Pointer to ::lh_gl_entity_visit_fn.
 */
#define lh_gl_entity_visit_cb lh_ptr_of(lh_gl_entity_visit_fn)

#endif /* LH_GL_ENTITY_VISIT_CB_H */
