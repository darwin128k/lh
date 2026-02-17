#ifndef LH_STATIC_CAST_H
#define LH_STATIC_CAST_H

#include <lh/compiler_cxx.h>

#ifdef LH_COMPILER_CXX
#    define lh_static_cast(T, expr) static_cast<T>(expr)
#else
#    define lh_static_cast(T, expr) ((T)expr)
#endif // LH_COMPILER_CXX

#endif // LH_STATIC_CAST_H
