/**
 * @file origin.h
 * @brief Optional “where was this thrown?” metadata for ::lh_exception_t.
 *
 * In **debug** builds (`NDEBUG` not defined),
 * ::lh_exception_t carries an ::lh_exception_origin_t:
 * one **source site** (file, function, line, optional timestamp), not a full stack trace.
 * In **release** builds the field is stripped from ::lh_exception_t entirely,
 * so there is no size or string cost.
 *
 * File, function, and timestamp are stored as `const char *` * (typically string literals from
 * `__FILE__`, `__FUNCTION__`, `__TIMESTAMP__`).
 *
 * Those pointers must remain valid for as long as the origin is read.
 * `line` is a ::lh_usize_t (often `__LINE__`).
 *
 * Typical initializers: `lh/exception/origin/initializer.h`.
 */

#ifndef LH_EXCEPTION_ORIGIN_H
#define LH_EXCEPTION_ORIGIN_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @struct lh_exception_origin
 * @brief One raise site: optional timestamp plus file, function, and line.
 *
 * Filled when constructing an exception for diagnostics.
 * String pointers may be null; `line` is a ::lh_usize_t (often from `__LINE__`).
 *
 * @see lh_exception_t
 * @see lh_exception_origin_initializer()
 * @see lh_exception_origin_initializer_now()
 */
typedef struct lh_exception_origin
{
    lh_str_cptr timestamp; /**< Optional time stamp text (e.g. from `__TIMESTAMP__`). */
    lh_str_cptr filename;  /**< Source path fragment (often `__FILE__`). */
    lh_str_cptr function;  /**< Function name (often `__FUNCTION__` / `__func__`). */
    lh_usize_t line;       /**< Line number (typically `__LINE__`). */
} lh_exception_origin_t;   /**< Typedef for struct ::lh_exception_origin. */

LH_COMPILER_EXTERN_C_BEGIN

/* ── set / init ──────────────────────────────────────────────────────────── */

/**
 * @brief Write all fields of @p self.
 *
 * @param self      Origin to modify (not null).
 * @param timestamp Build timestamp (typically @c __TIMESTAMP__).
 * @param file      Source file path (typically @c __FILE__).
 * @param function  Function name (typically @c __FUNCTION__).
 * @param line      Line number (typically @c __LINE__).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_exception_origin_set(lh_exception_origin_t *self, lh_str_cptr timestamp,
                        lh_str_cptr file, lh_str_cptr function, lh_usize_t line);

/**
 * @brief Initialize @p self by delegating to ::lh_exception_origin_set.
 *
 * @param self      Origin to initialize (not null).
 * @param timestamp Build timestamp (typically @c __TIMESTAMP__).
 * @param file      Source file path (typically @c __FILE__).
 * @param function  Function name (typically @c __FUNCTION__).
 * @param line      Line number (typically @c __LINE__).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_exception_origin_init(lh_exception_origin_t *self, lh_str_cptr timestamp,
                         lh_str_cptr file, lh_str_cptr function, lh_usize_t line);

/* ── unpack ──────────────────────────────────────────────────────────────── */

/**
 * @brief Read fields from @p self into optional output pointers.
 *
 * Pass ::lh_null for any pointer to skip that field.
 *
 * @param self      Origin to read from (not null).
 * @param timestamp Output for @c timestamp, or ::lh_null to skip.
 * @param file      Output for @c filename, or ::lh_null to skip.
 * @param function  Output for @c function, or ::lh_null to skip.
 * @param line      Output for @c line, or ::lh_null to skip.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_exception_origin_unpack(const lh_exception_origin_t *self, lh_str_cptr *timestamp,
                           lh_str_cptr *file, lh_str_cptr *function, lh_usize_t *line);

/**
 * @brief Unpack @p self into @p other (alias for ::lh_exception_origin_assign).
 * @param self  Source origin (not null).
 * @param other Destination origin (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_exception_origin_unpack_to_other(const lh_exception_origin_t *self,
                                    lh_exception_origin_t *other);

/* ── getters ─────────────────────────────────────────────────────────────── */

/**
 * @brief Return the timestamp stored in @p self.
 * @param self Origin to read from (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_exception_origin_get_timestamp(const lh_exception_origin_t *self);

/**
 * @brief Return the filename stored in @p self.
 * @param self Origin to read from (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_exception_origin_get_file(const lh_exception_origin_t *self);

/**
 * @brief Return the function name stored in @p self.
 * @param self Origin to read from (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_exception_origin_get_function(const lh_exception_origin_t *self);

/**
 * @brief Return the line number stored in @p self.
 * @param self Origin to read from (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_exception_origin_get_line(const lh_exception_origin_t *self);

/* ── copy ────────────────────────────────────────────────────────────────── */

/**
 * @brief Copy the origin state from @p other into @p self.
 * @param self  Destination (not null).
 * @param other Source (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_exception_origin_assign(lh_exception_origin_t *self, const lh_exception_origin_t *other);

/**
 * @brief Initialize @p self by copying from @p other.
 * @param self  Origin to initialize (not null).
 * @param other Source origin to copy from (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_exception_origin_init_by_other(lh_exception_origin_t *self,
                                  const lh_exception_origin_t *other);

LH_COMPILER_EXTERN_C_END

#endif // LH_EXCEPTION_ORIGIN_H
