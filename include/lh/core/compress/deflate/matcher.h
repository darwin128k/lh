/**
 * @file matcher.h
 * @brief Allocation-free hash-chain match finder for DEFLATE encoding.
 *
 * The matcher finds earlier occurrences of the bytes at a position within the
 * 32 KiB DEFLATE window (::LH_COMPRESS_DEFLATE_WINDOW_SIZE). Every inserted
 * position is linked into a chain of positions that share the hash of their
 * first three bytes; a search walks that chain, newest first, up to the chain
 * limit and keeps the longest match.
 *
 * The caller owns the matcher, typically as a long-lived or static object:
 * it holds two tables of ::lh_u32_t positions and is too large for a small
 * stack. One matcher serves one input at a time. ::lh_compress_deflate_matcher_init
 * or ::lh_compress_deflate_matcher_reset starts a new input; positions are then
 * inserted in increasing order with ::lh_compress_deflate_matcher_insert. The
 * input is borrowed by each call and must be the same buffer for one run.
 * Positions are stored in 32 bits, so an input holds at most
 * ::LH_COMPRESS_DEFLATE_MATCHER_INPUT_LIMIT bytes.
 *
 * A longer chain limit finds longer matches and costs more time; it bounds
 * the work per position, so the search never degrades on repetitive input.
 */

#ifndef LH_COMPRESS_DEFLATE_MATCHER_H
#define LH_COMPRESS_DEFLATE_MATCHER_H

#include <lh/core/compress/deflate/matcher/fields.h>
#include <lh/core/compress/deflate/window.h>
#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>
#include <lh/core/size.h>

/** Number of entries in the hash-head table; a 15-bit hash of three bytes. */
#define LH_COMPRESS_DEFLATE_MATCHER_HASH_SIZE 32768U

/** Shortest match DEFLATE can encode. */
#define LH_COMPRESS_DEFLATE_MATCHER_MIN_MATCH 3U

/** Longest match DEFLATE can encode. */
#define LH_COMPRESS_DEFLATE_MATCHER_MAX_MATCH 258U

/** Largest input, in bytes, whose positions fit the matcher's tables. */
#define LH_COMPRESS_DEFLATE_MATCHER_INPUT_LIMIT 0xFFFFFFFEU

/** Chain limit that trades a little ratio for speed on typical web content. */
#define LH_COMPRESS_DEFLATE_MATCHER_CHAIN_LIMIT 32U

/** @brief Hash-chain state for one DEFLATE encoding run. */
struct lh_compress_deflate_matcher
{
    /** Members: ::lh_compress_deflate_matcher_fields. */
    lh_compress_deflate_matcher_fields(lh_usize_t, lh_u32_t, LH_COMPRESS_DEFLATE_MATCHER_HASH_SIZE,
                                       LH_COMPRESS_DEFLATE_WINDOW_SIZE);
};
/** @copybrief lh_compress_deflate_matcher */
typedef struct lh_compress_deflate_matcher lh_compress_deflate_matcher_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Return the stored chain_limit; self must be valid. */
lh_usize_t
lh_compress_deflate_matcher_get_chain_limit(const lh_compress_deflate_matcher_t *self);

/** @brief Store chain_limit without validation; self must be valid. */
lh_void
lh_compress_deflate_matcher_set_chain_limit(lh_compress_deflate_matcher_t *self, lh_usize_t value);

/**
 * @brief Borrow the read-only hash-head table of
 *        ::LH_COMPRESS_DEFLATE_MATCHER_HASH_SIZE entries; self must be valid.
 */
const lh_u32_t *
lh_compress_deflate_matcher_get_head(const lh_compress_deflate_matcher_t *self);

/** @brief Borrow the writable hash-head table; self must be valid. */
lh_u32_t *
lh_compress_deflate_matcher_get_head_mut(lh_compress_deflate_matcher_t *self);

/**
 * @brief Borrow the read-only previous-position table of
 *        ::LH_COMPRESS_DEFLATE_WINDOW_SIZE entries; self must be valid.
 */
const lh_u32_t *
lh_compress_deflate_matcher_get_prev(const lh_compress_deflate_matcher_t *self);

/** @brief Borrow the writable previous-position table; self must be valid. */
lh_u32_t *
lh_compress_deflate_matcher_get_prev_mut(lh_compress_deflate_matcher_t *self);

/**
 * @brief Hash the three bytes at @p at into a hash-head index.
 * @param at Borrowed bytes; at least three must be readable.
 * @return An index below ::LH_COMPRESS_DEFLATE_MATCHER_HASH_SIZE.
 * @pre @p at is valid for three bytes; not checked.
 */
lh_usize_t
lh_compress_deflate_matcher_get_hash(const lh_byte_t *at);

/**
 * @brief Count the equal bytes at an earlier and the current position.
 *
 * Compares the bytes at @p candidate and @p position with
 * ::lh_memory_std_compare, up to ::LH_COMPRESS_DEFLATE_MATCHER_MAX_MATCH bytes
 * and up to the end of the input. The ranges may overlap, as DEFLATE
 * back-references do.
 * @param input Borrowed input.
 * @param size Input size in bytes.
 * @param candidate Earlier position, below @p position.
 * @param position Current position, at most @p size.
 * @return The length of the common prefix, from zero to the smaller of 258 and
 *         the bytes left after @p position.
 * @pre Input is valid for @p size bytes and candidate < position <= size; not
 *      checked.
 */
lh_usize_t
lh_compress_deflate_matcher_get_match_size(const lh_byte_t *input, lh_usize_t size,
                                           lh_usize_t candidate, lh_usize_t position);

/**
 * @brief Tell whether @p candidate may match more than @p length bytes.
 *
 * A match longer than @p length needs the byte right after the first
 * @p length to agree, so one byte decides that a candidate cannot beat the
 * best match found so far. The search asks this before the full
 * ::lh_compress_deflate_matcher_get_match_size: a hash collision or an equally
 * long candidate costs one byte instead of a kernel call. True does not
 * promise a longer match, only that one is possible.
 * @param input Borrowed input.
 * @param candidate Earlier position, below @p position.
 * @param position Current position.
 * @param length Length to beat; position + length must be below the input size.
 * @return True if the bytes at candidate + length and position + length are
 *         equal; false if no match through @p candidate exceeds @p length.
 * @pre Input is valid through position + length; not checked.
 */
lh_bool_t
lh_compress_deflate_matcher_can_improve(const lh_byte_t *input, lh_usize_t candidate,
                                        lh_usize_t position, lh_usize_t length);

/**
 * @brief Forget every inserted position; the chain limit is kept.
 *
 * Clears the hash-head table only: the previous-position table is read just
 * through chains that start at a head, and every slot such a chain reaches was
 * written when its position was inserted.
 * @param self Matcher; remains caller-owned.
 * @pre self is valid.
 */
lh_void
lh_compress_deflate_matcher_reset(lh_compress_deflate_matcher_t *self);

/**
 * @brief Link @p position into the chain of its three-byte hash.
 *
 * A position with fewer than three bytes left can start no match and is
 * ignored. Positions must be inserted in increasing order.
 * @param self Matcher.
 * @param input Borrowed input of the current run.
 * @param size Input size, at most ::LH_COMPRESS_DEFLATE_MATCHER_INPUT_LIMIT.
 * @param position Position to insert, below @p size.
 * @pre self and input are valid, size is within the limit, position is below
 *      size and greater than every earlier inserted position; not checked.
 */
lh_void
lh_compress_deflate_matcher_insert(lh_compress_deflate_matcher_t *self, const lh_byte_t *input,
                                   lh_usize_t size, lh_usize_t position);

/**
 * @brief Find the longest earlier match for the bytes at @p position.
 *
 * Walks the chain of @p position's hash from the newest inserted position,
 * examining at most the chain limit of entries and none farther than
 * ::LH_COMPRESS_DEFLATE_WINDOW_SIZE bytes back. @p position itself must not
 * be inserted yet. The matcher is not changed.
 * @param self Matcher.
 * @param input Borrowed input of the current run.
 * @param size Input size in bytes.
 * @param position Current position, below @p size.
 * @param length Receives a length of 3 through 258 on success.
 * @param distance Receives a distance of 1 through 32768 on success.
 * @return True if a match of at least ::LH_COMPRESS_DEFLATE_MATCHER_MIN_MATCH
 *         bytes was found; false otherwise, with both outputs unchanged.
 * @pre All pointers are valid, the outputs are distinct and position < size.
 */
lh_bool_t
lh_compress_deflate_matcher_try_find(const lh_compress_deflate_matcher_t *self,
                                     const lh_byte_t *input, lh_usize_t size, lh_usize_t position,
                                     lh_usize_t *length, lh_usize_t *distance);

/**
 * @brief Prepare a matcher for a new input with a chain limit.
 * @param self Matcher; remains caller-owned.
 * @param chain_limit Most chain entries one search examines; zero disables
 *                    matching, and ::LH_COMPRESS_DEFLATE_MATCHER_CHAIN_LIMIT is
 *                    a balanced default.
 * @pre self is valid.
 */
lh_void
lh_compress_deflate_matcher_init(lh_compress_deflate_matcher_t *self, lh_usize_t chain_limit);

/**
 * @brief Find a match, checking ::lh_compress_deflate_matcher_try_find.
 * @copydetails lh_compress_deflate_matcher_try_find
 * @fails ::lh_runtime_error_code_invalid_range No match was found.
 */
lh_void
lh_compress_deflate_matcher_find(const lh_compress_deflate_matcher_t *self, const lh_byte_t *input,
                                 lh_usize_t size, lh_usize_t position, lh_usize_t *length,
                                 lh_usize_t *distance);

LH_COMPILER_EXTERN_C_END

#endif /* LH_COMPRESS_DEFLATE_MATCHER_H */
