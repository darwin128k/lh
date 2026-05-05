/**
 * @file std.h
 * @brief Untyped byte memory operations: copy, move, fill, and compare.
 *
 * All sizes are in bytes (::lh_usize_t).
 * Pointers are checked in debug builds via ::lh_runtime_check_ref where applicable.
 */

#ifndef LH_MEMORY_STD_H
#define LH_MEMORY_STD_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Copy @p n bytes from @p src to @p dst in forward (low-to-high address) order.
 *
 * For overlapping ranges, behaviour is only defined when @p dst does not lie strictly
 * inside the source span (same constraints as `memcpy`).
 *
 * @param dst Destination buffer.
 * @param src Source buffer.
 * @param n Number of bytes to copy.
 *
 * @return Pointer one past the last byte written (i.e. `dst + n` as an untyped pointer).
 *
 * Example usage:
 * @code{.c}
 * lh_uchar_t buf[8];
 * lh_uchar_t src[4] = {1, 2, 3, 4};
 * lh_ptr end = lh_memory_std_copy(buf, src, 4);
 * // buf[0..3] == {1,2,3,4}; end == buf + 4
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr lh_memory_std_copy(lh_ptr dst, const lh_ptr src, lh_usize_t n);

/**
 * @brief Copy @p n bytes from @p src to @p dst while reversing byte order in the destination range.
 *
 * The first byte read from @p src is stored at the highest address of the @p n-byte destination
 * block; the last byte read is stored at @p dst.
 *
 * @param dst Start of the destination range (length @p n bytes).
 * @param src Source buffer.
 * @param n Number of bytes to copy.
 *
 * @return Pointer one past the last byte of the destination range (same as `dst + n`).
 *
 * Example usage:
 * @code{.c}
 * lh_uchar_t src[4] = {1, 2, 3, 4};
 * lh_uchar_t dst[4];
 * lh_memory_std_copy_rev(dst, src, 4);
 * // dst == {4, 3, 2, 1}
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr lh_memory_std_copy_rev(lh_ptr dst, const lh_ptr src, lh_usize_t n);

/**
 * @brief Copy @p n bytes from @p src to @p dst in reverse (high-to-low address) order.
 *
 * Suitable when the ranges overlap and @p dst lies inside the source span (analogous to the
 * backward copy path of `memmove`).
 *
 * @param dst Destination buffer.
 * @param src Source buffer.
 * @param n Number of bytes to copy.
 *
 * @return @p dst (the start of the destination range).
 *
 * Example usage:
 * @code{.c}
 * lh_uchar_t v[5] = {1, 2, 3, 4, 5};
 * lh_memory_std_rcopy(v + 1, v, 4);
 * // v == {1, 1, 2, 3, 4}
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr lh_memory_std_rcopy(lh_ptr dst, const lh_ptr src, lh_usize_t n);

/**
 * @brief Move @p n bytes from @p src to @p dst, correctly handling overlapping ranges.
 *
 * Chooses forward or reverse copy so that the result matches a non-overlapping copy of the
 * original source bytes (same semantics as `memmove`).
 *
 * @param dst Destination buffer.
 * @param src Source buffer.
 * @param n Number of bytes to move.
 *
 * @return Pointer one past the last byte written in the destination range.
 *
 * Example usage:
 * @code{.c}
 * lh_uchar_t buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};
 * lh_memory_std_move(buf + 2, buf, 6);
 * // buf == {1, 2, 1, 2, 3, 4, 5, 6}
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr lh_memory_std_move(lh_ptr dst, const lh_ptr src, lh_usize_t n);

/**
 * @brief Set each of @p n bytes starting at @p dst to @p val.
 *
 * @param dst Destination buffer.
 * @param val Byte value to fill.
 * @param n Number of bytes to write.
 *
 * @return Pointer one past the last byte written.
 *
 * Example usage:
 * @code{.c}
 * lh_uchar_t block[16];
 * lh_ptr end = lh_memory_std_set(block, 0, 16);
 * // all bytes in block are 0; end == block + 16
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr lh_memory_std_set(lh_ptr dst, lh_uchar_t val, lh_usize_t n);

/**
 * @brief Compare @p n bytes at @p lhs and @p rhs from the start toward higher addresses.
 *
 * @param lhs First buffer.
 * @param rhs Second buffer.
 * @param n Number of bytes to compare.
 *
 * @return Pointer to the first differing byte in @p lhs if a difference exists; otherwise
 *         ::lh_null if all @p n bytes are equal.
 *
 * Example usage:
 * @code{.c}
 * const lh_uchar_t a[] = {1, 2, 9, 4};
 * const lh_uchar_t b[] = {1, 2, 3, 4};
 * const lh_ptr diff = lh_memory_std_compare(a, b, 4);
 * // diff points to a[2] (value 9)
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr lh_memory_std_compare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);

/**
 * @brief Compare @p n bytes at @p lhs and @p rhs from the end toward lower addresses.
 *
 * @param lhs First buffer.
 * @param rhs Second buffer.
 * @param n Number of bytes to compare (the compared window is the last @p n bytes of each span).
 *
 * @return Pointer to the differing byte in @p lhs when scanning from the tail; ::lh_null if all
 *         @p n bytes match pairwise from both ends inward.
 *
 * Example usage:
 * @code{.c}
 * const lh_uchar_t a[] = {1, 2, 3, 9};
 * const lh_uchar_t b[] = {1, 2, 3, 4};
 * const lh_ptr diff = lh_memory_std_rcompare(a, b, 4);
 * // diff points to a[3] (last byte differs first when comparing from the end)
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr lh_memory_std_rcompare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);

LH_COMPILER_EXTERN_C_END

#endif // LH_MEMORY_STD_H
