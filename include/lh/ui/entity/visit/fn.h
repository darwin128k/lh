/**
 * @file fn.h
 * @brief What ::lh_ui_entity_walk calls for each entity.
 *
 * Not a pointer type by itself. ::lh_ui_entity_visit_cb is the pointer.
 */

#ifndef LH_UI_ENTITY_VISIT_FN_H
#define LH_UI_ENTITY_VISIT_FN_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>

struct lh_ui_entity;

/**
 * @typedef lh_ui_entity_visit_fn
 * @brief Function type called with one entity and the walk context.
 *
 * Return ::lh_bool_true to go on, ::lh_bool_false to stop the whole walk.
 */
LH_COMPILER_EXTERN_C_BEGIN
typedef lh_bool_t(lh_ui_entity_visit_fn)(const struct lh_ui_entity *entity, lh_ptr context);
LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_VISIT_FN_H */
