/**
 * @file span.h
 * @brief One name in a path: offset and length in the path buffer.
 *
 * Element of ::lh_os_fs_path_parts_t. Slice the buffer with
 * ::lh_os_fs_path_span_as_view. Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_PATH_SPAN_H
#define LH_OS_FS_PATH_SPAN_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/fs/path/span/fields.h>
#include <lh/size.h>
#include <lh/str/view.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/path/span.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_path_span
 * @brief Offset and length of one level. Fields via ::lh_os_fs_path_span_fields.
 */
struct lh_os_fs_path_span
{
    lh_os_fs_path_span_fields(lh_usize_t, lh_usize_t);
};

/**
 * @typedef lh_os_fs_path_span_t
 * @brief Alias for `struct lh_os_fs_path_span`.
 */
typedef struct lh_os_fs_path_span lh_os_fs_path_span_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Store @p offset and @p size in @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_span_init(lh_os_fs_path_span_t *self, lh_usize_t offset, lh_usize_t size);

/**
 * @brief Span with @p offset and @p size.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_fs_path_span_t
lh_os_fs_path_span_make(lh_usize_t offset, lh_usize_t size);

/**
 * @brief Start of the name in the path buffer.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_os_fs_path_span_get_offset(const lh_os_fs_path_span_t *self);

/**
 * @brief Length of the name in characters.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_os_fs_path_span_get_size(const lh_os_fs_path_span_t *self);

/**
 * @brief True when the name has no characters (empty root level).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_span_is_empty(const lh_os_fs_path_span_t *self);

/**
 * @brief View of this name inside @p text (the path buffer).
 */
LH_ATTRIBUTE_SYMBOL
lh_str_view_t
lh_os_fs_path_span_as_view(const lh_os_fs_path_span_t *self, lh_str_view_t text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_PATH_SPAN_H */
