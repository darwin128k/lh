#ifndef LH_REINTERPRET_CAST_H
#define LH_REINTERPRET_CAST_H

#include <lh/compiler_cxx.h>

#ifdef LH_COMPILER_CXX
#    define lh_reinterpret_cast(T, expr) reinterpret_cast<T>(expr)
#else
#    define lh_reinterpret_cast(T, expr) ((T)expr)
#endif // LH_COMPILER_CXX

#endif // LH_REINTERPRET_CAST_H
