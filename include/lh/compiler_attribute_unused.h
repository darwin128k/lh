#ifndef LH_COMPILER_ATTRIBUTE_UNUSED_H
#define LH_COMPILER_ATTRIBUTE_UNUSED_H

#include <lh/compiler_type.h>
#include <lh/void.h>

#if (LH_COMPILER_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_UNUSED __attribute__((unused))

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_UNUSED (lh_void)
#endif

#endif // LH_COMPILER_ATTRIBUTE_UNUSED_H
