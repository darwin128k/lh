/**
 * @file algorithm.h
 * @brief Small algorithm-like macros for low-level memory and value operations.
 *
 * Provides a small set of macros for performing common algorithms on sequences of values.
 * All operands are fully parenthesized to avoid precedence issues at call sites.
 */

#ifndef LH_UTIL_ALGORITHM_H
#define LH_UTIL_ALGORITHM_H

#include <lh/bool.h>
#include <lh/config.h>
#include <lh/size.h>
#include <lh/util/ptr.h>

/**
 * @def LH_ALGORITHM_COMPARE_BLOCK
 * @brief Element block size ::lh_algorithm_compare (and the matching fast path in
 *        ::lh_memory_find_step, src/lh/memory.c) scans branchlessly before falling
 *        back to a precise, element-by-element scan of the block that turned out
 *        to differ/match.
 *
 * Branchless (no early exit) so the compiler is free to auto-vectorize the
 * block loop; only the block containing a mismatch/hit (if any) pays for a
 * branch per element.
 *
 * Configurable via ::LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK
 * (`-DLH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK=<n>` at CMake configure time,
 * or edit `include/lh/config.h` directly on a non-CMake build).
 */
#define LH_ALGORITHM_COMPARE_BLOCK ((lh_usize_t)LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK)

/**
 * @brief Swap two values using a temporary variable.
 *
 * @param T Type of both values.
 * @param a First lvalue to swap.
 * @param b Second lvalue to swap.
 *
 * @code
 * int x = 10;
 * int y = 20;
 * lh_algorithm_swap(int, x, y);
 * // x == 20, y == 10
 * @endcode
 */
#define lh_algorithm_swap(T, a, b)                                                                 \
    do                                                                                             \
    {                                                                                              \
        T temp = (a);                                                                              \
        (a) = (b);                                                                                 \
        (b) = temp;                                                                                \
    } while (0)

/**
 * @brief Swap two integer-like values using XOR (without a temporary variable).
 *
 * @param a First integer lvalue to swap.
 * @param b Second integer lvalue to swap.
 *
 * @warning
 * Do not use when @p a and @p b refer to the same object.
 * Only valid for integer-like types that support XOR operations.
 *
 * @code
 * unsigned a = 1U;
 * unsigned b = 2U;
 * lh_algorithm_swap_no_temp(a, b);
 * // a == 2U, b == 1U
 * @endcode
 */
#define lh_algorithm_swap_no_temp(a, b)                                                            \
    do                                                                                             \
    {                                                                                              \
        (a) ^= (b);                                                                                \
        (b) ^= (a);                                                                                \
        (a) ^= (b);                                                                                \
    } while (0)

/**
 * @brief Copy @p n elements from source to destination in forward direction.
 *
 * @param T Element type.
 * @param dst Destination pointer.
 * @param src Source pointer.
 * @param n Number of elements to copy.
 *
 * @code
 * int src[3] = {1, 2, 3};
 * int dst[3] = {0, 0, 0};
 * lh_algorithm_copy(int, dst, src, 3);
 * // dst == {1, 2, 3}
 * @endcode
 */
#define lh_algorithm_copy(T, dst, src, n)                                                          \
    do                                                                                             \
    {                                                                                              \
        T *d = lh_ptr_cast(T, dst);                                                                \
        const T *s = lh_ptr_ccast(T, src);                                                         \
                                                                                                   \
        while (n--)                                                                                \
        {                                                                                          \
            *d++ = *s++;                                                                           \
        }                                                                                          \
    } while (0)

/**
 * @brief Copy @p n elements from source to destination in reverse direction.
 *
 * @param T Element type.
 * @param dst Destination pointer.
 * @param src Source pointer.
 * @param n Number of elements to copy.
 *
 * @note
 * Useful when ranges overlap and destination starts inside source range.
 *
 * @code
 * int v[5] = {1, 2, 3, 4, 5};
 * lh_algorithm_rcopy(int, v + 1, v, 4);
 * // v == {1, 1, 2, 3, 4}
 * @endcode
 */
#define lh_algorithm_rcopy(T, dst, src, n)                                                         \
    do                                                                                             \
    {                                                                                              \
        T *d = lh_ptr_add_by_offset_unsafe(T, dst, n);                                             \
        const T *s = lh_ptr_add_by_offset_unsafe(T, src, n);                                       \
                                                                                                   \
        while (n--)                                                                                \
        {                                                                                          \
            *(--d) = *(--s);                                                                       \
        }                                                                                          \
    } while (0)

/**
 * @brief Fill @p n elements in destination with the same value.
 *
 * @param T Element type.
 * @param dst Destination pointer.
 * @param val Value assigned to each element.
 * @param n Number of elements to write.
 *
 * @code
 * int out[4];
 * lh_algorithm_set(int, out, 7, 4);
 * // out == {7, 7, 7, 7}
 * @endcode
 */
#define lh_algorithm_set(T, dst, val, n)                                                           \
    do                                                                                             \
    {                                                                                              \
        T *d = lh_ptr_cast(T, dst);                                                                \
                                                                                                   \
        while (n--)                                                                                \
        {                                                                                          \
            *d++ = val;                                                                            \
        }                                                                                          \
    } while (0)

/**
 * @brief Find first occurrence of @p rhs in the @p n-element sequence starting at @p lhs.
 *
 * @param T Element type.
 * @param lhs Sequence pointer.
 * @param rhs Value to find.
 * @param n Number of elements to scan.
 *
 * @return In the enclosing function, returns pointer to first matching element in @p lhs.
 *
 * @note
 * This macro performs `return l;` on match, so it must be used inside a
 * function whose return type is compatible with `const T *`.
 */
#define lh_algorithm_find_byte(T, lhs, rhs, n)                                                     \
    do                                                                                             \
    {                                                                                              \
        const T *l = lh_ptr_ccast(T, lhs);                                                         \
                                                                                                   \
        while (n--)                                                                                \
        {                                                                                          \
            if (*l == (rhs))                                                                       \
            {                                                                                      \
                return l;                                                                          \
            }                                                                                      \
            l++;                                                                                   \
        }                                                                                          \
    } while (0)

/**
 * @brief Compare two sequences from start to end and return on first mismatch.
 *
 * @param T Element type.
 * @param lhs First sequence pointer.
 * @param rhs Second sequence pointer.
 * @param n Number of elements to compare.
 *
 * @return In the enclosing function, returns pointer to mismatched element in @p lhs.
 *
 * @note
 * This macro performs `return l;` on mismatch, so it must be used inside a
 * function whose return type is compatible with `const T *`.
 *
 * @code
 * static const int *first_diff(const int *a, const int *b, size_t n) {
 *     lh_algorithm_compare(int, a, b, n);
 *     return 0;
 * }
 * @endcode
 */
#define lh_algorithm_compare(T, lhs, rhs, n)                                                       \
    do                                                                                             \
    {                                                                                              \
        const T *l = lh_ptr_ccast(T, lhs);                                                         \
        const T *r = lh_ptr_ccast(T, rhs);                                                         \
                                                                                                   \
        while (n >= LH_ALGORITHM_COMPARE_BLOCK)                                                    \
        {                                                                                          \
            lh_bool_t block_diff = lh_bool_false;                                                  \
            lh_usize_t block_i;                                                                    \
            for (block_i = 0; block_i < LH_ALGORITHM_COMPARE_BLOCK; ++block_i)                     \
            {                                                                                      \
                block_diff = (lh_bool_t)(block_diff | (l[block_i] != r[block_i]));                 \
            }                                                                                      \
            if (block_diff)                                                                        \
            {                                                                                      \
                break; /* precise scan below finds the exact mismatch in this block */             \
            }                                                                                      \
            l += LH_ALGORITHM_COMPARE_BLOCK;                                                       \
            r += LH_ALGORITHM_COMPARE_BLOCK;                                                       \
            n -= LH_ALGORITHM_COMPARE_BLOCK;                                                       \
        }                                                                                          \
                                                                                                   \
        while (n--)                                                                                \
        {                                                                                          \
            if (*l != *r)                                                                          \
            {                                                                                      \
                return l;                                                                          \
            }                                                                                      \
            l++;                                                                                   \
            r++;                                                                                   \
        }                                                                                          \
    } while (0)

/**
 * @brief Compare two sequences from end to start and return on first mismatch.
 *
 * @param T Element type.
 * @param lhs First sequence pointer.
 * @param rhs Second sequence pointer.
 * @param n Number of elements to compare.
 *
 * @return In the enclosing function, returns pointer to mismatched element in @p lhs.
 *
 * @note
 * This macro performs `return l;` on mismatch, so it must be used inside a
 * function whose return type is compatible with `const T *`.
 *
 * @code
 * static const int *last_diff(const int *a, const int *b, size_t n) {
 *     lh_algorithm_rcompare(int, a, b, n);
 *     return 0;
 * }
 * @endcode
 */
#define lh_algorithm_rcompare(T, lhs, rhs, n)                                                      \
    do                                                                                             \
    {                                                                                              \
        const T *l = lh_ptr_add_unsafe(T, lhs, lh_math_sub_one(n));                                \
        const T *r = lh_ptr_add_unsafe(T, rhs, lh_math_sub_one(n));                                \
                                                                                                   \
        while (n--)                                                                                \
        {                                                                                          \
            if (*l != *r)                                                                          \
            {                                                                                      \
                return l;                                                                          \
            }                                                                                      \
            l--;                                                                                   \
            r--;                                                                                   \
        }                                                                                          \
    } while (0)

/**
 * @brief Copy @p n elements from source to destination while reversing order.
 *
 * @param T Element type.
 * @param dst Destination pointer.
 * @param src Source pointer.
 * @param n Number of elements to copy.
 *
 * @code
 * int src[4] = {1, 2, 3, 4};
 * int dst[4];
 * lh_algorithm_copy_rev(int, dst, src, 4);
 * // dst == {4, 3, 2, 1}
 * @endcode
 */
#define lh_algorithm_copy_rev(T, dst, src, n)                                                      \
    do                                                                                             \
    {                                                                                              \
        T *d = lh_ptr_add_by_offset_unsafe(T, dst, n);                                             \
        const T *s = lh_ptr_ccast(T, src);                                                         \
                                                                                                   \
        while (n--)                                                                                \
        {                                                                                          \
            *--d = *s++;                                                                           \
        }                                                                                          \
    } while (0)

#endif /* LH_UTIL_ALGORITHM_H */