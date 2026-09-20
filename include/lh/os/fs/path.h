/**
 * @file path.h
 * @brief A filesystem path (::lh_os_fs_path_t): levels plus a separator.
 *
 * A name, not a disk probe. Internally one ::lh_str_t buffer and offsets
 * of each name (OpenJDK `UnixPath`). `set` parses into that buffer;
 * ::lh_os_fs_path_join appends a level; ::lh_os_fs_path_get_text is the
 * glued string. Same encoding as `LoadLibraryA` / POSIX bytes. No wide
 * paths. Exists / is-file / stat / exe are not here.
 *
 * On failure the reason is in ::lh_os_get_last_error (see `lh/os.h`).
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_PATH_H
#define LH_OS_FS_PATH_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/index.h>
#include <lh/os/fs/path/fields.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/str.h>
#include <lh/str/ptr.h>
#include <lh/str/view.h>
#include <lh/vector.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/path.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_fs_path
 * @brief One filesystem path. Fields via ::lh_os_fs_path_fields.
 */
struct lh_os_fs_path
{
    lh_os_fs_path_fields(lh_char_t, lh_str_t, lh_vector_t);
};

/**
 * @typedef lh_os_fs_path_t
 * @brief Alias for `struct lh_os_fs_path`.
 */
typedef struct lh_os_fs_path lh_os_fs_path_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Preferred directory separator for this OS (`'\\'` on Windows,
 *        `'/'` elsewhere).
 *
 * Join glues with this character. Parse accepts `'/'` as well on Windows.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_os_fs_path_sep(void);

/**
 * @brief Empty path: no levels, empty buffer, OS separator stored.
 *
 * Call once on uninitialized storage. Reset an already-initialized path
 * with ::lh_os_fs_path_clear; free it with ::lh_os_fs_path_deinit.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_init(lh_os_fs_path_t *self);

/**
 * @brief Drop every level and empty the buffer. Keeps allocations.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_clear(lh_os_fs_path_t *self);

/**
 * @brief Release the buffer and the offset table.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_deinit(lh_os_fs_path_t *self);

/**
 * @brief Copy @p other into @p self (buffer and offsets).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_path_assign(lh_os_fs_path_t *self, const lh_os_fs_path_t *other);

/**
 * @brief Parse @p text into the buffer and name offsets.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_set(lh_os_fs_path_t *self, lh_str_view_t text);

/**
 * @brief Stored separator of @p self, after validating the pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t *
lh_os_fs_path_get_sep(lh_os_fs_path_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_path_get_sep.
 */
LH_ATTRIBUTE_SYMBOL
const lh_char_t *
lh_os_fs_path_get_sep_as_const(const lh_os_fs_path_t *self);

/**
 * @brief Offset table of @p self, after validating the pointer.
 *
 * Elements are `{offset, size}` into ::lh_os_fs_path_get_text. Other
 * functions go through this or ::lh_os_fs_path_get_parts_as_const instead
 * of `self->parts`.
 */
LH_ATTRIBUTE_SYMBOL
lh_vector_t *
lh_os_fs_path_get_parts(lh_os_fs_path_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_path_get_parts.
 */
LH_ATTRIBUTE_SYMBOL
const lh_vector_t *
lh_os_fs_path_get_parts_as_const(const lh_os_fs_path_t *self);

/**
 * @brief Level at @p index, as a view into the path buffer.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_view_t
lh_os_fs_path_get_part(const lh_os_fs_path_t *self, lh_uindex_t index);

/**
 * @brief Path buffer of @p self, after validating the pointer.
 *
 * Single access to `text`.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_t *
lh_os_fs_path_get_text(lh_os_fs_path_t *self);

/**
 * @brief `const` counterpart to ::lh_os_fs_path_get_text.
 */
LH_ATTRIBUTE_SYMBOL
const lh_str_t *
lh_os_fs_path_get_text_as_const(const lh_os_fs_path_t *self);

/**
 * @brief View over the path buffer of @p self (does not include the NUL).
 */
LH_ATTRIBUTE_SYMBOL
lh_str_view_t
lh_os_fs_path_as_view(const lh_os_fs_path_t *self);

/**
 * @brief True when @p self has no levels.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_is_empty(const lh_os_fs_path_t *self);

/**
 * @brief Join @p dir and @p name into @p out by appending @p name's levels.
 *
 * Empty @p name is an error.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_join(lh_os_fs_path_t *out, const lh_os_fs_path_t *dir, const lh_os_fs_path_t *name);

/**
 * @brief Directory containing @p path. A single relative level becomes `.`.
 *        A drive or `/` is left as the root.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_path_dir(const lh_os_fs_path_t *path, lh_os_fs_path_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_PATH_H */
