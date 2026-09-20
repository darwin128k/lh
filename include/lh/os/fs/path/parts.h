/**
 * @file parts.h
 * @brief Offset table of a path: a growable list of ::lh_os_fs_path_span_t.
 *
 * Does not own the path bytes. Views of a level stay on the path
 * (::lh_os_fs_path_get_part). Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_PATH_PARTS_H
#define LH_OS_FS_PATH_PARTS_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/index.h>
#include <lh/os/fs/path/parts/fields.h>
#include <lh/os/fs/path/span.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/vector.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/path/parts.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_path_parts
 * @brief Table of name spans. Fields via ::lh_os_fs_path_parts_fields.
 */
struct lh_os_fs_path_parts
{
    lh_os_fs_path_parts_fields(lh_vector_t);
};

/**
 * @typedef lh_os_fs_path_parts_t
 * @brief Alias for `struct lh_os_fs_path_parts`.
 */
typedef struct lh_os_fs_path_parts lh_os_fs_path_parts_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty table. Element type is ::lh_os_fs_path_span_t.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_parts_init(lh_os_fs_path_parts_t *self);

/**
 * @brief Drop every span. Keeps the allocation.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_parts_clear(lh_os_fs_path_parts_t *self);

/**
 * @brief Release the table.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_parts_deinit(lh_os_fs_path_parts_t *self);

/**
 * @brief Replace @p self with a copy of @p other.
 *
 * No-op when @p self is @p other.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_parts_assign(lh_os_fs_path_parts_t *self, const lh_os_fs_path_parts_t *other);

/**
 * @brief True when the table has no spans.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_parts_is_empty(const lh_os_fs_path_parts_t *self);

/**
 * @brief Number of spans.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_os_fs_path_parts_get_size(const lh_os_fs_path_parts_t *self);

/**
 * @brief Span at @p index.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_fs_path_span_t *
lh_os_fs_path_parts_get(lh_os_fs_path_parts_t *self, lh_uindex_t index);

/**
 * @brief `const` counterpart to ::lh_os_fs_path_parts_get.
 */
LH_ATTRIBUTE_SYMBOL
const lh_os_fs_path_span_t *
lh_os_fs_path_parts_get_as_const(const lh_os_fs_path_parts_t *self, lh_uindex_t index);

/**
 * @brief Append a copy of @p span.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_parts_push(lh_os_fs_path_parts_t *self, const lh_os_fs_path_span_t *span);

/**
 * @brief Remove the last span, optionally copying it into @p dst.
 *
 * @param dst Destination for the removed span, or ::lh_null to discard it.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_parts_pop(lh_os_fs_path_parts_t *self, lh_os_fs_path_span_t *dst);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_PATH_PARTS_H */
