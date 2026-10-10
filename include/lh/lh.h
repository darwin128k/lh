/**
 * @file lh.h
 * @brief The library's own interface: which lh this binary is.
 *
 * Not an umbrella, and deliberately not one. Nothing here pulls in a subsystem.
 * Each of those has its own door — `lh/math.h`, `lh/memory.h`, `lh/os.h`,
 * `lh/ui.h`, `lh/str.h` — so a program includes what it uses and nothing else.
 *
 * `lh/version.h` is a different thing and does not answer this question. That is
 * the version *value type*: assign, set components, compare, format, parse over a
 * version the caller built. This header answers the one question only the
 * compiled library can answer, which version of lh this binary is.
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
const lh_version_t *
lh_get_version(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_H */
