/**
 * @file destructor.h
 * @brief Compiler-portable destructor function macro.
 *
 * Provides ::lh_compiler_destructor — a macro
 * that declares a function to be called automatically after `main()`
 * returns (or after `exit()`), using the appropriate mechanism for each compiler.
 */

#ifndef LH_COMPILER_DESTRUCTOR_H
#define LH_COMPILER_DESTRUCTOR_H

#include <lh/compiler/arch.h>
#include <lh/compiler/type.h>

/**
 * @def lh_compiler_destructor(f)
 * @brief Declare a function that runs automatically after `main()` returns.
 *
 * The macro both declares and begins the definition of the function @p f.
 * The function body must follow immediately in `{ }`.
 *
 * On MSVC the implementation differs between 32-bit and 64-bit targets
 * due to linker symbol decoration (`_` prefix on 32-bit):
 *
 * | Compiler       | Mechanism                              |
 * |----------------|----------------------------------------|
 * | GCC / Clang    | `__attribute__((destructor))`          |
 * | MSVC 64-bit    | `.CRT$XTU` section + `/include:f_`     |
 * | MSVC 32-bit    | `.CRT$XTU` section + `/include:_f_`    |
 *
 * @param f Name of the destructor function (unquoted identifier).
 *
 * Example usage:
 * @code{.c}
 * lh_compiler_destructor(on_unload) {
 *     // runs after main() returns
 * }
 * @endcode
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    define lh_compiler_destructor(f) __attribute__((destructor)) static void f(void)

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    if LH_COMPILER_ARCH == 64
#        define lh_compiler_destructor(f)                                                          \
            __pragma(section(".CRT$XTU", read)) static void __cdecl f(void);                       \
            __declspec(allocate(".CRT$XTU")) void(__cdecl * f##_)(void) = f;                       \
            __pragma(comment(linker, "/include:" #f "_"));                                         \
            static void __cdecl f(void)
#    elif LH_COMPILER_ARCH == 32
#        define lh_compiler_destructor(f)                                                          \
            __pragma(section(".CRT$XTU", read)) static void __cdecl f(void);                       \
            __declspec(allocate(".CRT$XTU")) void(__cdecl * f##_)(void) = f;                       \
            __pragma(comment(linker, "/include:_" #f "_"));                                        \
            static void __cdecl f(void)
#    endif

#else
#    error "Destructor is not supported on this compiler"
#endif

#endif // LH_COMPILER_DESTRUCTOR_H