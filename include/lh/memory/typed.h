/**
 * @file typed.h
 * @brief Typed half-open byte bounds (::lh_memory_typed_t) and helpers.
 *
 * A typed memory object wraps ::lh_memory_bounds_t
 * with a @c type_size field that records the size of one element in bytes.
 * The underlying byte span is interpreted as a contiguous sequence of fixed-size elements.
 *
 * Valid typed memory satisfies three conditions:
 *  - the embedded @c bounds are valid (forward-ordered, both endpoints set);
 *  - @c type_size is non-zero;
 *  - the byte span size is divisible by @c type_size.
 *
 * Functions with the @c _v suffix require valid typed memory
 * and raise runtime errors when the precondition is not met.
 * Element-indexed access counts in elements, not bytes.
 *
 * @see lh_memory_bounds_t
 * @see lh_memory_typed_fields
 */

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
 * @brief Read @c bounds and @c type_size from @p self into optional outputs.
 *
 * Pass ::lh_null for any output to skip it.
 *
 * @param self      Typed memory to read.
 * @param begin     Output for @c bounds.first, or ::lh_null.
 * @param end       Output for @c bounds.second, or ::lh_null.
 * @param type_size Output for @c type_size, or ::lh_null.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_unpack(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                       lh_usize_t *type_size);

/**
 * @brief Return a read-only pointer to the embedded @c bounds.
 *
 * @param self Typed memory to read.
 * @return Pointer to the embedded ::lh_memory_bounds_t.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
const lh_memory_bounds_t *
lh_memory_typed_get_bounds(const lh_memory_typed_t *self);

/**
 * @brief Return @c bounds.first without validating the range.
 *
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
 * @brief Return @c bounds.second without validating the range.
 *
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
 * @brief Return @c type_size without validating the range.
 *
 * @param self Typed memory to read.
 * @return Stored element size in bytes.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_type_size(const lh_memory_typed_t *self);

/* -- classification -------------------------------------------------------- */

/**
 * @brief True iff the embedded @c bounds endpoints are both null.
 *
 * @param self Typed memory to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_uninitialized(const lh_memory_typed_t *self);

/**
 * @brief True iff both @c bounds endpoints are set.
 *
 * Does not check @c type_size or ordering.
 *
 * @param self Typed memory to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_initialized(const lh_memory_typed_t *self);

/**
 * @brief True iff @p self satisfies all validity conditions.
 *
 * Valid typed memory has forward-ordered bounds, a non-zero @c type_size, and
 * a byte span evenly divisible by @c type_size.
 *
 * @param self Typed memory to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_valid(const lh_memory_typed_t *self);

/**
 * @brief Logical negation of ::lh_memory_typed_is_valid.
 *
 * @param self Typed memory to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_invalid(const lh_memory_typed_t *self);

/* -- validated access, size ----------------------------------------------- */

/**
 * @brief Like ::lh_memory_typed_unpack but requires valid typed memory.
 *
 * @param self      Valid typed memory to read.
 * @param begin     Output for @c bounds.first, or ::lh_null.
 * @param end       Output for @c bounds.second, or ::lh_null.
 * @param type_size Output for @c type_size, or ::lh_null.
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
 * @brief Return @c bounds.first after validating @p self.
 *
 * @param self Valid typed memory to read.
 * @return Stored begin pointer.
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
 * @brief Return @c bounds.second after validating @p self.
 *
 * @param self Valid typed memory to read.
 * @return Stored exclusive end pointer.
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
 * @brief Return the total byte span of valid @p self.
 *
 * @param self Valid typed memory to inspect.
 * @return Number of bytes in the half-open interval.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_byte_size(const lh_memory_typed_t *self);

/**
 * @brief Return the number of elements in valid @p self.
 *
 * Divides the byte span by @c type_size.
 *
 * @param self Valid typed memory to inspect.
 * @return Number of elements.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self bounds are not valid.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @c type_size is zero.
 * @throw ::lh_runtime_error_code_size_not_multiple_of_type_size
 *        Byte span is not divisible by @c type_size.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_count(const lh_memory_typed_t *self);

/**
 * @brief True iff @p self is uninitialized or contains zero elements.
 *
 * @param self Typed memory to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is invalid and not uninitialized.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_empty(const lh_memory_typed_t *self);

/**
 * @brief True iff @p index addresses a valid element inside @p self.
 *
 * Valid indices are in <tt>[0, lh_memory_typed_get_count(self))</tt>.
 *
 * @param self  Valid typed memory to inspect.
 * @param index Element index to test.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_usize_t index);

/* -- element access -------------------------------------------------------- */

/**
 * @brief Return pointer to element at @p index from the beginning.
 *
 * Index @c 0 addresses the first element.
 *
 * @param self  Valid typed memory to index.
 * @param index Element index from the begin endpoint.
 * @return Pointer to the start of the requested element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p index is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_element_ptr_from_begin(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Return pointer to element at @p index from the end.
 *
 * Index @c 0 addresses the last element.
 *
 * @param self  Valid typed memory to index.
 * @param index Element index from the last element.
 * @return Pointer to the start of the requested element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p index is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_element_ptr_from_end(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Return pointer to the first element of valid @p self.
 *
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
lh_memory_typed_get_first_element_ptr(const lh_memory_typed_t *self);

/**
 * @brief Return pointer to the last element of valid @p self.
 *
 * @param self Valid typed memory to index.
 * @return Pointer to the last element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_last_element_ptr(const lh_memory_typed_t *self);

/* -- mutation -------------------------------------------------------------- */

/**
 * @brief Reset @p self to the fully uninitialized state.
 *
 * Sets both bound endpoints to null and @c type_size to zero.
 *
 * @param self Typed memory to clear.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_clear(lh_memory_typed_t *self);

/**
 * @brief Copy all fields from valid @p other into @p self.
 *
 * @param self  Typed memory to update.
 * @param other Valid typed memory to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_assign_v(lh_memory_typed_t *self, const lh_memory_typed_t *other);

/**
 * @brief Store @p begin, @p end, and @p type_size after full validation.
 *
 * @param self      Typed memory to update.
 * @param begin     New @c bounds.first endpoint.
 * @param end       New exclusive @c bounds.second endpoint.
 * @param type_size New element size in bytes.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid half-open range.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p type_size is zero.
 * @throw ::lh_runtime_error_code_size_not_multiple_of_type_size
 *        Byte span is not divisible by @p type_size.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_set_v(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size);

/**
 * @brief Store typed bounds starting at @p begin covering @p count elements.
 *
 * Computes the end endpoint as @c begin + @c count * @p type_size.
 *
 * @param self      Typed memory to update.
 * @param begin     Begin endpoint.
 * @param count     Number of elements.
 * @param type_size Element size in bytes.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null or @p type_size is zero.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p count is zero or the computed bounds are not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_set_by_count(lh_memory_typed_t *self, lh_ptr begin, lh_usize_t count,
                             lh_usize_t type_size);

/**
 * @brief Swap two valid typed memory objects.
 *
 * @param self  Valid typed memory.
 * @param other Valid typed memory.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_swap_v(lh_memory_typed_t *self, lh_memory_typed_t *other);

/* -- initialization -------------------------------------------------------- */

/**
 * @brief Initialize @p self with validated @p begin, @p end, and @p type_size.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not valid.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p type_size is zero.
 * @throw ::lh_runtime_error_code_size_not_multiple_of_type_size
 *        Byte span is not divisible by @p type_size.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_init(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size);

/**
 * @brief Initialize @p self with @p count elements of @p type_size bytes each.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null or @p type_size is zero.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p count is zero or the computed bounds are not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_init_by_count(lh_memory_typed_t *self, lh_ptr begin, lh_usize_t count,
                              lh_usize_t type_size);

/**
 * @brief Initialize @p self with null bounds and the given @p type_size.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_init_empty(lh_memory_typed_t *self, lh_usize_t type_size);

/**
 * @brief Initialize @p self by copying all fields from valid @p other.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_init_by_other(lh_memory_typed_t *self, const lh_memory_typed_t *other);

/* -- factory --------------------------------------------------------------- */

/**
 * @brief Build and validate typed memory from @p begin, @p end, @p type_size.
 *
 * @param begin     Start of the half-open span.
 * @param end       One past the last byte.
 * @param type_size Element size in bytes.
 * @return Constructed valid typed memory value.
 *
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not valid or not divisible by @p type_size.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p type_size is zero.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_typed_t
lh_memory_typed_make_v(lh_ptr begin, lh_ptr end, lh_usize_t type_size);

/**
 * @brief Build typed memory from @p begin covering @p count elements.
 *
 * @param begin     Begin endpoint.
 * @param count     Number of elements.
 * @param type_size Element size in bytes.
 * @return Constructed valid typed memory value.
 *
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null or @p type_size is zero.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p count is zero or the computed bounds are not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_typed_t
lh_memory_typed_make_by_count(lh_ptr begin, lh_usize_t count, lh_usize_t type_size);

/**
 * @brief Return typed memory with null bounds and @p type_size set.
 *
 * @param type_size Element size in bytes.
 * @return Uninitialized typed memory with the given @c type_size.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_typed_t
lh_memory_typed_make_empty(lh_usize_t type_size);

/**
 * @brief Build typed memory by copying all fields from valid @p other.
 *
 * @param other Valid typed memory to copy.
 * @return Copy of @p other.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_typed_t
lh_memory_typed_make_by_other(const lh_memory_typed_t *other);

LH_COMPILER_EXTERN_C_END

#endif // LH_MEMORY_TYPED_H
