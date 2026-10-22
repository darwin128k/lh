/**
 * @file kind.h
 * @brief What an ::lh_json_node_t holds: one of the six JSON values.
 */

#ifndef LH_JSON_KIND_H
#define LH_JSON_KIND_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_json_kind_t
 * @brief Discriminator of ::lh_json_node_t. Alias for ::lh_u8_t; values are
 *        the `lh_json_kind_*` constants below.
 */
typedef lh_u8_t lh_json_kind_t;

/**
 * @def lh_json_kind_none
 * @brief No value yet. What ::lh_json_node_init leaves.
 */
#define lh_json_kind_none 0U

/**
 * @def lh_json_kind_null
 * @brief `null`.
 */
#define lh_json_kind_null 1U

/**
 * @def lh_json_kind_bool
 * @brief `true` or `false`, in the node's flag.
 */
#define lh_json_kind_bool 2U

/**
 * @def lh_json_kind_number
 * @brief A number, kept as the text it was written as (`-12`, `3.5e2`).
 */
#define lh_json_kind_number 3U

/**
 * @def lh_json_kind_string
 * @brief A string, unescaped, in the node's text.
 */
#define lh_json_kind_string 4U

/**
 * @def lh_json_kind_array
 * @brief `[...]`: the children in order, without keys.
 */
#define lh_json_kind_array 5U

/**
 * @def lh_json_kind_object
 * @brief `{...}`: the children in order, each with its key.
 */
#define lh_json_kind_object 6U

#endif /* LH_JSON_KIND_H */
