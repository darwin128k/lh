/**
 * @file patch.h
 * @brief Type definition for the patch version component.
 */

#ifndef LH_VERSION_PATCH_H
#define LH_VERSION_PATCH_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_version_patch_t
 * @brief Unsigned integer type for the patch version number.
 *
 * Represents the patch component of a version (e.g., `3` in `1.2.3`).
 * A change in the patch version typically signals backwards-compatible
 * bug fixes with no new functionality.
 *
 * Range: 0 to 255.
 *
 * Alias for: ::lh_u8_t
 *
 * @see lh_version_t
 */
typedef lh_u8_t lh_version_patch_t;

#endif // LH_VERSION_PATCH_H
