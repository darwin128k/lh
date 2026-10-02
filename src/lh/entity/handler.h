/**
 * @file handler.h
 * @brief Library-private: a handler added to an entity, kept as a tree block
 *        owned by that entity (so it is freed with it).
 */

#ifndef LH_SRC_ENTITY_HANDLER_H
#define LH_SRC_ENTITY_HANDLER_H

#include <lh/entity/fn.h>
#include <lh/entity/handler/fields.h>
#include <lh/list.h>

/**
 * @struct lh_entity_handler
 * @brief Fields via ::lh_entity_handler_fields.
 */
struct lh_entity_handler
{
    lh_entity_handler_fields(lh_list_node_t, lh_entity_handler_fn, lh_ptr);
};
typedef struct lh_entity_handler lh_entity_handler_t;

#endif /* LH_SRC_ENTITY_HANDLER_H */
