/**
 * @file vector.h
 * @brief Growable, heap-owning typed array (::lh_vector_t).
 *
 * A vector owns a typed, heap-allocated block (::lh_memory_typed_allocated_t)
 * whose bounds always describe the full allocated @c capacity, plus a
 * @c size field tracking how many of those slots are actually in use
 * (@c size <= capacity). Growing @c size past the current capacity
 * reallocates the underlying block.
 *
 * @see lh_memory_typed_allocated_t
 */

#ifndef LH_VECTOR_H
#define LH_VECTOR_H

#include <lh/memory/typed/allocated.h>

/**
 * @struct lh_vector
 * @brief Typed, growable array backed by a heap-allocated block.
 *
 * @c typed.bounds always spans the full allocated capacity; @c size is the
 * number of elements actually in use, from the start of that capacity.
 */
typedef struct lh_vector
{
    lh_memory_typed_allocated_t typed; /**< Owns the block; bounds == capacity. */
    lh_usize_t size;                   /**< Elements in use; size <= capacity. */
} lh_vector_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return the number of elements the current allocation can hold.
 * @param self Vector to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_vector_get_capacity(const lh_vector_t *self);

/**
 * @brief Return the number of elements currently in use.
 * @param self Vector to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_vector_get_size(const lh_vector_t *self);

/**
 * @brief Return the size in bytes of one element.
 * @param self Vector to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_vector_get_type_size(const lh_vector_t *self);

/**
 * @brief Return a pointer to the first element.
 * @param self Vector to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_vector_get_begin(const lh_vector_t *self);

/**
 * @brief Alias for ::lh_vector_get_begin.
 * @param self Vector to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_vector_get_data(const lh_vector_t *self);

/**
 * @brief Return a pointer one past the last element in use.
 *
 * Unlike the underlying typed storage (whose end sits at @c capacity), this
 * reflects @c size, matching begin/end iteration idioms.
 *
 * @param self Vector to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_vector_get_end(const lh_vector_t *self);

/**
 * @brief True when @p self has no elements in use.
 * @param self Vector to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_vector_is_empty(const lh_vector_t *self);

/**
 * @brief Initialize @p self as an empty vector of elements sized @p type_size.
 *
 * No allocation happens until the vector is grown.
 *
 * @param self      Vector to initialize.
 * @param type_size Size of one element in bytes.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_vector_init(lh_vector_t *self, lh_usize_t type_size);

/**
 * @brief Ensure @p self can hold at least @p min_capacity elements.
 *
 * No-op when the current capacity is already sufficient. Otherwise reallocates
 * the underlying block to exactly @p min_capacity elements — this function
 * applies no growth strategy of its own; callers that need amortized growth
 * (for example an eventual push-back) compute the target size themselves and
 * call this with it.
 *
 * @param self         Vector to grow.
 * @param min_capacity Minimum number of elements the vector must be able to hold.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_vector_reserve(lh_vector_t *self, lh_usize_t min_capacity);

/**
 * @brief Insert @p count contiguous elements at @p index, shifting later
 *        elements right by @p count.
 *
 * The one real insertion primitive: ::lh_vector_push_back_of and
 * ::lh_vector_insert are both expressed in terms of this function. Grows the
 * vector at most once for the whole batch when needed: capacity becomes
 * @c max(size + count, capacity * ::LH_LIBRARY_OPTION_VECTOR_GROWTH_FACTOR)
 * (or ::LH_LIBRARY_OPTION_VECTOR_INITIAL_CAPACITY from empty), via ::lh_vector_reserve.
 * Passing @p index equal to the current size appends.
 *
 * @param self   Vector to insert into.
 * @param index  Position to insert at; must be <= ::lh_vector_get_size.
 * @param values Pointer to @p count contiguous values of the vector's element
 *               type (not null unless @p count is 0); their bytes are copied
 *               into the new slots.
 * @param count  Number of elements to insert.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_vector_insert_of(lh_vector_t *self, lh_uindex_t index, const lh_ptr values, lh_usize_t count);

/**
 * @brief Append @p count contiguous elements to the end of @p self.
 *
 * Equivalent to ::lh_vector_insert_of at index ::lh_vector_get_size.
 *
 * @param self   Vector to append to.
 * @param values Pointer to @p count contiguous values of the vector's element
 *               type (not null unless @p count is 0); their bytes are copied
 *               into the new slots.
 * @param count  Number of elements to append.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_vector_push_back_of(lh_vector_t *self, const lh_ptr values, lh_usize_t count);

/**
 * @brief Append @p value to the end of @p self, growing the vector if needed.
 *
 * Equivalent to ::lh_vector_push_back_of with a count of 1.
 *
 * @param self  Vector to append to.
 * @param value Pointer to a value of the vector's element type (not null);
 *              its bytes are copied into the new slot.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_vector_push_back(lh_vector_t *self, const lh_ptr value);

/**
 * @brief True when @p index addresses an element currently in use.
 *
 * Unlike the underlying typed storage (valid up to @c capacity), this checks
 * against @c size.
 *
 * @param self  Vector to inspect.
 * @param index Index to validate.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_vector_is_valid_index(const lh_vector_t *self, lh_uindex_t index);

/**
 * @brief Return a pointer to the element at @p index.
 *
 * @param self  Vector to index.
 * @param index Element index; must be < ::lh_vector_get_size.
 * @return Pointer to the element's bytes.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_vector_get_ptr(const lh_vector_t *self, lh_uindex_t index);

/**
 * @brief Remove the last element, optionally copying it out first.
 *
 * @param self Vector to shrink; must not be empty.
 * @param dst  Optional destination for the removed element's bytes, or
 *             ::lh_null to discard it.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_vector_pop_back(lh_vector_t *self, lh_ptr dst);

/**
 * @brief Insert @p value at @p index, shifting later elements right by one.
 *
 * Equivalent to ::lh_vector_insert_of with a count of 1.
 *
 * @param self  Vector to insert into.
 * @param index Position to insert at; must be <= ::lh_vector_get_size.
 * @param value Pointer to a value of the vector's element type (not null);
 *              its bytes are copied into the new slot.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_vector_insert(lh_vector_t *self, lh_uindex_t index, const lh_ptr value);

/**
 * @brief Remove the element at @p index, shifting later elements left by one.
 *
 * @param self  Vector to remove from.
 * @param index Position to remove; must be < ::lh_vector_get_size.
 * @param dst   Optional destination for the removed element's bytes, or
 *              ::lh_null to discard it.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_vector_erase(lh_vector_t *self, lh_uindex_t index, lh_ptr dst);

LH_COMPILER_EXTERN_C_END

#endif /* LH_VECTOR_H */
