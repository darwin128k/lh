/**
 * @file list.h
 * @brief Growable list of owning strings (::lh_str_list_t).
 *
 * Backed by ::lh_vector_t with element type ::lh_str_t — but not usable as
 * a plain ::lh_vector_t: ::lh_vector_t moves element bytes without knowing
 * an element can own a heap buffer, so every operation here that removes
 * or overwrites an element calls ::lh_str_deinit / ::lh_str_assign on it by
 * hand instead of the raw ::lh_vector_clear / ::lh_vector_assign.
 */

#ifndef LH_STR_LIST_H
#define LH_STR_LIST_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/str.h>
#include <lh/vector.h>

/**
 * @struct lh_str_list
 * @brief Growable list of ::lh_str_t elements.
 */
typedef struct lh_str_list
{
    lh_vector_t items; /**< Element type: ::lh_str_t. */
} lh_str_list_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Initialize @p self as an empty list.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_str_list_init(lh_str_list_t *self);

/**
 * @brief Deinit every stored string, then release the list's own storage.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_str_list_deinit(lh_str_list_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_LIST_H */
