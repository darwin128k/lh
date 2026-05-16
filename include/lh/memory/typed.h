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

/**
 * @brief Return pointer to the first element.
 * @param self Valid typed memory to index.
 * @return Pointer to the first element.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
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
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p self is empty.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_last_ptr(const lh_memory_typed_t *self);

/**
 * @brief Create typed memory from existing byte bounds and element size.
 * @param bounds Source memory bounds.
 * @param type_size Size of one element in bytes.
 * @return Typed memory with copied bounds and @p type_size.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p bounds is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_typed_t
lh_memory_typed_make_by_bounds(const lh_memory_bounds_t *bounds, lh_usize_t type_size);

/**
 * @brief Set typed memory bounds while keeping the current element size.
 * @param self Typed memory object to update.
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
 * @param self Typed memory object to initialize.
 * @param bounds Source memory bounds.
 * @param type_size Size of one element in bytes.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p bounds is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_init_by_bounds(lh_memory_typed_t *self, const lh_memory_bounds_t *bounds,
                               lh_usize_t type_size);

LH_COMPILER_EXTERN_C_END

#endif // LH_MEMORY_TYPED_H
