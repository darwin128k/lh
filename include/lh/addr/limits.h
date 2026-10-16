/**
 * @file limits.h
 * @brief Min/max/size macros for ::lh_uaddr_t / ::lh_saddr_t.
 *
 * Per-width aliases of the matching fixed-width limits from
 * <lh/numeric/fixed/limits.h>, selected by ::LH_COMPILER_ARCH. Kept out of
 * <lh/addr.h> so the typedef header stays light.
 */

#ifndef LH_ADDR_LIMITS_H
#define LH_ADDR_LIMITS_H

#include <lh/compiler/arch.h>
#include <lh/numeric/fixed/limits.h>

#if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64

#    ifndef LH_UADDR_T_MIN
/**
 * @def LH_UADDR_T_MIN
 * @brief Minimum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_UADDR_T_MIN LH_U64_T_MIN
#    endif /* LH_UADDR_T_MIN */

#    ifndef LH_UADDR_T_MAX
/**
 * @def LH_UADDR_T_MAX
 * @brief Maximum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_UADDR_T_MAX LH_U64_T_MAX
#    endif /* LH_UADDR_T_MAX */

#    ifndef LH_UADDR_T_SIZE
/**
 * @def LH_UADDR_T_SIZE
 * @brief Size of ::lh_uaddr_t in bytes when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_UADDR_T_SIZE LH_U64_T_SIZE
#    endif /* LH_UADDR_T_SIZE */

#    ifndef LH_SADDR_T_MIN
/**
 * @def LH_SADDR_T_MIN
 * @brief Minimum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_SADDR_T_MIN LH_S64_T_MIN
#    endif /* LH_SADDR_T_MIN */

#    ifndef LH_SADDR_T_MAX
/**
 * @def LH_SADDR_T_MAX
 * @brief Maximum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_SADDR_T_MAX LH_S64_T_MAX
#    endif /* LH_SADDR_T_MAX */

#    ifndef LH_SADDR_T_SIZE
/**
 * @def LH_SADDR_T_SIZE
 * @brief Size of ::lh_saddr_t in bytes when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_SADDR_T_SIZE LH_S64_T_SIZE
#    endif /* LH_SADDR_T_SIZE */

#elif LH_COMPILER_ARCH == LH_COMPILER_ARCH_32

#    ifndef LH_UADDR_T_MIN
/**
 * @def LH_UADDR_T_MIN
 * @brief Minimum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_UADDR_T_MIN LH_U32_T_MIN
#    endif /* LH_UADDR_T_MIN */

#    ifndef LH_UADDR_T_MAX
/**
 * @def LH_UADDR_T_MAX
 * @brief Maximum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_UADDR_T_MAX LH_U32_T_MAX
#    endif /* LH_UADDR_T_MAX */

#    ifndef LH_UADDR_T_SIZE
/**
 * @def LH_UADDR_T_SIZE
 * @brief Size of ::lh_uaddr_t in bytes when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_UADDR_T_SIZE LH_U32_T_SIZE
#    endif /* LH_UADDR_T_SIZE */

#    ifndef LH_SADDR_T_MIN
/**
 * @def LH_SADDR_T_MIN
 * @brief Minimum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_SADDR_T_MIN LH_S32_T_MIN
#    endif /* LH_SADDR_T_MIN */

#    ifndef LH_SADDR_T_MAX
/**
 * @def LH_SADDR_T_MAX
 * @brief Maximum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_SADDR_T_MAX LH_S32_T_MAX
#    endif /* LH_SADDR_T_MAX */

#    ifndef LH_SADDR_T_SIZE
/**
 * @def LH_SADDR_T_SIZE
 * @brief Size of ::lh_saddr_t in bytes when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_SADDR_T_SIZE LH_S32_T_SIZE
#    endif /* LH_SADDR_T_SIZE */

#elif LH_COMPILER_ARCH == LH_COMPILER_ARCH_16

#    ifndef LH_UADDR_T_MIN
/**
 * @def LH_UADDR_T_MIN
 * @brief Minimum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_UADDR_T_MIN LH_U16_T_MIN
#    endif /* LH_UADDR_T_MIN */

#    ifndef LH_UADDR_T_MAX
/**
 * @def LH_UADDR_T_MAX
 * @brief Maximum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_UADDR_T_MAX LH_U16_T_MAX
#    endif /* LH_UADDR_T_MAX */

#    ifndef LH_UADDR_T_SIZE
/**
 * @def LH_UADDR_T_SIZE
 * @brief Size of ::lh_uaddr_t in bytes when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_UADDR_T_SIZE LH_U16_T_SIZE
#    endif /* LH_UADDR_T_SIZE */

#    ifndef LH_SADDR_T_MIN
/**
 * @def LH_SADDR_T_MIN
 * @brief Minimum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_SADDR_T_MIN LH_S16_T_MIN
#    endif /* LH_SADDR_T_MIN */

#    ifndef LH_SADDR_T_MAX
/**
 * @def LH_SADDR_T_MAX
 * @brief Maximum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_SADDR_T_MAX LH_S16_T_MAX
#    endif /* LH_SADDR_T_MAX */

#    ifndef LH_SADDR_T_SIZE
/**
 * @def LH_SADDR_T_SIZE
 * @brief Size of ::lh_saddr_t in bytes when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_SADDR_T_SIZE LH_S16_T_SIZE
#    endif /* LH_SADDR_T_SIZE */

#elif LH_COMPILER_ARCH == LH_COMPILER_ARCH_8

#    ifndef LH_UADDR_T_MIN
/**
 * @def LH_UADDR_T_MIN
 * @brief Minimum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_UADDR_T_MIN LH_U8_T_MIN
#    endif /* LH_UADDR_T_MIN */

#    ifndef LH_UADDR_T_MAX
/**
 * @def LH_UADDR_T_MAX
 * @brief Maximum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_UADDR_T_MAX LH_U8_T_MAX
#    endif /* LH_UADDR_T_MAX */

#    ifndef LH_UADDR_T_SIZE
/**
 * @def LH_UADDR_T_SIZE
 * @brief Size of ::lh_uaddr_t in bytes when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_UADDR_T_SIZE LH_U8_T_SIZE
#    endif /* LH_UADDR_T_SIZE */

#    ifndef LH_SADDR_T_MIN
/**
 * @def LH_SADDR_T_MIN
 * @brief Minimum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_SADDR_T_MIN LH_S8_T_MIN
#    endif /* LH_SADDR_T_MIN */

#    ifndef LH_SADDR_T_MAX
/**
 * @def LH_SADDR_T_MAX
 * @brief Maximum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_SADDR_T_MAX LH_S8_T_MAX
#    endif /* LH_SADDR_T_MAX */

#    ifndef LH_SADDR_T_SIZE
/**
 * @def LH_SADDR_T_SIZE
 * @brief Size of ::lh_saddr_t in bytes when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_SADDR_T_SIZE LH_S8_T_SIZE
#    endif /* LH_SADDR_T_SIZE */

#else

#    error "LH_COMPILER_ARCH must be 8, 16, 32, or 64 for lh/addr/limits.h"

#endif

#endif /* LH_ADDR_LIMITS_H */
