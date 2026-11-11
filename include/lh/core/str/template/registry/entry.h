/**
 * @file entry.h
 * @brief ::lh_str_template_registry_entry_t — one named value of a template
 *        registry, and who added it.
 */

#ifndef LH_STR_TEMPLATE_REGISTRY_ENTRY_H
#define LH_STR_TEMPLATE_REGISTRY_ENTRY_H

#include <lh/core/str.h>

/** @brief One named value and its owner. */
struct lh_str_template_registry_entry
{
    lh_ptr owner;   /**< who added it; only they remove it by name */
    lh_str_t name;  /**< the token name, owned */
    lh_str_t value; /**< the text the token stands for, owned */
};
/** @copybrief lh_str_template_registry_entry */
typedef struct lh_str_template_registry_entry lh_str_template_registry_entry_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Who added the entry (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_str_template_registry_entry_get_owner(const lh_str_template_registry_entry_t *self);

/**
 * @brief Store who added the entry (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The entry; checked by ::lh_assert_runtime_ref.
 * @param owner The new value.
 */
lh_void
lh_str_template_registry_entry_set_owner(lh_str_template_registry_entry_t *self, lh_ptr owner);

/**
 * @brief The token name (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_str_t *
lh_str_template_registry_entry_get_name(const lh_str_template_registry_entry_t *self);

/**
 * @brief The token name, to change in place (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_str_t *
lh_str_template_registry_entry_get_name_mut(lh_str_template_registry_entry_t *self);

/**
 * @brief The text the token stands for (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_str_t *
lh_str_template_registry_entry_get_value(const lh_str_template_registry_entry_t *self);

/**
 * @brief The text the token stands for, to change in place (level 1).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_str_t *
lh_str_template_registry_entry_get_value_mut(lh_str_template_registry_entry_t *self);

/**
 * @brief Make @p self the entry @p name = @p value of @p owner, copying both texts
 *        (level 2).
 *
 * @param self  The entry; checked by ::lh_assert_runtime_ref.
 * @param owner Who adds it.
 * @param name  The token name.
 * @param value The text it stands for.
 */
lh_void
lh_str_template_registry_entry_init(lh_str_template_registry_entry_t *self, lh_ptr owner,
                                    lh_str_view_t name, lh_str_view_t value);

/**
 * @brief Release the texts of @p self (level 2).
 *
 * @param self The entry; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_str_template_registry_entry_deinit(lh_str_template_registry_entry_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_TEMPLATE_REGISTRY_ENTRY_H */
