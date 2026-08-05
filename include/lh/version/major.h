/**
 * @file major.h
 * @brief Type definition for the major version component.
 */

#ifndef LH_VERSION_MAJOR_H
#define LH_VERSION_MAJOR_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_version_major_t
 * @brief Unsigned integer type for the major version number.
 *
 * Represents the major component of a version (e.g., `1` in `1.2.3`).
 * A change in the major version signals a breaking or incompatible update.
 *
 * Range: 0 to 65535.
 *
 * Alias for: ::lh_u16_t
 *
 * @see lh_version_t
 */
typedef lh_u16_t lh_version_major_t;

#endif /* LH_VERSION_MAJOR_H */
