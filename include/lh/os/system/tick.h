/** @file tick.h @brief Native monotonic clock and finite thread sleep. */
#ifndef LH_OS_SYSTEM_TICK_H
#define LH_OS_SYSTEM_TICK_H
#include <lh/config.h>
#include <lh/bool.h>
#include <lh/numeric/fixed/types.h>
#include <lh/compiler/extern/c.h>
#if !LH_LIBRARY_OPTION_OS
#    error "OS clocks require LH_LIBRARY_OPTION_OS"
#endif
LH_COMPILER_EXTERN_C_BEGIN
/** Preserve out on failure; native error is captured. */
lh_bool_t
lh_os_system_tick_us(lh_u64_t *out);
lh_bool_t
lh_os_system_sleep_ms(lh_u32_t milliseconds);
LH_COMPILER_EXTERN_C_END
#endif
