#ifndef LH_COMPILER_ATTRIBUTE_SYMBOL_H
#define LH_COMPILER_ATTRIBUTE_SYMBOL_H

#include <lh/compiler_type.h>

#if (LH_COMPILER_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT                                \
        __attribute__((visibility("default")))

#    define LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT                                \
        __attribute__((visibility("default")))

#    define LH_COMPILER_ATTRIBUTE_SYMBOL_HIDDEN                                \
        __attribute__((visibility("hidden")))

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_SYMBOL_EXPORT __declspec(dllexport)

#    define LH_COMPILER_ATTRIBUTE_SYMBOL_IMPORT __declspec(dllimport)

#    define LH_COMPILER_ATTRIBUTE_SYMBOL_HIDDEN

#endif

#endif // LH_COMPILER_ATTRIBUTE_SYMBOL_H