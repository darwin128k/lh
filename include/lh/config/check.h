/**
 * @file check.h
 * @brief Compile-time validation of LH configuration values.
 *
 * Included by <lh/config.h> after its option definitions. Invalid values are
 * rejected here for both generated and manually maintained configurations.
 */

#ifndef LH_CONFIG_CHECK_H
#define LH_CONFIG_CHECK_H

#include <lh/core/compiler/arch.h>
#include <lh/core/compiler/os.h>

#if LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX < 1
#    error LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX must be positive
#endif

#if LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD > 512
#    error "LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD must be <= 512"
#endif

#if LH_LIBRARY_OPTION_OS && LH_LIBRARY_OPTION_OS_WERROR && LH_COMPILER_OS != LH_COMPILER_OS_WINDOWS
#    error "lh/os/system/posix/error/capture.c: LH_LIBRARY_OPTION_OS_WERROR is Windows-only for now"
#endif

#if LH_COMPILER_ARCH != LH_COMPILER_ARCH_8 && LH_COMPILER_ARCH != LH_COMPILER_ARCH_16 &&           \
    LH_COMPILER_ARCH != LH_COMPILER_ARCH_32 && LH_COMPILER_ARCH != LH_COMPILER_ARCH_64
#    error "LH_COMPILER_ARCH must be 8, 16, 32, or 64"
#endif

#endif /* LH_CONFIG_CHECK_H */
