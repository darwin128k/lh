#ifndef LH_UTIL_H
#define LH_UTIL_H

#include <lh/platform_limits.h>
#include <lh/reinterpret_cast.h>
#include <lh/static_cast.h>

#define lh_size_of(expr) sizeof(expr)
#define lh_bits_of(expr) (lh_size_of(expr) * LH_CHAR_BIT)
#define lh_cast(T, expr) lh_static_cast(T, expr)
#define lh_rcast(T, expr) lh_reinterpret_cast(T, expr)

#endif // LH_UTIL_H
