#ifndef LH_MEMORY_TYPED_H
#define LH_MEMORY_TYPED_H

#include <lh/memory/typed/fields.h>
#include <lh/compiler/extern/c.h>
#include <lh/attribute/symbol.h>
#include <lh/memory/bounds.h>

typedef struct lh_memory_typed
{
    lh_memory_typed_fields(lh_memory_bounds_t);
} lh_memory_typed_t;

LH_COMPILER_EXTERN_C_BEGIN

/* -- unpack / getters ------------------------------------------------------ */

/**
 * @brief Read @c begin, @c end, and @c type_size from @p self into optional outputs.
 *
 * Pass ::lh_null for any output to skip it.
 *
 * @param self      Typed memory to read.
 * @param begin     Output for the begin pointer, or ::lh_null.
 * @param end       Output for the exclusive end pointer, or ::lh_null.
 * @param type_size Output for the element size in bytes, or ::lh_null.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_unpack(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                       lh_usize_t *type_size);

/**
 * @brief Unpack @p self into @p other.
 * @param self  Source typed memory (not null).
 * @param other Destination typed memory (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_unpack_to_other(const lh_memory_typed_t *self, lh_memory_typed_t *other);

/**
 * @brief Return the begin pointer without validating the range.
 * @param self Typed memory to read.
 * @return Stored begin pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_begin(const lh_memory_typed_t *self);

/**
 * @brief Return the exclusive end pointer without validating the range.
 * @param self Typed memory to read.
 * @return Stored exclusive end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_end(const lh_memory_typed_t *self);

/**
 * @brief Return the element size in bytes without validating the range.
 * @param self Typed memory to read.
 * @return Element size in bytes.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_type_size(const lh_memory_typed_t *self);

/* -- classification -------------------------------------------------------- */

/**
 * @brief True if both endpoints are ::lh_null.
 * @param self Typed memory to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_uninitialized(const lh_memory_typed_t *self);

/**
 * @brief True if both endpoints are non-null.
 * @param self Typed memory to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_initialized(const lh_memory_typed_t *self);

/**
 * @brief True if the byte size of @p self is divisible by @p alignment.
 * @param self      Typed memory to inspect.
 * @param alignment Non-zero divisor.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_multiple_of(const lh_memory_typed_t *self, lh_usize_t alignment);

/**
 * @brief True if @p self has valid bounds and its byte size is divisible by @c type_size.
 * @param self Typed memory to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_valid(const lh_memory_typed_t *self);

/* -- validated getters ----------------------------------------------------- */

/**
 * @brief Like ::lh_memory_typed_unpack but requires valid typed memory.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_unpack_v(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                         lh_usize_t *type_size);

/**
 * @brief Return the begin pointer after validating @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_begin_v(const lh_memory_typed_t *self);

/**
 * @brief Return the exclusive end pointer after validating @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_end_v(const lh_memory_typed_t *self);

/**
 * @brief Return the element size in bytes after validating @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_type_size_v(const lh_memory_typed_t *self);

/* -- size ------------------------------------------------------------------ */

/**
 * @brief Return the total byte size of @p self.
 * @param self Valid typed memory.
 * @return Number of bytes covered by the half-open interval.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_size_of_bytes(const lh_memory_typed_t *self);

/**
 * @brief Return the number of elements in @p self.
 * @param self Valid typed memory.
 * @return Element count (byte size divided by @c type_size).
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_size(const lh_memory_typed_t *self);

/**
 * @brief True if @p self is uninitialized or has zero elements.
 * @param self Typed memory to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_empty(const lh_memory_typed_t *self);

/**
 * @brief True if @p index addresses a valid element inside @p self.
 * @param self  Valid typed memory to inspect.
 * @param index Zero-based element index to test.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_usize_t index);

/* -- pointer access -------------------------------------------------------- */

/**
 * @brief Return pointer to the element at @p index from the beginning.
 * @param self  Valid typed memory to index.
 * @param index Zero-based element index from the first element.
 * @return Pointer to the start of the element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p index is out of range.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_ptr_from_begin(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Return pointer to the element at @p index from the end.
 * @param self  Valid typed memory to index.
 * @param index Zero-based element index from the last element.
 * @return Pointer to the start of the element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p index is out of range.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_ptr_from_end(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Return pointer to the element at signed @p index.
 *
 * Non-negative @p index counts from the first element; negative counts from
 * the last element (@c -1 is the last, @c -2 the second-to-last, etc.).
 *
 * @param self  Valid typed memory to index.
 * @param index Signed element index.
 * @return Pointer to the start of the element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p index is out of range.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_ptr(const lh_memory_typed_t *self, lh_ssize_t index);

/**
 * @brief Return pointer to the first element.
 * @param self Valid typed memory to index.
 * @return Pointer to the first element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_first_ptr(const lh_memory_typed_t *self);

/**
 * @brief Return pointer to the last element.
 * @param self Valid non-empty typed memory to index.
 * @return Pointer to the last element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p self is empty.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_last_ptr(const lh_memory_typed_t *self);

/* -- value access ---------------------------------------------------------- */

/**
 * @brief Read the first byte of the element at @p index from the beginning.
 * @param self  Valid typed memory to index.
 * @param index Zero-based element index from the first element.
 * @return Byte at the start of the element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p index is out of range.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_typed_get_value_from_begin(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Read the first byte of the element at @p index from the end.
 * @param self  Valid typed memory to index.
 * @param index Zero-based element index from the last element.
 * @return Byte at the start of the element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p index is out of range.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_typed_get_value_from_end(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Read the first byte of the element at signed @p index.
 *
 * Non-negative @p index counts from the first element; negative counts from
 * the last element (@c -1 is the last, @c -2 the second-to-last, etc.).
 *
 * @param self  Valid typed memory to index.
 * @param index Signed element index.
 * @return Byte at the start of the element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p index is out of range.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_typed_get_value(const lh_memory_typed_t *self, lh_ssize_t index);

/**
 * @brief Read the first byte of the first element.
 * @param self Valid non-empty typed memory.
 * @return Byte at the start of the first element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_typed_get_first_value(const lh_memory_typed_t *self);

/**
 * @brief Read the first byte of the last element.
 * @param self Valid non-empty typed memory.
 * @return Byte at the start of the last element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p self is empty.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_typed_get_last_value(const lh_memory_typed_t *self);

/* -- mutation -------------------------------------------------------------- */

/**
 * @brief Set typed memory bounds while keeping the current element size.
 * @param self   Typed memory object to update.
 * @param bounds New memory bounds.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p bounds is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_set_by_bounds(lh_memory_typed_t *self, const lh_memory_bounds_t *bounds);

/**
 * @brief Initialize typed memory from existing byte bounds and element size.
 * @param self      Typed memory object to initialize.
 * @param bounds    Source memory bounds.
 * @param type_size Size of one element in bytes.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p bounds is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_init_by_bounds(lh_memory_typed_t *self, const lh_memory_bounds_t *bounds,
                               lh_usize_t type_size);

/**
 * @brief Create typed memory from existing byte bounds and element size.
 * @param bounds    Source memory bounds.
 * @param type_size Size of one element in bytes.
 * @return Typed memory with copied bounds and @p type_size.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p bounds is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_typed_t
lh_memory_typed_make_by_bounds(const lh_memory_bounds_t *bounds, lh_usize_t type_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_TYPED_H */
