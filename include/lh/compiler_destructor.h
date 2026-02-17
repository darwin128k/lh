#ifndef LH_COMPILER_DESTRUCTOR_H
#define LH_COMPILER_DESTRUCTOR_H

#include <lh/compiler_type.h>

#if (LH_COMPILER_TYPE_GCC_LIKE)
#    define lh_compiler_destructor(f) __attribute__((destructor)) static void f(void)

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    include <lh/compiler_bit.h>
#    if LH_COMPILER_BIT == 64
#        define lh_compiler_destructor(f)                                                            \
            __pragma(section(".CRT$XTU", read)) static void __cdecl f(void);                         \
            __declspec(allocate(".CRT$XTU")) void(__cdecl * f##_)(void) = f;                         \
            __pragma(comment(linker, "/include:" #f "_"));                                           \
            static void __cdecl f(void)
#    elif LH_COMPILER_BIT == 32
#        define lh_compiler_destructor(f)                                                            \
            __pragma(section(".CRT$XTU", read)) static void __cdecl f(void);                         \
            __declspec(allocate(".CRT$XTU")) void(__cdecl * f##_)(void) = f;                         \
            __pragma(comment(linker, "/include:_" #f "_"));                                          \
            static void __cdecl f(void)
#    endif
#endif

#endif // LH_COMPILER_DESTRUCTOR_H
