/**
 * @file option.h
 * @brief The values of ::LH_LIBRARY_OPTION_ATOMIC: how the atomics are built.
 *
 * ::LH_ATOMIC_COMPILER — on the compiler's atomic instructions (GCC/Clang
 * `__atomic`, MSVC `_Interlocked`): lock-free, the default.
 * ::LH_ATOMIC_CRITICAL — each operation is plain code inside the critical
 * section of the slot (lh/core/runtime/critical.h), for a core without atomic
 * read-modify-write instructions (Cortex-M0, M0+): not lock-free.
 */

#ifndef LH_ATOMIC_OPTION_H
#define LH_ATOMIC_OPTION_H

#include <lh/config.h>

/** @brief ::LH_LIBRARY_OPTION_ATOMIC: the compiler's atomic instructions. */
#define LH_ATOMIC_COMPILER 0
/** @brief ::LH_LIBRARY_OPTION_ATOMIC: plain code in the slot's critical section. */
#define LH_ATOMIC_CRITICAL 1

#endif /* LH_ATOMIC_OPTION_H */
