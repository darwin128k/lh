/**
 * @file exception.h
 * @brief Value thrown through the `lh` jump-based exception path.
 *
 * An ::lh_exception_t is the payload moved when you “throw”:
 * always an ::lh_error_t (`code`, message, etc.).
 *
 * When **`NDEBUG` is not defined**,
 * the struct also stores ::lh_exception_origin_t
 * so you can log **where** the throw was raised.
 *
 * Defining `NDEBUG` removes `origin` entirely
 * for smaller structs and no extra string pointers in release builds.
 *
 * Catch sites hold a copy in ::lh_exception_catch_t;
 * the runtime stack in `lh/runtime/exception/catch/stack.h`
 * copies into `exception` on throw.
 *
 * Construction helpers: `lh/exception/initializer.h`.
 */

#ifndef LH_EXCEPTION_H
#define LH_EXCEPTION_H

#include <lh/attribute/symbol.h>
#include <lh/error.h>

#ifndef NDEBUG
#    include <lh/exception/origin.h>
#endif // NDEBUG

/**
 * @struct lh_exception
 * @brief Error value plus optional debug-only provenance.
 *
 * @note Layout depends on `NDEBUG`: the `origin` member exists only in debug builds.
 *       Portable code should not rely on `sizeof(lh_exception_t)`
 *       or binary layout across build modes.
 *
 * @see lh_exception_catch_t
 * @see lh_exception_initializer()
 * @see lh_exception_empty_initializer()
 */
typedef struct lh_exception
{
    lh_error_t error; /**< Primary payload (code and related error fields). */
#ifndef NDEBUG
    lh_exception_origin_t origin; /**< Raise site metadata; absent when `NDEBUG` is defined. */
#endif
} lh_exception_t; /**< Typedef for struct ::lh_exception. */

LH_COMPILER_EXTERN_C_BEGIN

/* ── set / init ──────────────────────────────────────────────────────────── */

/**
 * @brief Replace all fields of @p self from individual error and origin fields.
 *
 * @param self      Exception to modify (not null).
 * @param code      Error code.
 * @param desc      Error description (may be null).
 * @param timestamp Build timestamp (typically @c __TIMESTAMP__; debug builds only).
 * @param file      Source file path (typically @c __FILE__; debug builds only).
 * @param function  Function name (typically @c __FUNCTION__; debug builds only).
 * @param line      Line number (typically @c __LINE__; debug builds only).
 */
LH_ATTRIBUTE_SYMBOL
#ifndef NDEBUG
void
lh_exception_set(lh_exception_t *self, lh_error_code_t code, lh_error_desc_t desc,
                 const lh_str_ptr timestamp, const lh_str_ptr file, const lh_str_ptr function, lh_usize_t line);
#else
void
lh_exception_set(lh_exception_t *self, lh_error_code_t code, lh_error_desc_t desc);
#endif

/**
 * @brief Initialize @p self by delegating to ::lh_exception_set.
 *
 * @param self      Exception to initialize (not null).
 * @param code      Error code.
 * @param desc      Error description (may be null).
 * @param timestamp Build timestamp (typically @c __TIMESTAMP__; debug builds only).
 * @param file      Source file path (typically @c __FILE__; debug builds only).
 * @param function  Function name (typically @c __FUNCTION__; debug builds only).
 * @param line      Line number (typically @c __LINE__; debug builds only).
 */
LH_ATTRIBUTE_SYMBOL
#ifndef NDEBUG
void
lh_exception_init(lh_exception_t *self, lh_error_code_t code, lh_error_desc_t desc,
                  const lh_str_ptr timestamp, const lh_str_ptr file, const lh_str_ptr function, lh_usize_t line);
#else
void
lh_exception_init(lh_exception_t *self, lh_error_code_t code, lh_error_desc_t desc);
#endif

/**
 * @brief Initialize @p self by copying from @p error and @p origin.
 *
 * @param self   Exception to initialize (not null).
 * @param error  Source error to copy from (not null).
 * @param origin Source raise-site metadata to copy from (not null; debug builds only).
 */
LH_ATTRIBUTE_SYMBOL
#ifndef NDEBUG
void
lh_exception_init_by_error(lh_exception_t *self, const lh_error_t *error,
                            const lh_exception_origin_t *origin);
#else
void
lh_exception_init_by_error(lh_exception_t *self, const lh_error_t *error);
#endif

/* ── unpack ──────────────────────────────────────────────────────────────── */

/**
 * @brief Read fields from @p self into optional output pointers.
 *
 * Pass ::lh_null for any pointer to skip that field.
 *
 * @param self      Exception to read from (not null).
 * @param code      Output for error code, or ::lh_null to skip.
 * @param desc      Output for error description, or ::lh_null to skip.
 * @param timestamp Output for origin timestamp, or ::lh_null to skip (debug builds only).
 * @param file      Output for origin file path, or ::lh_null to skip (debug builds only).
 * @param function  Output for origin function name, or ::lh_null to skip (debug builds only).
 * @param line      Output for origin line number, or ::lh_null to skip (debug builds only).
 */
LH_ATTRIBUTE_SYMBOL
#ifndef NDEBUG
void
lh_exception_unpack(const lh_exception_t *self, lh_error_code_t *code, lh_error_desc_t *desc,
                    lh_str_cptr *timestamp, lh_str_cptr *file, lh_str_cptr *function,
                    lh_usize_t *line);
#else
void
lh_exception_unpack(const lh_exception_t *self, lh_error_code_t *code, lh_error_desc_t *desc);
#endif

/**
 * @brief Unpack @p self into @p other (alias for exception assign).
 * @param self  Source exception (not null).
 * @param other Destination exception (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_exception_unpack_to_other(const lh_exception_t *self, lh_exception_t *other);

/**
 * @brief Mutable pointer to the embedded ::lh_error_t inside @p self.
 *
 * Layout guarantees the error is the first member, so this is equivalent to casting @p self to
 * ::lh_error_t * in supported configurations.
 *
 * @param self Exception value (not null).
 * @return Pointer to @p self->error.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_t *
lh_exception_get_error(const lh_exception_t *self);

#ifndef NDEBUG
/**
 * @brief Mutable pointer to the debug origin stored in @p self.
 *
 * This accessor is available only when `NDEBUG` is not defined, because
 * ::lh_exception_t::origin is omitted from release builds.
 *
 * @param self Exception value (not null).
 * @return Pointer to @p self->origin.
 *
 * @see lh_exception_origin_t
 */
LH_ATTRIBUTE_SYMBOL
lh_exception_origin_t *
lh_exception_get_origin(const lh_exception_t *self);
#endif

/**
 * @brief Replace only the embedded error code.
 *
 * @param self Exception value to modify (not null).
 * @param code New error code.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_exception_set_code(lh_exception_t *self, lh_error_code_t code);

/**
 * @brief Replace only the embedded error description.
 *
 * @param self Exception value to modify (not null).
 * @param desc New description pointer (may be null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_exception_set_desc(lh_exception_t *self, lh_error_desc_t desc);

/**
 * @brief Return the embedded error code.
 *
 * @param self Exception value (not null).
 * @return Code stored in @p self->error.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_exception_get_code(const lh_exception_t *self);

/**
 * @brief Return the embedded error description.
 *
 * @param self Exception value (not null).
 * @return Description stored in @p self->error; may be null.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_desc_t
lh_exception_get_desc(const lh_exception_t *self);

/**
 * @brief Return the embedded description or @p fallback when it is null.
 *
 * @param self Exception value (not null).
 * @param fallback Description returned when @p self has no description.
 * @return Stored description when non-null, otherwise @p fallback.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_desc_t
lh_exception_get_desc_or(const lh_exception_t *self, lh_error_desc_t fallback);

/**
 * @brief Test whether the embedded error code is equal to @p code.
 *
 * @param self Exception value (not null).
 * @param code Error code to compare with.
 * @return ::lh_bool_true when @p self carries @p code, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_has_code(const lh_exception_t *self, lh_error_code_t code);

/**
 * @brief Test whether the embedded error has the success code.
 *
 * @param self Exception value (not null).
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_is_ok(const lh_exception_t *self);

/**
 * @brief Test whether the embedded error has a non-success code.
 *
 * @param self Exception value (not null).
 * @return ::lh_bool_true when @p self does not store ::lh_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_is_failure(const lh_exception_t *self);

/**
 * @brief Test whether the embedded error has a non-null description.
 *
 * @param self Exception value (not null).
 * @return ::lh_bool_true when @p self stores a description pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_has_desc(const lh_exception_t *self);

/**
 * @brief Test whether the embedded error is the cleared success state.
 *
 * @param self Exception value (not null).
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok and no description.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_is_empty(const lh_exception_t *self);

/**
 * @brief Test whether two exceptions carry equal embedded errors.
 *
 * Debug origin metadata is ignored. Description equality is pointer equality,
 * matching ::lh_error_equals.
 *
 * @param self Exception value (not null).
 * @param other Exception value to compare with (not null).
 * @return ::lh_bool_true when both exceptions carry equal embedded errors.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_equals(const lh_exception_t *self, const lh_exception_t *other);

/**
 * @brief Test whether two exceptions carry the same embedded error code.
 *
 * Descriptions and debug origins are ignored.
 *
 * @param self Exception value (not null).
 * @param other Exception value to compare with (not null).
 * @return ::lh_bool_true when both exceptions carry the same code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_has_same_code(const lh_exception_t *self, const lh_exception_t *other);

/**
 * @brief Test whether two exceptions carry different embedded error codes.
 *
 * Descriptions and debug origins are ignored.
 *
 * @param self Exception value (not null).
 * @param other Exception value to compare with (not null).
 * @return ::lh_bool_true when both exceptions carry different codes.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_has_diff_code(const lh_exception_t *self, const lh_exception_t *other);

LH_COMPILER_EXTERN_C_END

#endif // LH_EXCEPTION_H
