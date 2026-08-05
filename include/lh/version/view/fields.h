/**
 * @file fields.h
 * @brief Macro for declaring versioned major/minor/patch member fields.
 */

#ifndef LH_VERSION_FIELDS_H
#define LH_VERSION_FIELDS_H

/**
 * @def lh_version_fields(Major, Minor, Patch)
 * @brief Expands to three member variable declarations for a version struct.
 *
 * Intended to be embedded inside a struct or union body to inject typed
 * `major`, `minor`, and `patch` fields in a single, consistent declaration.
 *
 * @param Major Type of the major version field.
 * @param Minor Type of the minor version field.
 * @param Patch Type of the patch version field.
 *
 * Example usage:
 * @code{.c}
 * struct Version {
 *     lh_version_fields(uint8_t, uint8_t, uint16_t);
 * };
 *
 * Version v;
 * v.major = 1;
 * v.minor = 2;
 * v.patch = 3;
 * @endcode
 */
#define lh_version_fields(Major, Minor, Patch)                                                     \
    Major major;                                                                                   \
    Minor minor;                                                                                   \
    Patch patch

#endif /* LH_VERSION_FIELDS_H */