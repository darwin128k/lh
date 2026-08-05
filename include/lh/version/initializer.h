/**
 * @file initializer.h
 * @brief Macro for initializing a ::lh_version_t value.
 */

#ifndef LH_VERSION_INITIALIZER_H
#define LH_VERSION_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_version_initializer(major, minor, patch)
 * @brief Produces a brace-enclosed initializer for ::lh_version_t.
 *
 * Expands to a compound initializer that can be used in variable definitions
 * or static storage initialization of a ::lh_version_t struct.
 *
 * @param major Major version number (::lh_version_major_t).
 * @param minor Minor version number (::lh_version_minor_t).
 * @param patch Patch version number (::lh_version_patch_t).
 *
 * Example usage:
 * @code{.c}
 * static lh_version_t ver = lh_version_initializer(1, 0, 0);
 * @endcode
 *
 * @see lh_version_pack
 */
#define lh_version_initializer(major, minor, patch) lh_initializer(major, minor, patch)

#endif /* LH_VERSION_INITIALIZER_H */
