/**
 * @file numeric.h
 * @brief Umbrella header for numeric types, limits, and intervals.
 *
 * Including this file pulls in the entire numeric module:
 * - Integer type definitions with explicit signedness
 * - Compile-time numeric limits and bounds
 * - Fixed-width integer types
 * - Interval types and bounds
 *
 * For specific functionality, include individual headers directly:
 * - ::lh/numeric/types.h — portable integer type aliases
 * - ::lh/numeric/fixed/types.h — fixed-width integer types
 *
 * @see lh::numeric for the C++ wrapper namespace
 */

#ifndef LH_NUMERIC_H
#define LH_NUMERIC_H

#include <lh/numeric/types.h>
#include <lh/numeric/fixed.h>

#endif // LH_NUMERIC_H