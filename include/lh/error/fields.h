/**
 * @file fields.h
 * @brief Macro for declaring error code and description fields.
 */

#ifndef LH_ERROR_FIELDS_H
#define LH_ERROR_FIELDS_H

/**
 * @def lh_error_fields(code_type, desc_type)
 * @brief Expands to the `code` and `desc` members of an error struct.
 *
 * Intended to be embedded inside a struct body so ::lh_error_t
 * and compatible wrappers share one declaration shape.
 *
 * @param code_type Type of the @c code field.
 * @param desc_type Type of the @c desc field.
 *
 * Example usage:
 * @code{.c}
 * typedef struct lh_error {
 *     lh_error_fields(lh_error_code_t, lh_error_desc_t);
 * } lh_error_t;
 * @endcode
 *
 * @see lh_error_t
 */
#define lh_error_fields(code_type, desc_type)                                                      \
    code_type code;                                                                                \
    desc_type desc

#endif // LH_ERROR_FIELDS_H
