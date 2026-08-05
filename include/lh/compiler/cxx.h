/**
 * @file cxx.h
 * @brief C++ compilation detection macros.
 *
 * Defines ::LH_COMPILER_CXX and ::LH_COMPILER_CXX_VERSION
 * when the translation unit is compiled as C++.
 */

#ifndef LH_COMPILER_CXX_H
#define LH_COMPILER_CXX_H

#ifdef __cplusplus

/**
 * @def LH_COMPILER_CXX
 * @brief Defined when the translation unit is compiled as C++.
 *
 * Presence of this macro indicates a C++ compiler is in use.
 * Expands to nothing — use `#ifdef LH_COMPILER_CXX` to test.
 */
#    define LH_COMPILER_CXX

/**
 * @def LH_COMPILER_CXX_VERSION
 * @brief Numeric C++ standard version in use.
 *
 * Alias for the predefined `__cplusplus` macro.
 * Common values:
 *
 * | Value     | Standard |
 * |-----------|----------|
 * | 199711L   | C++98    |
 * | 201103L   | C++11    |
 * | 201402L   | C++14    |
 * | 201703L   | C++17    |
 * | 202002L   | C++20    |
 *
 * Undefined in pure C compilation.
 */
#    define LH_COMPILER_CXX_VERSION __cplusplus

#endif /* __cplusplus */

#endif /* LH_COMPILER_CXX_H */