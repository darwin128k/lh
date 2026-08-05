/**
 * @file lh/version/fallback.h
 * @brief Default ::LH_VERSION_MAJOR / ::LH_VERSION_MINOR / ::LH_VERSION_PATCH.
 *
 * Macros from `lh/config.h` apply first
 * (tracked file; CMake may prepend a generated copy on the include path).
 * Any component still undefined here falls back to `0`.
 *
 * Same idea as `lh/library/fallback.h`: explicit settings win,
 * otherwise a safe default applies.
 */

#ifndef LH_VERSION_BUILD_FALLBACK_H
#define LH_VERSION_BUILD_FALLBACK_H

#include <lh/config.h>

#ifndef LH_VERSION_MAJOR
/** @brief Default major when not set by the toolchain. */
#    define LH_VERSION_MAJOR 0
#endif

#ifndef LH_VERSION_MINOR
/** @brief Default minor when not set by the toolchain. */
#    define LH_VERSION_MINOR 0
#endif

#ifndef LH_VERSION_PATCH
/** @brief Default patch when not set by the toolchain. */
#    define LH_VERSION_PATCH 0
#endif

#endif /* LH_VERSION_BUILD_FALLBACK_H */
