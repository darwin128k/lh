/**
 * @file cb.h
 * @brief Pointer to ::lh_entity_face_fn.
 */

#ifndef LH_ENTITY_FACE_CB_H
#define LH_ENTITY_FACE_CB_H

#include <lh/entity/face/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_entity_face_cb
 * @brief Pointer to ::lh_entity_face_fn.
 */
#define lh_entity_face_cb lh_ptr_of(lh_entity_face_fn)

#endif /* LH_ENTITY_FACE_CB_H */
