/**
 * @file arch.h
 * @brief Target architecture detection (bit width, etc.).
 *
 * Provides ::LH_COMPILER_ARCH — a compile-time constant for the target’s
 * pointer width in bits. Auto-detection yields `32` or `64`. You may define
 * `LH_COMPILER_ARCH` to `8` or `16` before including this header for embedded
 * ports; pair with `lh/addr.h` only if that matches `sizeof(void *)` on the target.
 */

#ifndef LH_COMPILER_ARCH_H
#define LH_COMPILER_ARCH_H

#ifndef LH_COMPILER_ARCH
#    if defined(_WIN64) || defined(__WIN64__) || defined(__x86_64__) || defined(__x86_64) ||       \
        defined(__amd64__) || defined(__amd64) || defined(__aarch64__) || defined(__arm64__)
/**
 * @def LH_COMPILER_ARCH
 * @brief Target pointer width in bits (`64` or `32` when auto-detected).
 *
 * Chosen from predefined compiler macros below; can be overridden by defining
 * `LH_COMPILER_ARCH` before including this header (e.g. `8` / `16` for some
 * embedded toolchains — not set automatically).
 *
 * | Value | Meaning                                            |
 * |-------|----------------------------------------------------|
 * | `64`  | x86-64, AMD64, AArch64, ARM64, Win64               |
 * | `32`  | x86, i386, ARM (32-bit)                            |
 *
 * Example usage:
 * @code{.c}
 * #if LH_COMPILER_ARCH == 64
 *     // 64-bit specific code
 * #endif
 * @endcode
 */
#        define LH_COMPILER_ARCH 64
#    elif defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(i386) ||             \
        defined(__arm__) || defined(__ARM_ARCH)
#        define LH_COMPILER_ARCH 32
#    else
#        error "Unsupported architecture bit width"
#    endif
#endif /* LH_COMPILER_ARCH */

#endif /* LH_COMPILER_ARCH_H */
