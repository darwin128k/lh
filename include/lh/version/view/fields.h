/**
 * @file fields.h
 * @brief Macro for declaring versioned major/minor/patch member fields.
 */

#ifndef LH_VERSION_FIELDS_H
#define LH_VERSION_FIELDS_H

/**
 * @def lh_version_fields(major_type, minor_type, patch_type)
 * @brief Expands to three member variable declarations for a version struct.
 *
 * Intended to be embedded inside a struct or union body to inject typed
 * `major`, `minor`, and `patch` fields in a single, consistent declaration.
 *
 * @param major_type Type of the `major` field.
 * @param minor_type Type of the `minor` field.
 * @param patch_type Type of the `patch` field.
 *
 * Example usage:
 * @code{.c}
 * struct lh_version {
 *     lh_version_fields(lh_u16_t, lh_u8_t, lh_u8_t);
 * };
 * @endcode
 */
#define lh_version_fields(major_type, minor_type, patch_type)                                      \
    major_type major;                                                                              \
    minor_type minor;                                                                              \
    patch_type patch

#endif /* LH_VERSION_FIELDS_H */
