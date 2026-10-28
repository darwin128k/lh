/**
 * @file memory.h
 * @brief Sized memory operations: copy, fill, compare, search, sentinel scan.
 *
 * All sizes are in bytes (::lh_usize_t). Copy/move/compare helpers pass
 * <code>min(dst_size, src_size)</code> (or the analogous minimum for compare) to the
 * underlying ::lh_memory_std_* routines, so transfers never exceed the smaller span.
 *
 * Sized search (::lh_memory_find / ::lh_memory_find_step) takes an explicit haystack
 * length. Sentinel scan (::lh_memory_scan / ::lh_memory_scan_step) walks from @c lhs
 * until the first match or the remaining address space; callers do not pass
 * @c SIZE_MAX.
 *
 * Pointer validity is enforced where the implementation checks references (see each
 * function and ::lh/memory/std.h for the low-level behaviour).
 */

#ifndef LH_MEMORY_H
#define LH_MEMORY_H

/* Umbrella: the memory core. This header also carries the sized helpers
   (lh_memory_* in src/lh/memory.c), which is why its name and the directory
   lh/memory/ collide — see ARCHITECTURE.md. */
#include <lh/memory/std.h>
#include <lh/memory/tree.h>
#include <lh/memory/typed/allocated.h>

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
lh_ptr
lh_memory_set(lh_ptr dst, lh_usize_t size, lh_byte_t val);

/**
 * @brief Set @p count 16-bit words from @p dst to @p val.
 *
 * ::lh_memory_set for a two-byte value: a row of RGB565 pixels, for one.
 * Unrolled eight words a turn, which the compiler vectorizes at -O2.
 *
 * @param dst   First word (aligned for ::lh_u16_t).
 * @param count Number of words to write.
 * @param val   Word value.
 *
 * @return Pointer one past the last word written.
 */
lh_ptr
lh_memory_set_u16(lh_u16_t *dst, lh_usize_t count, lh_u16_t val);

/**
 * @brief Set @p count 32-bit words from @p dst to @p val.
 *
 * ::lh_memory_set for a four-byte value: a row of ARGB8888 pixels, for one.
 * Unrolled eight words a turn, which the compiler vectorizes at -O2.
 *
 * @param dst   First word (aligned for ::lh_u32_t).
 * @param count Number of words to write.
 * @param val   Word value.
 *
 * @return Pointer one past the last word written.
 */
lh_ptr
lh_memory_set_u32(lh_u32_t *dst, lh_usize_t count, lh_u32_t val);

/**
 * @brief XOR the first @c min(dst_size, lhs_size, rhs_size) bytes
 *        of @p lhs and @p rhs into @p dst (see ::lh_memory_std_xor).
 *
 * @param dst       Destination buffer; may be @p lhs or @p rhs.
 * @param dst_size  Size of @p dst in bytes (upper bound on @c n).
 * @param lhs       First operand.
 * @param lhs_size  Size of @p lhs in bytes (upper bound on @c n).
 * @param rhs       Second operand.
 * @param rhs_size  Size of @p rhs in bytes (upper bound on @c n).
 *
 * @return Pointer one past the last byte written (i.e. @c dst + n).
 */
lh_ptr
lh_memory_xor(lh_ptr dst, lh_usize_t dst_size, const lh_ptr lhs, lh_usize_t lhs_size,
              const lh_ptr rhs, lh_usize_t rhs_size);

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
const lh_ptr
lh_memory_rcompare(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size);

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
lh_ptr
lh_memory_set_pattern(lh_ptr dst, lh_usize_t dst_size, const lh_ptr src, lh_usize_t src_size);

/**
 * @brief Whether @p lhs_size is the address-space bound of @p lhs
 *        (::lh_memory_scan_bound): a sentinel scan with no real length.
 *
 * Such a haystack ends wherever its sentinel is, so nothing may be read past a
 * match: the find bricks below take their element-by-element walk for it.
 */
lh_bool_t
lh_memory_is_unbounded(const lh_ptr lhs, lh_usize_t lhs_size);

/**
 * @brief Whether the @p size bytes at @p lhs and @p rhs are the same.
 *
 * The yes/no question over ::lh_memory_std_compare, which answers where the
 * first difference is. Zero @p size is equal.
 *
 * @param lhs  First block.
 * @param rhs  Second block.
 * @param size Bytes to compare.
 */
lh_bool_t
lh_memory_is_equal(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t size);

/**
 * @brief First byte equal to @p byte at offsets 0, @p step, 2 * @p step, ... of @p lhs.
 *
 * Reads exactly the candidates up to the match, so it is safe for a sentinel
 * scan (::lh_memory_is_unbounded).
 *
 * @param lhs      Haystack.
 * @param lhs_size Size of @p lhs in bytes, or its address-space bound.
 * @param byte     Byte to find.
 * @param step     Byte distance between candidates; non-zero.
 * @return The first match, or ::lh_null.
 */
const lh_ptr
lh_memory_find_byte_by_step(const lh_ptr lhs, lh_usize_t lhs_size, lh_byte_t byte, lh_usize_t step);

/**
 * @brief First @p unit_size-byte unit of @p lhs equal to @p unit, unit by unit.
 *
 * Candidates are the whole units at offsets 0, @p unit_size, ...; each is read
 * once, up to the match, so it is safe for a sentinel scan.
 *
 * @param lhs       Haystack.
 * @param lhs_size  Size of @p lhs in bytes, or its address-space bound.
 * @param unit      The unit to find (e.g. one ::lh_wchar_t).
 * @param unit_size Size of @p unit in bytes; non-zero.
 * @return The first match, or ::lh_null.
 */
const lh_ptr
lh_memory_find_unit_by_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr unit,
                            lh_usize_t unit_size);

/**
 * @brief First @p unit_size-byte unit of @p lhs equal to @p unit, on the unit grid.
 *
 * Composed of ::lh_memory_std_find: the kernel searches the bytes, and a hit
 * that does not start on a unit boundary of @p lhs resumes the search at the
 * next unit. The kernel reads whole words or vectors, so @p lhs_size must be
 * the real haystack length: never use it for a sentinel scan
 * (::lh_memory_is_unbounded) — that takes ::lh_memory_find_unit_by_step.
 *
 * @param lhs       Haystack.
 * @param lhs_size  Real size of @p lhs in bytes.
 * @param unit      The unit to find (e.g. one ::lh_wchar_t).
 * @param unit_size Size of @p unit in bytes; non-zero.
 * @return The first match, or ::lh_null.
 */
const lh_ptr
lh_memory_find_unit(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr unit, lh_usize_t unit_size);

/**
 * @brief First match of a multi-byte needle at offsets 0, @p step, ... of @p lhs.
 *
 * Tests the needle's first byte in place and calls ::lh_memory_compare only
 * where it matches. Reads at most the needle's length at a candidate, so it is
 * safe for a sentinel scan that holds the needle.
 *
 * @param lhs      Haystack.
 * @param lhs_size Size of @p lhs in bytes, or its address-space bound.
 * @param rhs      Needle.
 * @param rhs_size Size of @p rhs in bytes; non-zero.
 * @param step     Byte distance between candidates; non-zero.
 * @return The first match, or ::lh_null.
 */
const lh_ptr
lh_memory_find_needle_by_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs,
                              lh_usize_t rhs_size, lh_usize_t step);

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
 * Dispatches to the bricks above: with a real length ::lh_memory_std_find on a
 * byte grid and ::lh_memory_find_unit on a unit grid; for a sentinel
 * scan (::lh_memory_is_unbounded) only the walks that read nothing past the
 * match.
 *
 * @return Pointer to the start of the first match, or ::lh_null if none.
 */
const lh_ptr
lh_memory_find_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size,
                    lh_usize_t step);

/**
 * @brief Offset of the last candidate on a @p step grid where @p rhs_size bytes fit.
 *
 * `lhs_size - rhs_size`, rounded down to a multiple of @p step (a mask when
 * @p step is a power of two). The reverse walks start here.
 *
 * @param lhs_size Haystack size in bytes; at least @p rhs_size.
 * @param rhs_size Needle size in bytes.
 * @param step     Grid step; non-zero.
 */
lh_usize_t
lh_memory_get_last_step_offset(lh_usize_t lhs_size, lh_usize_t rhs_size, lh_usize_t step);

/**
 * @brief Last match of @p rhs at offsets on a @p step grid of @p lhs, walking back.
 *
 * Starts at ::lh_memory_get_last_step_offset; tests the needle's first byte in
 * place and compares the rest with ::lh_memory_std_compare only there.
 *
 * @param lhs      Haystack.
 * @param lhs_size Size of @p lhs in bytes.
 * @param rhs      Needle.
 * @param rhs_size Size of @p rhs in bytes; non-zero.
 * @param step     Byte distance between candidates; non-zero.
 * @return The last match, or ::lh_null (also when @p rhs does not fit).
 */
const lh_ptr
lh_memory_rfind_needle_by_step(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs,
                               lh_usize_t rhs_size, lh_usize_t step);

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
const lh_ptr
lh_memory_rfind(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size);

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
const lh_ptr
lh_memory_find(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size);

/**
 * @brief Like ::lh_memory_scan, but only tries start offsets on a @p step grid.
 *
 * Use @p step equal to @p rhs_size for fixed-size units (e.g. ::lh_wchar_t).
 * The bound is remaining bytes from @p lhs, not a fake element count.
 *
 * @param lhs       Haystack start.
 * @param rhs       Needle.
 * @param rhs_size  Size of @p rhs in bytes.
 * @param step      Byte distance between candidate starts; must be non-zero.
 *
 * @return Pointer to the start of the first match, or ::lh_null if none
 *         before the address-space bound.
 *
 * @see lh_memory_find_step
 */
const lh_ptr
lh_memory_scan_step(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t rhs_size, lh_usize_t step);

/**
 * @brief Sentinel scan: find the leftmost match of @p rhs starting at @p lhs,
 *        with no caller-supplied haystack length.
 *
 * The scan bound is the remaining address space from @p lhs
 * (<code>::LH_USIZE_T_MAX - ::lh_ptr_to_uaddr(lhs)</code> bytes). Equivalent to
 * ::lh_memory_scan_step with @p step @c 1.
 *
 * @param lhs       Haystack start.
 * @param rhs       Needle.
 * @param rhs_size  Size of @p rhs in bytes.
 *
 * @return Pointer to the start of the first match, or ::lh_null if none
 *         before the address-space bound.
 *
 * @see lh_memory_scan_step
 * @see lh_memory_find
 */
const lh_ptr
lh_memory_scan(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t rhs_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_H */
