/**
 * @file array.h
 * @brief Growable, heap-owning typed array (::lh_array_t).
 *
 * A array owns a typed, heap-allocated block (::lh_memory_typed_allocated_t)
 * whose bounds always describe the full allocated @c capacity, plus a
 * @c size field tracking how many of those slots are actually in use
 * (@c size <= capacity). Growing @c size past the current capacity
 * reallocates the underlying block.
 *
 * @see lh_memory_typed_allocated_t
 */

#ifndef LH_ARRAY_H
#define LH_ARRAY_H

#include <lh/memory/typed/allocated.h>

/**
 * @struct lh_array
 * @brief Typed, growable array backed by a heap-allocated block.
 *
 * @c typed.bounds always spans the full allocated capacity; @c size is the
 * number of elements actually in use, from the start of that capacity.
 */
struct lh_array
{
    lh_memory_typed_allocated_t typed; /**< Owns the block; bounds == capacity. */
    lh_usize_t size;                   /**< Elements in use; size <= capacity. */
};
typedef struct lh_array lh_array_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return a pointer to the @c typed field, after validating @p self.
 *
 * The single validation point for @c typed: every other function that needs
 * it goes through this (or ::lh_array_get_typed_as_const) instead of
 * checking @p self again and reaching into the field directly.
 *
 * @param self Array to inspect.
 */
lh_memory_typed_allocated_t *
lh_array_get_typed(lh_array_t *self);

/**
 * @brief `const` counterpart to ::lh_array_get_typed.
 * @param self Array to inspect.
 */
const lh_memory_typed_allocated_t *
lh_array_get_typed_as_const(const lh_array_t *self);

/**
 * @brief Return the number of elements the current allocation can hold.
 * @param self Array to inspect.
 */
lh_usize_t
lh_array_get_capacity(const lh_array_t *self);

/**
 * @brief Return the number of elements currently in use.
 * @param self Array to inspect.
 */
lh_usize_t
lh_array_get_size(const lh_array_t *self);

/**
 * @brief Return the size in bytes of one element.
 * @param self Array to inspect.
 */
lh_usize_t
lh_array_get_type_size(const lh_array_t *self);

/**
 * @brief Return a pointer to the first element.
 * @param self Array to inspect.
 */
lh_ptr
lh_array_get_begin(const lh_array_t *self);

/**
 * @brief Alias for ::lh_array_get_begin.
 * @param self Array to inspect.
 */
lh_ptr
lh_array_get_data(const lh_array_t *self);

/**
 * @brief Return a pointer one past the last element in use.
 *
 * Unlike the underlying typed storage (whose end sits at @c capacity), this
 * reflects @c size, matching begin/end iteration idioms.
 *
 * @param self Array to inspect.
 */
lh_ptr
lh_array_get_end(const lh_array_t *self);

/**
 * @brief True when @p self has no elements in use.
 * @param self Array to inspect.
 */
lh_bool_t
lh_array_is_empty(const lh_array_t *self);

/**
 * @brief Drop every element in use without releasing the allocated block.
 *
 * Unlike ::lh_array_deinit, the current capacity is kept, so pushing back
 * afterwards does not need to reallocate.
 *
 * @param self Array to clear.
 */
lh_void
lh_array_clear(lh_array_t *self);

/**
 * @brief Initialize @p self as an empty array of elements sized @p type_size.
 *
 * No allocation happens until the array is grown.
 *
 * @param self      Array to initialize.
 * @param type_size Size of one element in bytes.
 */
lh_void
lh_array_init(lh_array_t *self, lh_usize_t type_size);

/**
 * @brief Free the block owned by @p self and reset it to an empty array.
 *
 * Element type size is preserved; only the owned allocation is released.
 *
 * @param self Array to deinitialize.
 */
lh_void
lh_array_deinit(lh_array_t *self);

/**
 * @brief Ensure @p self can hold at least @p min_capacity elements.
 *
 * No-op when the current capacity is already sufficient. Otherwise reallocates
 * the underlying block to exactly @p min_capacity elements — this function
 * applies no growth strategy of its own; callers that need amortized growth
 * (for example an eventual push-back) compute the target size themselves and
 * call this with it.
 *
 * @param self         Array to grow.
 * @param min_capacity Minimum number of elements the array must be able to hold.
 */
lh_void
lh_array_reserve(lh_array_t *self, lh_usize_t min_capacity);

/**
 * @brief Compute the capacity @p self would grow to for a given minimum.
 *
 * No-op-equivalent when @p capacity already satisfies @p min_capacity (returns
 * @p capacity unchanged). Otherwise applies the array's amortized growth
 * policy: @c max(min_capacity, capacity * ::LH_LIBRARY_OPTION_ARRAY_GROWTH_FACTOR)
 * (or ::LH_LIBRARY_OPTION_ARRAY_INITIAL_CAPACITY when @p capacity is zero).
 *
 * This is a pure query — it performs no allocation. Callers can use it to
 * predict or replicate the exact capacity a future insertion would grow to,
 * without mutating a array.
 *
 * @param capacity     Current capacity.
 * @param min_capacity Minimum capacity that must be reached.
 * @return Capacity to reserve: @p capacity itself, or the grown capacity.
 */
lh_usize_t
lh_array_get_grown_capacity(lh_usize_t capacity, lh_usize_t min_capacity);

/**
 * @brief Insert @p count contiguous elements at @p index, shifting later
 *        elements right by @p count.
 *
 * The one real insertion primitive: ::lh_array_push_back_of and
 * ::lh_array_insert are both expressed in terms of this function. Grows the
 * array at most once for the whole batch when needed, via
 * ::lh_array_get_grown_capacity and ::lh_array_reserve. Passing @p index
 * equal to the current size appends.
 *
 * @param self   Array to insert into.
 * @param index  Position to insert at; must be <= ::lh_array_get_size.
 * @param values Pointer to @p count contiguous values of the array's element
 *               type (not null unless @p count is 0); their bytes are copied
 *               into the new slots.
 * @param count  Number of elements to insert.
 */
lh_void
lh_array_insert_of(lh_array_t *self, lh_uindex_t index, const lh_ptr values, lh_usize_t count);

/**
 * @brief Append @p count contiguous elements to the end of @p self.
 *
 * Equivalent to ::lh_array_insert_of at index ::lh_array_get_size.
 *
 * @param self   Array to append to.
 * @param values Pointer to @p count contiguous values of the array's element
 *               type (not null unless @p count is 0); their bytes are copied
 *               into the new slots.
 * @param count  Number of elements to append.
 * @return Index of the first appended element (::lh_array_get_size before
 *         the call). Meaningful even when @p count is `0`: the position
 *         the (empty) batch would have started at.
 */
lh_uindex_t
lh_array_push_back_of(lh_array_t *self, const lh_ptr values, lh_usize_t count);

/**
 * @brief Append @p value to the end of @p self, growing the array if needed.
 *
 * Equivalent to ::lh_array_push_back_of with a count of 1.
 *
 * @param self  Array to append to.
 * @param value Pointer to a value of the array's element type (not null);
 *              its bytes are copied into the new slot.
 * @return Index @p value was stored at (::lh_array_get_size before the call).
 */
lh_uindex_t
lh_array_push_back(lh_array_t *self, const lh_ptr value);

/**
 * @brief True when @p index addresses an element currently in use.
 *
 * Unlike the underlying typed storage (valid up to @c capacity), this checks
 * against @c size.
 *
 * @param self  Array to inspect.
 * @param index Index to validate.
 */
lh_bool_t
lh_array_is_valid_index(const lh_array_t *self, lh_uindex_t index);

/**
 * @brief Return a pointer to the element at @p index.
 *
 * @param self  Array to index.
 * @param index Element index; must be < ::lh_array_get_size.
 * @return Pointer to the element's bytes.
 */
lh_ptr
lh_array_get_ptr(const lh_array_t *self, lh_uindex_t index);

/**
 * @brief Remove the last element, optionally copying it out first.
 *
 * @param self Array to shrink; must not be empty.
 * @param dst  Optional destination for the removed element's bytes, or
 *             ::lh_null to discard it.
 */
lh_void
lh_array_pop_back(lh_array_t *self, lh_ptr dst);

/**
 * @brief Insert @p value at @p index, shifting later elements right by one.
 *
 * Equivalent to ::lh_array_insert_of with a count of 1.
 *
 * @param self  Array to insert into.
 * @param index Position to insert at; must be <= ::lh_array_get_size.
 * @param value Pointer to a value of the array's element type (not null);
 *              its bytes are copied into the new slot.
 */
lh_void
lh_array_insert(lh_array_t *self, lh_uindex_t index, const lh_ptr value);

/**
 * @brief Shrink or grow @p self so ::lh_array_get_size equals @p n.
 *
 * Growing reserves capacity if needed and leaves new slots uninitialized.
 * Shrinking drops trailing elements without releasing capacity.
 *
 * @param self Array to resize.
 * @param n    New size in elements.
 */
lh_void
lh_array_resize(lh_array_t *self, lh_usize_t n);

/**
 * @brief Replace @p self's elements with a copy of @p other.
 *
 * No-op when @p self is @p other. Both must have the same element size.
 *
 * @param self  Destination array.
 * @param other Source array.
 */
lh_void
lh_array_assign(lh_array_t *self, const lh_array_t *other);

/**
 * @brief Remove the element at @p index, shifting later elements left by one.
 *
 * @param self  Array to remove from.
 * @param index Position to remove; must be < ::lh_array_get_size.
 * @param dst   Optional destination for the removed element's bytes, or
 *              ::lh_null to discard it.
 */
lh_void
lh_array_erase(lh_array_t *self, lh_uindex_t index, lh_ptr dst);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ARRAY_H */
