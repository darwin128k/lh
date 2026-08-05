/**
 * @file memory.h
 * @brief Sized memory operations: copy, fill, compare, search.
 *
 * All sizes are in bytes (::lh_usize_t). Copy/move/compare helpers pass
 * <code>min(dst_size, src_size)</code> (or the analogous minimum for compare) to the
 * underlying ::lh_memory_std_* routines, so transfers never exceed the smaller span.
 *
 * Pointer validity is enforced where the implementation checks references (see each
 * function and ::lh/memory/std.h for the low-level behaviour).
 */

#ifndef LH_MEMORY_H
#define LH_MEMORY_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Copy up to @c min(dst_size, src_size) bytes
 *        from @p src to @p dst forward.
 *
 * Same overlap rules as ::lh_memory_std_copy for the effective length @c n.
 *
 * @param dst        Destination buffer.
 * @param dst_size   Size of @p dst in bytes (upper bound on @c n).
 * @param src        Source buffer.
 * @param src_size   Size of @p src in bytes (upper bound on @c n).
 *
 * @return Pointer one past the last byte written (i.e. @c dst + n).
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_copy(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size);

/**
 * @brief Copy up to @c min(dst_size, src_size) bytes
 *        from @p src into @p dst with reversed order
 *        in the destination (see ::lh_memory_std_copy_rev).
 *
 * @param dst        Start of destination range.
 * @param dst_size   Size of @p dst in bytes.
 * @param src        Source buffer.
 * @param src_size   Size of @p src in bytes.
 *
 * @return Pointer one past the last byte of the destination range.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_copy_rev(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size);

/**
 * @brief Copy up to @c min(dst_size, src_size) bytes
 *        using the reverse-direction path (see ::lh_memory_std_rcopy).
 *
 * @param dst        Destination buffer.
 * @param dst_size   Size of @p dst in bytes.
 * @param src        Source buffer.
 * @param src_size   Size of @p src in bytes.
 *
 * @return @p dst.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_rcopy(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size);

/**
 * @brief Move up to @c min(dst_size, src_size) bytes
 *        from @p src to @p dst (see ::lh_memory_std_move).
 *
 * @param dst        Destination buffer.
 * @param dst_size   Size of @p dst in bytes.
 * @param src        Source buffer.
 * @param src_size   Size of @p src in bytes.
 *
 * @return Pointer one past the last byte written in the destination range.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_move(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size);

/**
 * @brief Set every byte in @p dst
 *        to @p val (see ::lh_memory_std_set).
 *
 * @param dst   Destination buffer.
 * @param size  Number of bytes to write.
 * @param val   Byte value.
 *
 * @return Pointer one past the last byte written.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_set(lh_ptr dst, lh_usize_t size, lh_uchar_t val);

/**
 * @brief Compare the first @c min(lhs_size, rhs_size) bytes
 *        of @p lhs and @p rhs forward (see ::lh_memory_std_compare).
 *
 * @param lhs       First buffer.
 * @param lhs_size  Size of @p lhs in bytes.
 * @param rhs       Second buffer.
 * @param rhs_size  Size of @p rhs in bytes.
 *
 * @return Pointer to the first differing byte in @p lhs,
 *         or ::lh_null if all compared bytes match.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_compare(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size);

/**
 * @brief Compare the last @c n bytes of each span, where @c n is @c min(lhs_size, rhs_size),
 *        scanning from the ends inward (see ::lh_memory_std_rcompare).
 *
 * The compared window is the suffix of length @c n in each buffer.
 *
 * @param lhs       First buffer.
 * @param lhs_size  Size of @p lhs in bytes.
 * @param rhs       Second buffer.
 * @param rhs_size  Size of @p rhs in bytes.
 *
 * @return Pointer into @p lhs to the differing byte in that suffix window,
 *         or ::lh_null if equal.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_rcompare(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size);

/**
 * @brief Find the leftmost offset in @p lhs where bytes
 *        match @p rhs under ::lh_memory_compare.
 *
 * Equivalent to ::lh_memory_find_step with @p step @c 1: every byte offset is tried.
 * For fixed-size elements (e.g. ::lh_wchar_t) use ::lh_memory_find_step with
 * @p step equal to @p rhs_size so matches are not reported starting mid-element.
 *
 * Each candidate position uses the remaining length
 * of @p lhs versus @p rhs_size (minimum wins),
 * same rule as ::lh_memory_compare.
 *
 * @param lhs       Haystack.
 * @param lhs_size  Size of @p lhs in bytes.
 * @param rhs       Needle.
 * @param rhs_size  Size of @p rhs in bytes.
 *
 * @return Pointer to the start of the first match,
 *         or ::lh_null if none.
 *
 * @see lh_memory_find_step
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_find(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size);

/**
 * @brief Like ::lh_memory_find, but only tries start offsets @c base, @c base+step,
 * @c base+2*step, … relative to @p lhs (byte offsets into the haystack).
 *
 * @param lhs       Haystack.
 * @param lhs_size  Size of @p lhs in bytes.
 * @param rhs       Needle.
 * @param rhs_size  Size of @p rhs in bytes.
 * @param step      Byte distance between candidate starts; must be non-zero.
 *                  Use @p rhs_size to search aligned fixed-size records (e.g. wide characters).
 *
 * @return Pointer to the start of the first match, or ::lh_null if none.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_find_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size,
                    lh_usize_t step);

/**
 * @brief Find the rightmost offset in @p lhs
 * where a suffix matches @p rhs under ::lh_memory_rcompare.
 *
 * Equivalent to ::lh_memory_rfind_step with @p step @c 1.
 *
 * @param lhs       Haystack.
 * @param lhs_size  Size of @p lhs in bytes.
 * @param rhs       Needle.
 * @param rhs_size  Size of @p rhs in bytes.
 *
 * @return Pointer to the start of the last matching occurrence,
 *         or ::lh_null if none.
 *
 * @see lh_memory_rfind_step
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_rfind(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size);

/**
 * @brief Like ::lh_memory_rfind, but only considers start offsets on the same @p step grid
 *        as ::lh_memory_find_step.
 *
 * @param lhs       Haystack.
 * @param lhs_size  Size of @p lhs in bytes.
 * @param rhs       Needle.
 * @param rhs_size  Size of @p rhs in bytes.
 * @param step      Byte distance between candidate starts; must be non-zero.
 *
 * @return Pointer to the start of the last match, or ::lh_null if none.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_rfind_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size,
                     lh_usize_t step);

/**
 * @brief Fill @p dst with @p dst_size bytes
 *        by repeating the @p src_size-byte pattern at @p src.
 *
 * If @p dst_size or @p src_size is zero,
 * returns ::lh_null and writes nothing.
 *
 * @param dst        Destination buffer.
 * @param dst_size   Total number of bytes to fill.
 * @param src        Pattern bytes.
 * @param src_size   Length of the pattern in bytes.
 *
 * @return Pointer one past the last byte written.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_set_pattern(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_H */
