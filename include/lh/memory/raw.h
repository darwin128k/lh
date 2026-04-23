/**
 * @file raw.h
 * @brief Untyped byte memory operations using half-open pointer spans.
 *
 * Each range is described by a start pointer and an end pointer: bytes covered are
 * <code>[ptr, ptr_end)</code>. Sizes in bytes are computed as the difference between
 * end and start (see ::lh_ptr_udiff). Behaviour matches ::lh/memory.h for the derived
 * lengths: transfers and comparisons use @c min(lhs_size, rhs_size) where applicable.
 *
 * In debug builds, end pointers are validated with ::lh_runtime_check_ref.
 */

#ifndef LH_MEMORY_RAW_H
#define LH_MEMORY_RAW_H

#include <lh/attribute.h>
#include <lh/compiler.h>
#include <lh/ptr.h>

LH_COMPILER(EXTERN_C_BEGIN)

/**
 * @brief Copy up to @c min(dst_end - dst, src_end - src) bytes
 *        from @p src to @p dst forward (see ::lh_memory_copy).
 *
 * @param dst      Start of the destination span.
 * @param dst_end  One past the last writable byte in the destination.
 * @param src      Start of the source span.
 * @param src_end  One past the last readable byte in the source.
 *
 * @return Pointer one past the last byte written (i.e. @c dst + n).
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_raw_copy(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end);

/**
 * @brief Copy up to @c min(dst_end - dst, src_end - src) bytes
 *        with reversed byte order in the destination (see ::lh_memory_copy_rev).
 *
 * @param dst      Start of the destination span.
 * @param dst_end  One past the last byte of the destination range.
 * @param src      Start of the source span.
 * @param src_end  One past the last byte of the source.
 *
 * @return Pointer one past the last byte of the destination range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_raw_copy_rev(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src,
                              const lh_ptr src_end);

/**
 * @brief Copy up to @c min(dst_end - dst, src_end - src) bytes
 *        using the high-to-low copy path (see ::lh_memory_rcopy).
 *
 * @param dst      Start of the destination span.
 * @param dst_end  One past the last writable byte.
 * @param src      Start of the source span.
 * @param src_end  One past the last readable byte.
 *
 * @return @p dst.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_raw_rcopy(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src,
                           const lh_ptr src_end);

/**
 * @brief Move up to @c min(dst_end - dst, src_end - src) bytes
 *        from @p src to @p dst, handling overlap (see ::lh_memory_move).
 *
 * @param dst      Start of the destination span.
 * @param dst_end  One past the last writable byte.
 * @param src      Start of the source span.
 * @param src_end  One past the last readable byte.
 *
 * @return Pointer one past the last byte written in the destination range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_raw_move(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src, const lh_ptr src_end);

/**
 * @brief Set every byte in <code>[dst, dst_end)</code> to @p val (see ::lh_memory_set).
 *
 * @param dst      Start of the span to fill.
 * @param dst_end  One past the last byte to write.
 * @param val      Byte value.
 *
 * @return Pointer one past the last byte written (i.e. @p dst_end).
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_raw_set(lh_ptr dst, const lh_ptr dst_end, lh_uchar_t val);

/**
 * @brief Compare the first @c min(lhs_end - lhs, rhs_end - rhs) bytes
 *        of @p lhs and @p rhs forward (see ::lh_memory_compare).
 *
 * @param lhs      Start of the first span.
 * @param lhs_end  One past the last byte of @p lhs.
 * @param rhs      Start of the second span.
 * @param rhs_end  One past the last byte of @p rhs.
 *
 * @return Pointer to the first differing byte in @p lhs,
 *         or ::lh_null if all compared bytes match.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_ptr lh_memory_raw_compare(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                                   const lh_ptr rhs_end);

/**
 * @brief Compare suffixes of length @c min(lhs_end - lhs, rhs_end - rhs),
 *        scanning from the ends inward (see ::lh_memory_rcompare).
 *
 * @param lhs      Start of the first span.
 * @param lhs_end  One past the last byte of @p lhs.
 * @param rhs      Start of the second span.
 * @param rhs_end  One past the last byte of @p rhs.
 *
 * @return Pointer into @p lhs to the differing byte in that suffix window,
 *         or ::lh_null if equal.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_ptr lh_memory_raw_rcompare(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                                    const lh_ptr rhs_end);

/**
 * @brief Find the leftmost match of <code>[rhs, rhs_end)</code> inside
 *        <code>[lhs, lhs_end)</code> (see ::lh_memory_find).
 *
 * @param lhs      Haystack start.
 * @param lhs_end  One past the last haystack byte.
 * @param rhs      Needle start.
 * @param rhs_end  One past the last needle byte.
 *
 * @return Pointer to the start of the first match, or ::lh_null if none.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_ptr lh_memory_raw_find(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                                const lh_ptr rhs_end);

/**
 * @brief Find the rightmost match using the same suffix comparison rule as
 *        ::lh_memory_rfind (see ::lh_memory_rfind).
 *
 * @param lhs      Haystack start.
 * @param lhs_end  One past the last haystack byte.
 * @param rhs      Needle start.
 * @param rhs_end  One past the last needle byte.
 *
 * @return Pointer to the start of the last matching occurrence, or ::lh_null if none.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_ptr lh_memory_raw_rfind(const lh_ptr lhs, const lh_ptr lhs_end, const lh_ptr rhs,
                                 const lh_ptr rhs_end);

/**
 * @brief Fill <code>[dst, dst_end)</code> by repeating the pattern
 *        <code>[src, src_end)</code> (see ::lh_memory_set_pattern).
 *
 * If the destination or pattern length is zero, returns ::lh_null and writes nothing.
 *
 * @param dst      Start of the span to fill.
 * @param dst_end  One past the last byte to write.
 * @param src      Start of the pattern.
 * @param src_end  One past the last pattern byte.
 *
 * @return Pointer one past the last byte written, or ::lh_null when nothing is written.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_raw_set_pattern(lh_ptr dst, const lh_ptr dst_end, const lh_ptr src,
                                 const lh_ptr src_end);

LH_COMPILER(EXTERN_C_END)

#endif // LH_MEMORY_RAW_H
