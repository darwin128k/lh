#ifndef LH_COMPILER_TYPE_H
#define LH_COMPILER_TYPE_H

#ifndef LH_COMPILER_TYPE_UNKNOWN
#    define LH_COMPILER_TYPE_UNKNOWN 0
#endif // LH_COMPILER_TYPE_UNKNOWN

#ifndef LH_COMPILER_TYPE_GCC
#    define LH_COMPILER_TYPE_GCC 1
#endif // LH_COMPILER_TYPE_GCC

#ifndef LH_COMPILER_TYPE_CLANG
#    define LH_COMPILER_TYPE_CLANG 2
#endif // LH_COMPILER_TYPE_CLANG

#ifndef LH_COMPILER_TYPE_MSVC
#    define LH_COMPILER_TYPE_MSVC 3
#endif // LH_COMPILER_TYPE_MSVC

#ifndef LH_COMPILER_TYPE
#    if defined(__clang__)
#        define LH_COMPILER_TYPE LH_COMPILER_TYPE_CLANG
#    elif defined(__GNUC__)
#        define LH_COMPILER_TYPE LH_COMPILER_TYPE_GCC
#    elif defined(_MSC_VER)
#        define LH_COMPILER_TYPE LH_COMPILER_TYPE_MSVC
#    else
#        error "Unknown compiler type"
#    endif
#endif // LH_COMPILER_TYPE

#ifndef LH_COMPILER_TYPE_IS_GCC_LIKE
#    define LH_COMPILER_TYPE_IS_GCC_LIKE                                          \
        (LH_COMPILER_TYPE == LH_COMPILER_TYPE_GCC) ||                          \
            (LH_COMPILER_TYPE == LH_COMPILER_TYPE_CLANG)
#endif // LH_COMPILER_TYPE_IS_GCC_LIKE

#endif // LH_COMPILER_TYPE_H