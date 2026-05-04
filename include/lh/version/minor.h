/**
 * @file minor.h
 * @brief Type definition for the minor version component.
 */

#ifndef LH_VERSION_MINOR_H
#define LH_VERSION_MINOR_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_version_minor_t
 * @brief Unsigned integer type for the minor version number.
 *
 * Represents the minor component of a version (e.g., `2` in `1.2.3`).
 * A change in the minor version typically signals new, backwards-compatible
 * functionality.
 *
 * Range: 0 to 255.
 *
 * Alias for: ::lh_u8_t
 *
 * @see lh_version_t
 */
typedef lh_u8_t lh_version_minor_t;

#endif // LH_VERSION_MINOR_H
