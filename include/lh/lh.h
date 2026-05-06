/**
 * @file lh.h
 * @brief Top-level public API of the lh library.
 *
 * Include this header to access the library's runtime interface.
 * Currently, exposes ::lh_get_version for querying the compiled version.
 */

#ifndef LH_H
#define LH_H

#include <lh/version.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return a pointer to the compiled library version.
 *        The returned pointer refers to a static object — do not free it.
 *
 * @return Pointer to the ::lh_version_t describing this build.
 *
 * Example usage:
 * @code{.c}
 * const lh_version_t *v = lh_get_version();
 * lh_version_major_t major = lh_version_get_major(v);
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
const lh_version_t *
lh_get_version(void);

LH_COMPILER_EXTERN_C_END

#endif // LH_H
