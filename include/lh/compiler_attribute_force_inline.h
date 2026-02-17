#ifndef LH_COMPILER_ATTRIBUTE_FORCE_INLINE_H
#define LH_COMPILER_ATTRIBUTE_FORCE_INLINE_H

#include <lh/compiler_attribute_inline.h>
#include <lh/compiler_type.h>

#if (LH_COMPILER_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_FORCE_INLINE                                 \
        __attribute__((always_inline)) LH_COMPILER_ATTRIBUTE_INLINE

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_FORCE_INLINE __forceinline
#endif

#endif // LH_COMPILER_ATTRIBUTE_FORCE_INLINE_H
