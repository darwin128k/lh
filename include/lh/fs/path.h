/**
 * @file path.h
 * @brief A filesystem path (::lh_fs_path_t): root plus segments.
 *
 * Pure value type — no clock, no OS, no allocation beyond the segment
 * list's own. Builds and runs anywhere ::lh itself does, including without
 * an OS (see `lh/os/fs` for the actual filesystem I/O contract — open,
 * stat, list — which does depend on the platform and takes an
 * ::lh_fs_path_t as input). Same split as ::lh_net_ip4_t (`lh/net/ip.h`)
 * vs `lh/os/net`.
 *
 * Root (::lh_fs_path_root_kind_t + drive letter) and segments
 * (::lh_str_list_t of real names) are separate fields — a segment is never
 * asked "are you secretly the root". A name, not a disk probe: exists /
 * is-file / stat are not here.
 */

#ifndef LH_FS_PATH_H
#define LH_FS_PATH_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/fs/path/fields.h>
#include <lh/fs/path/root/kind.h>
#include <lh/size.h>
#include <lh/str.h>
#include <lh/str/list.h>
#include <lh/str/ptr.h>
#include <lh/str/view.h>

/**
 * @struct lh_fs_path
 * @brief Root plus segments. Fields via ::lh_fs_path_fields.
 */
typedef struct lh_fs_path
{
    lh_fs_path_fields(lh_fs_path_root_kind_t, lh_char_t, lh_str_list_t);
} lh_fs_path_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty, relative path (no root, no segments). Does not touch the OS.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_fs_path_init(lh_fs_path_t *self);

/**
 * @brief Release everything owned by @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_fs_path_deinit(lh_fs_path_t *self);

/**
 * @brief Reset @p self to empty (no root, no segments). Keeps allocations.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_fs_path_clear(lh_fs_path_t *self);

/**
 * @brief Copy @p other into @p self (root and segments).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_fs_path_assign(lh_fs_path_t *self, const lh_fs_path_t *other);

/**
 * @brief What (if anything) @p self is rooted at.
 */
LH_ATTRIBUTE_SYMBOL
lh_fs_path_root_kind_t
lh_fs_path_get_root_kind(const lh_fs_path_t *self);

/**
 * @brief Drive letter of @p self; meaningful only when the root kind is
 *        ::lh_fs_path_root_kind_drive.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_fs_path_get_root_drive(const lh_fs_path_t *self);

/**
 * @brief True when @p part is a Windows drive (`C:`). Elsewhere always false.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_fs_path_is_drive(lh_str_view_t part);

/**
 * @brief Segments of @p self, after validating the pointer.
 *
 * Single access to `segments`.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_list_t *
lh_fs_path_get_segments(lh_fs_path_t *self);

/**
 * @brief `const` counterpart to ::lh_fs_path_get_segments.
 */
LH_ATTRIBUTE_SYMBOL
const lh_str_list_t *
lh_fs_path_get_segments_as_const(const lh_fs_path_t *self);

/**
 * @brief Number of segments in @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_fs_path_get_segment_count(const lh_fs_path_t *self);

/**
 * @brief True when @p self has neither a root nor any segments.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_fs_path_is_empty(const lh_fs_path_t *self);

/**
 * @brief True when @p self is rooted (POSIX `/` or a drive), as opposed to
 *        relative.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_fs_path_is_absolute(const lh_fs_path_t *self);

/**
 * @brief True when @p self is exactly a root (`/` or a drive) with no
 *        segments — nothing to drop before hitting bedrock.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_fs_path_is_root(const lh_fs_path_t *self);

/**
 * @brief True when @p self's last segment is hidden by the dot-prefix
 *        naming convention (`.name`, but not `.` or `..`).
 *
 * Pure naming convention, not a filesystem attribute — no OS call, no
 * dependency on any platform's actual "hidden" bit (see `lh/os/fs/attr.h`
 * for that). ::lh_bool_false for an empty path or one with no segments.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_fs_path_is_hidden(const lh_fs_path_t *self);

/**
 * @brief Parse @p text into @p self's root and segments, replacing its
 *        current contents.
 *
 * A leading separator becomes ::lh_fs_path_root_kind_posix; a drive letter
 * (`C:`) becomes ::lh_fs_path_root_kind_drive. Repeated and trailing
 * separators produce no empty segments.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_fs_path_set(lh_fs_path_t *self, lh_str_view_t text);

/**
 * @brief Render @p self's root and segments into @p out as OS-ready text
 *        (`CreateFileA` / `open` encoding), replacing its current contents.
 *
 * Pure and stateless: @p self keeps no cached text of its own — this is
 * computed fresh every call. @p out is an ordinary ::lh_str_t; get the
 * `const char *` for an OS call with ::lh_str_get_data(@p out).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_fs_path_to_str(const lh_fs_path_t *self, lh_str_t *out);

/**
 * @brief Render @p self into @p scratch and return its `const char *`.
 *
 * @p scratch is initialized by this call (must not already be initialized)
 * and owns the text ::lh_fs_path_to_str builds; the caller is responsible
 * for ::lh_str_deinit(@p scratch) once done with the returned pointer.
 * Equivalent to init + ::lh_fs_path_to_str + ::lh_str_get_data, collapsed
 * for the common "one OS call" case.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_fs_path_to_cstr(const lh_fs_path_t *self, lh_str_t *scratch);

/**
 * @brief Join @p dir and @p name into @p self by appending @p name's
 *        segments. @p name's own root (if any) is ignored — @p self keeps
 *        @p dir's root.
 *
 * @p self may alias @p dir and/or @p name.
 *
 * @return ::lh_bool_false (and clears @p self) when @p name has no segments.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_fs_path_join(lh_fs_path_t *self, const lh_fs_path_t *dir, const lh_fs_path_t *name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_FS_PATH_H */
