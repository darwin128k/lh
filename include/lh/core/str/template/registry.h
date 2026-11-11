/**
 * @file registry.h
 * @brief ::lh_str_template_registry_t — named values for templates, each with the
 *        owner that added it.
 *
 * A name is unique in the registry; it is added by one owner and removed by that
 * owner, by name, or with everything else the owner added. The registry copies
 * names and values. ::lh_str_template_registry_lookup lets
 * ::lh_str_template_try_expand read it directly. Not thread-safe.
 */

#ifndef LH_STR_TEMPLATE_REGISTRY_H
#define LH_STR_TEMPLATE_REGISTRY_H

#include <lh/core/str/template/registry/entry.h>

/** @brief Named values and their owners. */
struct lh_str_template_registry
{
    lh_array_t entries; /**< ::lh_str_template_registry_entry_t, in the order added */
};
/** @copybrief lh_str_template_registry */
typedef struct lh_str_template_registry lh_str_template_registry_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The entries, in the order added (level 1).
 *
 * @param self The registry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_array_t *
lh_str_template_registry_get_entries(const lh_str_template_registry_t *self);

/**
 * @brief The entries, to change in place (level 1).
 *
 * @param self The registry; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_array_t *
lh_str_template_registry_get_entries_mut(lh_str_template_registry_t *self);

/**
 * @brief Make @p self an empty registry (level 2).
 *
 * @param self The registry; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_str_template_registry_init(lh_str_template_registry_t *self);

/**
 * @brief The number of entries (level 2).
 *
 * @param self The registry; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_str_template_registry_get_count(const lh_str_template_registry_t *self);

/**
 * @brief The entry at @p index, to change in place (level 2).
 *
 * @param self  The registry; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_str_template_registry_get_count.
 */
lh_str_template_registry_entry_t *
lh_str_template_registry_get_entry_mut(lh_str_template_registry_t *self, lh_usize_t index);

/**
 * @brief The entry at @p index (level 2).
 *
 * @param self  The registry; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_str_template_registry_get_count.
 */
const lh_str_template_registry_entry_t *
lh_str_template_registry_get_entry(const lh_str_template_registry_t *self, lh_usize_t index);

/**
 * @brief Release the entry at @p index and take it out (level 3).
 *
 * @param self  The registry; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_str_template_registry_get_count.
 */
lh_void
lh_str_template_registry_erase(lh_str_template_registry_t *self, lh_usize_t index);

/**
 * @brief The index of the entry named @p name, or ::LH_STR_VIEW_INVALID (level 3).
 *
 * Names compare byte for byte.
 *
 * @param self The registry; checked by ::lh_assert_runtime_ref.
 * @param name The name.
 */
lh_usize_t
lh_str_template_registry_find(const lh_str_template_registry_t *self, const lh_str_view_t *name);

/**
 * @brief Release every entry and the registry's storage (level 4).
 *
 * @param self The registry; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_str_template_registry_deinit(lh_str_template_registry_t *self);

/**
 * @brief Write the value of @p name into @p value (level 4).
 *
 * @param self  The registry; checked by ::lh_assert_runtime_ref.
 * @param name  The name.
 * @param value Receives a view of the value, valid until the entry is removed.
 * @return ::lh_bool_false when no entry has that name; @p value is untouched then.
 */
lh_bool_t
lh_str_template_registry_try_get_value(const lh_str_template_registry_t *self,
                                       const lh_str_view_t *name, lh_str_view_t *value);

/**
 * @brief Add @p name = @p value for @p owner (level 4).
 *
 * @param self  The registry; checked by ::lh_assert_runtime_ref.
 * @param owner Who adds it; the only one who removes it by name.
 * @param name  The name; ::lh_str_template_is_name must accept it.
 * @param value The text it stands for.
 * @return ::lh_bool_false when the name is not a token name or is taken.
 */
lh_bool_t
lh_str_template_registry_try_add(lh_str_template_registry_t *self, lh_ptr owner,
                                 const lh_str_view_t *name, const lh_str_view_t *value);

/**
 * @brief Remove the entry @p owner added as @p name (level 4).
 *
 * @param self  The registry; checked by ::lh_assert_runtime_ref.
 * @param owner Who added it.
 * @param name  The name.
 * @return ::lh_bool_false when @p owner has no entry of that name.
 */
lh_bool_t
lh_str_template_registry_try_remove(lh_str_template_registry_t *self, lh_ptr owner,
                                    const lh_str_view_t *name);

/**
 * @brief Remove every entry @p owner added (level 4).
 *
 * @param self  The registry; checked by ::lh_assert_runtime_ref.
 * @param owner Whose entries go.
 */
lh_void
lh_str_template_registry_remove_owner(lh_str_template_registry_t *self, lh_ptr owner);

/**
 * @brief An ::lh_str_template_lookup_fn over a registry (level 5).
 *
 * @param user  The ::lh_str_template_registry_t to read.
 * @param name  The name.
 * @param value Receives its value.
 */
lh_bool_t
lh_str_template_registry_lookup(lh_ptr user, const lh_str_view_t *name, lh_str_view_t *value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_TEMPLATE_REGISTRY_H */
