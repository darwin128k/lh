/**
 * @file fn.h
 * @brief ::lh_rail_base_sleep_fn — pause the calling thread.
 */

#ifndef LH_RAIL_BASE_SLEEP_FN_H
#define LH_RAIL_BASE_SLEEP_FN_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Pause the calling thread for at least @p milliseconds.
 *
 * @param milliseconds The shortest pause.
 * @return ::lh_bool_true once the time passed; ::lh_bool_false on a failure.
 */
typedef lh_bool_t(lh_rail_base_sleep_fn)(lh_u32_t milliseconds);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_BASE_SLEEP_FN_H */
