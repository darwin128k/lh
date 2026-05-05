/**
 * @file typed.h
 * @brief Typed memory span type (::lh_memory_typed_t) and helpers.
 *
 * A typed span wraps a ::lh_memory_range_t and adds a @c type_size field denoting
 * the size of each element in bytes. This allows treating the raw memory as an
 * array of elements of a fixed size rather than as a byte buffer.
 *
 * The number of elements is computed as @c range.get_size() / @p type_size.
 * Typed validity is defined by divisibility of byte size by @p type_size.
 * Since divisibility is computed via range size, queries may raise
 * ::lh_runtime_error_code_invalid_memory_range when the underlying range is invalid.
 *
 * For the const-qualified counterpart, see ::lh_memory_view_t.
 *
 * @see lh_memory_range_t
 * @see lh_memory_view_t
 */

#ifndef LH_MEMORY_TYPED_H
#define LH_MEMORY_TYPED_H

#include <lh/memory/range.h>
#include <lh/memory/typed/fields.h>

/**
 * @struct lh_memory_typed
 * @brief Non-owning typed memory span: wraps a ::lh_memory_range_t with element size.
 *
 * Stores a range and a @c type_size field describing the size of each element
 * in bytes. Typed invariants require range byte size to be divisible by @c type_size.
 * Public typedef: ::lh_memory_typed_t.
 *
 * @attention This struct is @b not validated on construction.
 *           Use lh_memory_typed_make_v() to create a validated instance.
 *
 * @see lh_memory_range_t
 * @see lh_memory_typed_make_v
 */
typedef struct lh_memory_typed {
    lh_memory_typed_fields(lh_memory_range_t);
} lh_memory_typed_t;

LH_COMPILER(EXTERN_C_BEGIN)

/* ── pack / unpack ────────────────────────────────────────────────────────── */

/**
 * @brief Return mutable pointer to the underlying byte range.
 *
 * @param self Typed span to access.
 * @return Pointer to the @c range field.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_range_t *lh_memory_typed_get_range(lh_memory_typed_t *self);

/**
 * @brief Return const pointer to the underlying byte range.
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
 * @brief Pack range and type size into the typed span.
 *
 * @param self Typed span to initialize.
 * @param begin Start address of the memory range.
 * @param end End address of the memory range (one-past-last).
 * @param type_size Optional pointer to element size in bytes. If NULL, type_size is unchanged.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_pack(lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                             const lh_usize_t *type_size);

/**
 * @brief Pack typed span fields from a range object plus optional type size.
 *
 * @param self Typed span to update.
 * @param range Source range for begin/end fields.
 * @param type_size Optional new element size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_pack_from_range(lh_memory_typed_t *self, lh_memory_range_t *range,
                                        const lh_usize_t *type_size);

/**
 * @brief Validated variant of ::lh_memory_typed_pack.
 *
 * Applies optional field updates and commits only when resulting typed state
 * satisfies ::lh_memory_typed_is_valid.
 *
 * @param self Typed span to update.
 * @param begin Optional new begin pointer.
 * @param end Optional new end pointer.
 * @param type_size Optional new element size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_pack_v(lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                               const lh_usize_t *type_size);

/**
 * @brief Unpack range and type size from the typed span.
 *
 * @param self Typed span to inspect.
 * @param begin Optional pointer to receive start address.
 * @param end Optional pointer to receive end address (one-past-last).
 * @param type_size Optional pointer to receive element size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_unpack(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                               lh_usize_t *type_size);

/**
 * @brief Validated variant of ::lh_memory_typed_unpack.
 *
 * @param self Typed span to inspect.
 * @param begin Optional pointer to receive start address.
 * @param end Optional pointer to receive end address (one-past-last).
 * @param type_size Optional pointer to receive element size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_unpack_v(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                                 lh_usize_t *type_size);

/**
 * @brief Unpack into a range object plus optional type size output.
 *
 * @param self Typed span to inspect.
 * @param range Output range receiving @c first and @c second.
 * @param type_size Optional pointer to receive element size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_unpack_to_range(const lh_memory_typed_t *self, lh_memory_range_t *range,
                                        lh_usize_t *type_size);

/* ── set / init / assign ──────────────────────────────────────────────────── */

/**
 * @brief Set range and type size for the typed span.
 *
 * @param self Typed span to modify.
 * @param begin Start address of the memory range.
 * @param end End address of the memory range (one-past-last).
 * @param type_size Element size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_set(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end,
                            lh_usize_t type_size);

/**
 * @brief Initialize typed span with range and type size.
 *
 * @param self Typed span to initialize.
 * @param begin Start address of the memory range.
 * @param end End address of the memory range (one-past-last).
 * @param type_size Element size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_init(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end,
                             lh_usize_t type_size);

/**
 * @brief Assign contents of one typed span to another.
 *
 * @param self Typed span to modify.
 * @param other Typed span to copy from.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_assign(lh_memory_typed_t *self, const lh_memory_typed_t *other);

/**
 * @brief Set range and type size using begin pointer and size.
 *
 * @param self Typed span to modify.
 * @param begin Start address of the memory range.
 * @param size Size in bytes.
 * @param type_size Element size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_set_by_size(lh_memory_typed_t *self, lh_ptr begin, lh_usize_t size,
                                    lh_usize_t type_size);

/**
 * @brief Initialize typed span using begin pointer and size.
 *
 * @param self Typed span to initialize.
 * @param begin Start address of the memory range.
 * @param size Size in bytes.
 * @param type_size Element size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_init_by_size(lh_memory_typed_t *self, lh_ptr begin, lh_usize_t size,
                                     lh_usize_t type_size);

/**
 * @brief Initialize @p self to empty range with explicit element size.
 * @param self Destination typed span.
 * @param type_size Element size to store in the empty typed span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_init_by_empty(lh_memory_typed_t *self, lh_usize_t type_size);

/* ── clone / dup ──────────────────────────────────────────────────────────── */

/**
 * @brief Clone typed span without explicit validation step.
 *
 * @param self Typed span to clone.
 * @return Shallow copy of @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_typed_t lh_memory_typed_clone(const lh_memory_typed_t *self);

/**
 * @brief Validated assignment from @p other into @p self.
 *
 * @param self Destination typed span.
 * @param other Source typed span expected to satisfy typed validity.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_assign_v(lh_memory_typed_t *self, const lh_memory_typed_t *other);

/**
 * @brief Duplicate @p self into @p other.
 *
 * @param self Source typed span.
 * @param other Destination typed span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_dup(const lh_memory_typed_t *self, lh_memory_typed_t *other);

/**
 * @brief Validated duplicate @p self into @p other.
 *
 * @param self Source typed span.
 * @param other Destination typed span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_typed_dup_v(const lh_memory_typed_t *self, lh_memory_typed_t *other);

/**
 * @brief Clone typed span with validation.
 *
 * @param self Typed span to clone.
 * @return Validated copy of @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_typed_t lh_memory_typed_clone_v(const lh_memory_typed_t *self);

/**
 * @brief Build typed span from explicit bounds and element size.
 *
 * @param begin Start address of the memory range.
 * @param end End address of the memory range (one-past-last).
 * @param type_size Element size in bytes.
 * @return Constructed typed span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_typed_t lh_memory_typed_make(lh_ptr begin, lh_ptr end, lh_usize_t type_size);

/**
 * @brief Validated variant of ::lh_memory_typed_make.
 *
 * @param begin Start address of the memory range.
 * @param end End address of the memory range (one-past-last).
 * @param type_size Element size in bytes.
 * @return Constructed typed span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_typed_t lh_memory_typed_make_v(lh_ptr begin, lh_ptr end, lh_usize_t type_size);

/**
 * @brief Return an empty typed span with explicit element size.
 * @return Empty typed span value.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_typed_t lh_memory_typed_make_by_empty(lh_usize_t type_size);

/**
 * @brief Build typed span from explicit bounds and element size, or empty on failure.
 *
 * @param begin Start address of the memory range.
 * @param end End address of the memory range (one-past-last).
 * @param type_size Element size in bytes.
 * @return Constructed typed span or empty typed span on failure.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_typed_t lh_memory_typed_make_or_empty(lh_ptr begin, lh_ptr end, lh_usize_t type_size);

/* ── classification / geometry ────────────────────────────────────────────── */

/**
 * @brief Check typed divisibility invariant for this span.
 *
 * @param self Typed span to check.
 * @return True iff @c range.get_size() is divisible by @c type_size.
 *
 * @note This function computes range size and therefore may raise
 *       ::lh_runtime_error_code_invalid_memory_range when @p self->range is invalid.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_typed_is_valid(const lh_memory_typed_t *self);

/**
 * @brief Logical negation of ::lh_memory_typed_is_valid.
 *
 * @param self Typed span to check.
 * @return True if invalid, false otherwise.
 *
 * @note Inherits failure behavior of ::lh_memory_typed_is_valid for invalid ranges.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_typed_is_invalid(const lh_memory_typed_t *self);

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
 * Fails with ::lh_runtime_error_code_size_not_multiple_of_type_size if
 * range size is not evenly divisible by @p type_size.
 * May fail with ::lh_runtime_error_code_invalid_memory_range when the stored
 * range is invalid.
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
 * @brief True iff @p self can produce a typed slice for (@p offset, @p size).
 *
 * @param self   Source typed span.
 * @param offset Start element offset from the front.
 * @param size   Slice size in elements.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_typed_is_sliceable(const lh_memory_typed_t *self, lh_usize_t offset,
                                       lh_usize_t size);

/**
 * @brief Return a typed subspan covering elements <tt>[offset, offset + size)</tt>.
 *
 * Fails runtime check with ::lh_runtime_error_code_out_of_range if requested
 * element window does not fit in @p self.
 *
 * @param self   Source typed span.
 * @param offset Start element offset from the front.
 * @param size   Slice size in elements.
 * @return Typed subspan with the same @c type_size as @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_typed_t lh_memory_typed_slice(const lh_memory_typed_t *self, lh_usize_t offset,
                                        lh_usize_t size);

/**
 * @brief Like ::lh_memory_typed_slice, but returns empty typed span on failure.
 *
 * @param self   Source typed span.
 * @param offset Start element offset from the front.
 * @param size   Slice size in elements.
 * @return Typed subspan or empty typed span on failure.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_typed_t lh_memory_typed_slice_or_empty(const lh_memory_typed_t *self, lh_usize_t offset,
                                                 lh_usize_t size);

/* ── element access ───────────────────────────────────────────────────────── */

/**
 * @brief Address of element @p index from the front.
 *
 * @param self  Typed span to access.
 * @param index Element index (0-based).
 * @return Pointer to the element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_get_ptr_from_front(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Address of element @p index from the back.
 *
 * @param self  Typed span to access.
 * @param index Element index from the end (0-based).
 * @return Pointer to the element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_get_ptr_from_back(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Dispatch to front or back pointer access.
 *
 * @param self     Typed span to access.
 * @param index   Element index.
 * @param from_back If true, count from the end.
 * @return Pointer to the element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_typed_get_ptr(const lh_memory_typed_t *self, lh_usize_t index,
                               lh_bool_t from_back);

/**
 * @brief First byte of element @p index from the front.
 *
 * @param self  Typed span to read.
 * @param index Element index (0-based).
 * @return First byte located at the element address.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_get_value_from_front(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief First byte of element @p index from the back.
 *
 * @param self  Typed span to read.
 * @param index Element index from the end (0-based).
 * @return First byte located at the element address.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_get_value_from_back(const lh_memory_typed_t *self, lh_usize_t index);

/**
 * @brief Dispatch to front or back first-byte access.
 *
 * @param self      Typed span to read.
 * @param index     Element index.
 * @param from_back If true, count from the end.
 * @return First byte located at the selected element address.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_get_value(const lh_memory_typed_t *self, lh_usize_t index,
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
 * @brief First byte of the first element.
 *
 * @param self Typed span to read.
 * @return First byte at element 0.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_get_front_value(const lh_memory_typed_t *self);

/**
 * @brief First byte of the last element.
 *
 * @param self Typed span to read.
 * @return First byte at last element.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_typed_get_back_value(const lh_memory_typed_t *self);

LH_COMPILER(EXTERN_C_END)

#endif // LH_MEMORY_TYPED_H
