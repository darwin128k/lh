#ifndef LH_MEMORY_TYPED_H
#define LH_MEMORY_TYPED_H

#include <lh/bool.h>
#include <lh/memory/range.h>
#include <lh/memory/typed/fields.h>

typedef struct lh_memory_typed {
    lh_memory_typed_fields(lh_memory_range_t);
} lh_memory_typed_t;

LH_COMPILER(EXTERN_C_BEGIN)

/**
 * @brief Return mutable pointer to the underlying range.
 *
 * @param self Typed span to access.
 * @return Pointer to the @c range field.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_range_t *lh_memory_typed_get_range(lh_memory_typed_t *self);

/**
 * @brief Return const pointer to the underlying range.
 *
 * @param self Typed span to access.
 * @return Const pointer to the @c range field.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_memory_range_t *lh_memory_typed_get_crange(const lh_memory_typed_t *self);

/**
 * @brief Return size of one element in bytes.
 *
 * @param self Typed span to inspect.
 * @return Value stored in @c type_size.
 */
LH_ATTRIBUTE(SYMBOL)
lh_usize_t lh_memory_typed_get_type_size(const lh_memory_typed_t *self);

/**
 * @brief Return @c first — start of the half-open span.
 *
 * @param self Typed span to inspect.
 * @return Begin pointer.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_get_begin(const lh_memory_typed_t *self);

/**
 * @brief Return @c second — one past the last byte.
 *
 * @param self Typed span to inspect.
 * @return End pointer.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_get_end(const lh_memory_typed_t *self);

/**
 * @brief Return number of elements in the typed span.
 *
 * Computes @c range.get_size() / @p type_size.
 * Fails with ::lh_runtime_error_code_size_not_multiple_of_type_size
 * if range size is not evenly divisible by type_size.
 *
 * @param self Typed span to inspect.
 * @return Number of elements.
 */
LH_ATTRIBUTE(SYMBOL)
lh_usize_t lh_memory_typed_get_size(const lh_memory_typed_t *self);

/**
 * @brief True iff the typed span contains no elements.
 *
 * @param self Typed span to inspect.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_typed_is_empty(const lh_memory_typed_t *self);

/**
 * @brief True iff @p index is a valid element offset.
 *
 * @param self Typed span to inspect.
 * @param index Element index to test (0-based).
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Address of element @p index from the front.
 *
 * @param self  Typed span to access.
 * @param index Element index (0-based).
 * @return Pointer to the element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_at_ptr_from_front(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Address of element @p index from the back.
 *
 * @param self  Typed span to access.
 * @param index Element index from the end (0-based).
 * @return Pointer to the element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_at_ptr_from_back(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Dispatch to front or back pointer access.
 *
 * @param self     Typed span to access.
 * @param index   Element index.
 * @param from_back If true, count from the end.
 * @return Pointer to the element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_at_ptr(const lh_memory_typed_t *self, lh_usize_t index,
                               lh_bool_t from_back);

/**
 * @brief Byte value at element @p index from the front.
 *
 * @param self  Typed span to read.
 * @param index Element index (0-based).
 * @return Byte value at the element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_at_from_front(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Byte value at element @p index from the back.
 *
 * @param self  Typed span to read.
 * @param index Element index from the end (0-based).
 * @return Byte value at the element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_at_from_back(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Dispatch to front or back value access.
 *
 * @param self      Typed span to read.
 * @param index    Element index.
 * @param from_back If true, count from the end.
 * @return Byte value at the element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_at(const lh_memory_typed_t *self, lh_usize_t index,
                              lh_bool_t from_back);

/**
 * @brief Pointer to the first element.
 *
 * @param self Typed span to access.
 * @return Pointer to element 0.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_get_front_ptr(const lh_memory_typed_t *self);

/**
 * @brief Pointer to the last element.
 *
 * @param self Typed span to access.
 * @return Pointer to last element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_get_back_ptr(const lh_memory_typed_t *self);

/**
 * @brief Byte value of the first element.
 *
 * @param self Typed span to read.
 * @return Byte at element 0.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_get_front(const lh_memory_typed_t *self);

/**
 * @brief Byte value of the last element.
 *
 * @param self Typed span to read.
 * @return Byte at last element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_get_back(const lh_memory_typed_t *self);

LH_COMPILER(EXTERN_C_END)

#endif // LH_MEMORY_TYPED_H
