/**
 * @file class.h
 * @brief What kind of entity an ::lh_entity_t is: its size and behavior.
 *
 * A class is constant data, usually one `static const` per kind of entity.
 * A derived class names its base, and its instance struct starts with the
 * base's fields, so a pointer to it is also a pointer to the base:
 * @code{.c}
 * struct my_button
 * {
 *     lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
 *     lh_int_t clicks;
 * };
 *
 * static const lh_entity_class_t my_button_class = lh_entity_class_initializer(
 *     &lh_entity_base_class, sizeof(struct my_button), lh_null, lh_null, my_button_event);
 * @endcode
 */

#ifndef LH_ENTITY_CLASS_H
#define LH_ENTITY_CLASS_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/class/fields.h>
#include <lh/entity/fn.h>
#include <lh/size.h>

/**
 * @struct lh_entity_class
 * @brief Fields via ::lh_entity_class_fields.
 */
struct lh_entity_class
{
    lh_entity_class_fields(struct lh_entity_class, lh_usize_t, lh_entity_constructor_fn,
                           lh_entity_destructor_fn, lh_entity_event_fn);
};
typedef struct lh_entity_class lh_entity_class_t;

/**
 * @def lh_entity_class_initializer(base, size, constructor, destructor, event)
 * @brief Brace initializer for a `static const` ::lh_entity_class_t.
 */
#define lh_entity_class_initializer(base, size, constructor, destructor, event)                    \
    {(base), (size), (constructor), (destructor), (event)}

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class every other class derives from: a plain entity with no
 *        behavior of its own. Used directly, it groups others (a container).
 */
extern const lh_entity_class_t lh_entity_base_class;

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_CLASS_H */
