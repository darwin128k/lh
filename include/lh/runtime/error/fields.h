/**
 * @file fields.h
 * @brief Macro for declaring runtime error code and description fields.
 */

#ifndef LH_RUNTIME_ERROR_FIELDS_H
#define LH_RUNTIME_ERROR_FIELDS_H

#include <lh/error/fields.h>

/**
 * @def lh_runtime_error_fields(code_type, desc_type)
 * @brief Expands to the @c code and @c desc members of a runtime error struct.
 *
 * Delegates to ::lh_error_fields so runtime error structs share the same
 * field layout as ::lh_error_t while remaining in the runtime namespace.
 *
 * @param code_type Type of the @c code field (e.g. ::lh_runtime_error_code_t).
 * @param desc_type Type of the @c desc field (e.g. ::lh_runtime_error_desc_t).
 *
 * Example usage:
 * @code{.c}
 * typedef struct lh_runtime_error {
 *     lh_runtime_error_fields(lh_runtime_error_code_t, lh_runtime_error_desc_t);
 * } lh_runtime_error_t;
 * @endcode
 *
 * @see lh_error_fields
 * @see lh_runtime_error_t
 */
#define lh_runtime_error_fields(code_type, desc_type) lh_error_fields(code_type, desc_type)

#endif // LH_RUNTIME_ERROR_FIELDS_H
