/**
 * @file kind.h
 * @brief What an ::lh_ini_entry_t is: a section header or a key/value pair.
 */

#ifndef LH_INI_ENTRY_KIND_H
#define LH_INI_ENTRY_KIND_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_ini_entry_kind_t
 * @brief Discriminator of ::lh_ini_entry_t. Alias for ::lh_u8_t; values are
 *        the `lh_ini_entry_kind_*` constants below.
 */
typedef lh_u8_t lh_ini_entry_kind_t;

/**
 * @def lh_ini_entry_kind_none
 * @brief No entry. What ::lh_ini_entry_init leaves.
 */
#define lh_ini_entry_kind_none 0U

/**
 * @def lh_ini_entry_kind_section
 * @brief A `[name]` line: the pairs after it belong to `name`.
 */
#define lh_ini_entry_kind_section 1U

/**
 * @def lh_ini_entry_kind_pair
 * @brief A `key = value` line.
 */
#define lh_ini_entry_kind_pair 2U

#endif /* LH_INI_ENTRY_KIND_H */
