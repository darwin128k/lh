/**
 * @file cb.h
 * @brief Pointer to ::lh_ui_entity_visit_fn.
 */

#ifndef LH_UI_ENTITY_VISIT_CB_H
#define LH_UI_ENTITY_VISIT_CB_H

#include <lh/ui/entity/visit/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_ui_entity_visit_cb
 * @brief Pointer to ::lh_ui_entity_visit_fn.
 */
#define lh_ui_entity_visit_cb lh_ptr_of(lh_ui_entity_visit_fn)

#endif /* LH_UI_ENTITY_VISIT_CB_H */
