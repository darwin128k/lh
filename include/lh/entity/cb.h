/**
 * @file cb.h
 * @brief Function-pointer aliases for the ::lh_entity_t callbacks.
 */

#ifndef LH_ENTITY_CB_H
#define LH_ENTITY_CB_H

#include <lh/entity/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_entity_handler_cb
 * @brief Pointer to ::lh_entity_handler_fn.
 */
#define lh_entity_handler_cb lh_ptr_of(lh_entity_handler_fn)

#endif /* LH_ENTITY_CB_H */
