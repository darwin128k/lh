/**
 * @file path.h
 * @brief A filesystem path (::lh_fs_path_t): root plus segments.
 *
 * Pure value type — no clock, no OS, one buffer of its own. Builds and runs anywhere ::lh itself does, including without
 * an OS (see `lh/os/fs` for the actual filesystem I/O contract — open,
 * stat, list — which does depend on the platform and takes an
 * ::lh_fs_path_t as input). Same split as ::lh_net_ip4_t (`lh/net/ip.h`)
 * vs `lh/os/net`.
 *
 * Stored the way `java.nio`'s UnixPath/WindowsPath are: one normalized
 * text (root prefix, then segments joined by `/`) plus the root kind. A
 * segment is a slice of that text (::lh_fs_path_get_segment), never a
 * string of its own, so parsing is one allocation and one pass and reading a
 * segment allocates nothing. The root is still its own field — a segment is
 * never asked "are you secretly the root". A name, not a disk probe:
 * exists / is-file / stat are not here.
 */

#ifndef LH_FS_PATH_H
#define LH_FS_PATH_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/fs/path/fields.h>
#include <lh/fs/path/root/kind.h>
#include <lh/fs/path/style.h>
#include <lh/index.h>
#include <lh/size.h>
#include <lh/str.h>
#include <lh/str/ptr.h>
#include <lh/str/view.h>

/**
 * @struct lh_fs_path
 * @brief Normalized text plus root kind. Fields via ::lh_fs_path_fields.
 */
typedef struct lh_fs_path
{
    lh_fs_path_fields(lh_str_t, lh_fs_path_root_kind_t);
} lh_fs_path_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty, relative path (no root, no segments). Does not touch the OS.
 */
void
lh_fs_path_init(lh_fs_path_t *self);

/**
 * @brief Release everything owned by @p self.
 */
void
lh_fs_path_deinit(lh_fs_path_t *self);

/**
 * @brief Reset @p self to empty (no root, no segments). Keeps allocations.
 */
void
lh_fs_path_clear(lh_fs_path_t *self);

/**
 * @brief Copy @p other into @p self (root and segments).
 */
void
lh_fs_path_assign(lh_fs_path_t *self, const lh_fs_path_t *other);

/**
 * @brief What (if anything) @p self is rooted at.
 */
lh_fs_path_root_kind_t
lh_fs_path_get_root_kind(const lh_fs_path_t *self);

/**
 * @brief Drive letter of @p self; meaningful only when the root kind is
 *        ::lh_fs_path_root_kind_drive.
 */
lh_char_t
lh_fs_path_get_root_drive(const lh_fs_path_t *self);

/**
 * @brief True when @p part has the shape of a drive: one letter and `:`
 *        (`C:`). A shape test only — whether a drive is meaningful is the
 *        style's call (::lh_fs_path_set only looks for one in
 *        ::lh_fs_path_style_windows).
 */
lh_bool_t
lh_fs_path_is_drive(lh_str_view_t part);

/**
 * @brief Number of segments in @p self.
 */
lh_usize_t
lh_fs_path_get_segment_count(const lh_fs_path_t *self);

/**
 * @brief Segment @p index of @p self (`0` is the first after the root), as
 *        a view into @p self's own text.
 *
 * Valid until @p self is next modified or deinitialized.
 *
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p index is not below ::lh_fs_path_get_segment_count.
 */
lh_str_view_t
lh_fs_path_get_segment(const lh_fs_path_t *self, lh_uindex_t index);

/**
 * @brief True when @p self has neither a root nor any segments.
 */
lh_bool_t
lh_fs_path_is_empty(const lh_fs_path_t *self);

/**
 * @brief True when @p self is rooted (POSIX `/` or a drive), as opposed to
 *        relative.
 */
lh_bool_t
lh_fs_path_is_absolute(const lh_fs_path_t *self);

/**
 * @brief True when @p self is exactly a root (`/` or a drive) with no
 *        segments — nothing to drop before hitting bedrock.
 */
lh_bool_t
lh_fs_path_is_root(const lh_fs_path_t *self);

/**
 * @brief True when @p self's last segment is hidden by the dot-prefix
 *        naming convention (`.name`, but not `.` or `..`).
 *
 * Pure naming convention, not a filesystem attribute — no OS call, no
 * dependency on any platform's actual "hidden" bit (see `lh/fs/attr.h`
 * for that). ::lh_bool_false for an empty path or one with no segments.
 */
lh_bool_t
lh_fs_path_is_hidden(const lh_fs_path_t *self);

/**
 * @brief Parse @p text into @p self's root and segments, replacing its
 *        current contents.
 *
 * @p style decides which characters separate segments and whether a
 * leading `C:` is a drive (see ::lh_fs_path_style_t). A leading separator
 * becomes ::lh_fs_path_root_kind_posix; a drive letter becomes
 * ::lh_fs_path_root_kind_drive. Repeated and trailing separators produce no
 * empty segments.
 */
void
lh_fs_path_set(lh_fs_path_t *self, lh_str_view_t text, lh_fs_path_style_t style);

/**
 * @brief Render @p self's root and segments into @p out in @p style,
 *        replacing its current contents.
 *
 * A drive root renders as `C:` plus the style's separator in either style;
 * the posix style has no drives of its own, but does not drop one.
 *
 * One copy of @p self's text: verbatim for ::lh_fs_path_style_posix, with
 * `/` swapped for a backslash for ::lh_fs_path_style_windows. @p out is an
 * ordinary ::lh_str_t; get the `const char *` for an OS call with
 * ::lh_str_get_data(@p out).
 */
void
lh_fs_path_to_str(const lh_fs_path_t *self, lh_fs_path_style_t style, lh_str_t *out);

/**
 * @brief Render @p self in @p style into @p scratch and return its
 *        `const char *`.
 *
 * @p scratch is initialized by this call (must not already be initialized)
 * and owns the text ::lh_fs_path_to_str builds; the caller is responsible
 * for ::lh_str_deinit(@p scratch) once done with the returned pointer.
 * Equivalent to init + ::lh_fs_path_to_str + ::lh_str_get_data, collapsed
 * for the common "one OS call" case.
 */
lh_str_cptr
lh_fs_path_to_cstr(const lh_fs_path_t *self, lh_fs_path_style_t style, lh_str_t *scratch);

/**
 * @brief Join @p dir and @p name into @p self by appending @p name's
 *        segments (one append of @p name's text). @p name's own root (if any) is ignored — @p self keeps
 *        @p dir's root.
 *
 * @p self may alias @p dir and/or @p name.
 *
 * @return ::lh_bool_false (and clears @p self) when @p name has no segments.
 */
lh_bool_t
lh_fs_path_join(lh_fs_path_t *self, const lh_fs_path_t *dir, const lh_fs_path_t *name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_FS_PATH_H */
