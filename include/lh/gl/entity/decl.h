/**
 * @file decl.h
 * @brief Forward declaration of ::lh_gl_entity_t.
 *
 * Only the name: the draw call of an entity class takes an entity, and the
 * entity holds its class, so the call's signature cannot include `entity.h`.
 * The definition and the methods are in `lh/gl/entity.h`.
 */

#ifndef LH_GL_ENTITY_DECL_H
#define LH_GL_ENTITY_DECL_H

/**
 * @struct lh_gl_entity
 * @brief A rectangle that knows how to draw itself; defined in `lh/gl/entity.h`.
 */
struct lh_gl_entity;
/** @copybrief lh_gl_entity */
typedef struct lh_gl_entity lh_gl_entity_t;

#endif /* LH_GL_ENTITY_DECL_H */