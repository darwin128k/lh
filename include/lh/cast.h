/**
 * @file cast.h
 * @brief Umbrella header for all `lh` cast macros.
 *
 * Including this file pulls in ::lh_cast_static, ::lh_cast_reinterpret,
 * and ::lh_cast_const.
 * Prefer individual headers when only one cast variant is needed.
 */

#ifndef LH_CAST_H
#define LH_CAST_H

#include <lh/cast/const.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>

#endif // LH_CAST_H
