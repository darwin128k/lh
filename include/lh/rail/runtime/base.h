/**
 * @file base.h
 * @brief The rail base slot: the one ::lh_rail_base_t every rail mechanism calls.
 *
 * The slot starts empty — no clock, no lock, no signal, no thread — like the
 * core's runtime slots: rail brings no platform of its own. The host copies
 * its platform's base in (::lh_rail_base_assign with, say, the OS base of
 * lh/os/rail/base.h) before it starts any rail thread or loop, so every thread
 * sees the same calls. Until then the `try_` forms that need a missing group
 * refuse.
 */

#ifndef LH_RAIL_RUNTIME_BASE_H
#define LH_RAIL_RUNTIME_BASE_H

#include <lh/rail/base.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The rail base slot, for filling.
 *
 * Fill it once at start, before any rail thread or loop runs; it is not guarded
 * against a change while they run.
 *
 * @return The slot; never ::lh_null.
 */
lh_rail_base_t *
lh_rail_runtime_base_get_mut(void);

/**
 * @brief The rail base slot.
 * @return The slot; never ::lh_null.
 */
const lh_rail_base_t *
lh_rail_runtime_base_get(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_RUNTIME_BASE_H */
