/**
 * @file view.h
 * @brief Non-owning read-only wide string view (::lh_wstr_view_t) and helpers.
 *
 * ::lh_wstr_view_t is a direct typedef of ::lh_memory_view_t with @c const
 * @c lh_wchar_t endpoints. The half-open interval is <tt>[first, second)</tt>
 * in address space; ::lh_wstr_view_get_size reports ::lh_wchar_t units, not bytes.
 *
 * Because the type is layout-compatible with ::lh_memory_view_t, all
 * ::lh_memory_view functions accept an ::lh_wstr_view_t pointer without a cast.
 * This header exposes wide-string construction and typed @c lh_wchar_t access.
 *
 * @see lh_memory_view_t
 * @see lh_str_view_t
 * @see lh_wstr_view_initializer
 * @see lh_wstr_view_lit
 */

#ifndef LH_WSTR_VIEW_H
#define LH_WSTR_VIEW_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/memory/view.h>
#include <lh/size.h>
#include <lh/void.h>
#include <lh/wchar.h>
#include <lh/wstr/ptr.h>
#include <lh/wstr/view/initializer.h>

/**
 * @brief Non-owning read-only half-open wide string view.
 *
 * A direct typedef of ::lh_memory_view_t.
 * Endpoints are expected to point into @c const @c lh_wchar_t storage.
 * Public typedef: ::lh_wstr_view_t.
 */
typedef lh_memory_view_t lh_wstr_view_t;

LH_COMPILER_EXTERN_C_BEGIN

/* -- init ------------------------------------------------------------------ */

/**
 * @brief Initialize @p self as a half-open wide view starting at @p data.
 *
 * Stores a byte span covering @p size ::lh_wchar_t elements:
 * <tt>[data, data + size)</tt> in wchar units.
 *
 * @param self View to initialize.
 * @param data Pointer to the first wide character.
 * @param size Number of ::lh_wchar_t elements in the view.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p data is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed view is not valid.
 */
lh_void
lh_wstr_view_init_by_size(lh_wstr_view_t *self, lh_wstr_cptr data, lh_usize_t size);

/**
 * @brief Initialize @p self from a NUL-terminated wide string @p data.
 *
 * The view length is determined by the wide NUL terminator
 * (::lh_wstr_ptr_len).
 *
 * @param self View to initialize.
 * @param data NUL-terminated wide string to view.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p data is ::lh_null.
 */
lh_void
lh_wstr_view_init(lh_wstr_view_t *self, lh_wstr_cptr data);

/**
 * @brief Initialize @p self with the empty view initializer.
 *
 * @param self View to initialize.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_wstr_view_init_empty(lh_wstr_view_t *self);

/**
 * @brief Initialize @p self as a copy of @p other.
 *
 * @param self  View to initialize.
 * @param other Source view to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
lh_void
lh_wstr_view_init_by_other(lh_wstr_view_t *self, const lh_wstr_view_t *other);

/**
 * @brief Construct a view over a NUL-terminated wide C string @p data.
 *
 * ::lh_null or an empty string become ::lh_wstr_view_empty_initializer.
 * Otherwise the view covers the characters before the terminator
 * (same length as ::lh_wstr_ptr_len).
 *
 * @param data NUL-terminated wide string, or ::lh_null.
 * @return Constructed view (does not own @p data).
 */
lh_wstr_view_t
lh_wstr_view_make(lh_wstr_cptr data);

/* -- getters --------------------------------------------------------------- */

/**
 * @brief Return @c first as ::lh_wstr_cptr after validating @p self.
 *
 * @param self Valid view to read.
 * @return Begin pointer cast to ::lh_wstr_cptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_wstr_cptr
lh_wstr_view_get_begin(const lh_wstr_view_t *self);

/**
 * @brief Return @c second as ::lh_wstr_cptr after validating @p self.
 *
 * @c second is the exclusive end: one past the last wide character.
 *
 * @param self Valid view to read.
 * @return End pointer cast to ::lh_wstr_cptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_wstr_cptr
lh_wstr_view_get_end(const lh_wstr_view_t *self);

/**
 * @brief Return @c first as ::lh_wstr_cptr.
 * @param self View to read.
 * @return @c first cast to ::lh_wstr_cptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_wstr_cptr
lh_wstr_view_get_data(const lh_wstr_view_t *self);

/**
 * @brief Return the number of wide characters in @p self.
 *
 * Byte length of the stored span divided by ::LH_WCHAR_T_SIZE.
 * Does not count a stored @c L'\\0' that sits at @c second.
 *
 * @param self Valid view to read.
 * @return Number of ::lh_wchar_t elements covered by the half-open interval.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_usize_t
lh_wstr_view_get_size(const lh_wstr_view_t *self);

/* -- state ----------------------------------------------------------------- */

/**
 * @brief Return ::lh_bool_true if @p self covers zero wide characters.
 *
 * Uninitialized (::lh_null, ::lh_null) and zero-length views are empty.
 *
 * @param self View to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_wstr_view_is_empty(const lh_wstr_view_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WSTR_VIEW_H */
