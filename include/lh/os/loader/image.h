/**
 * @file image.h
 * @brief Where a module image lies: the native suffix, the image's own name, and
 *        the folder layout `<name>/<name>.so|.dll`.
 *
 * A module image is a shared library of this system (::lh_os_system_shared_get_ext
 * gives its suffix). A host that keeps its modules in folders — one folder per
 * module, the image inside named like the folder — names a module by the folder
 * alone, and ::lh_os_loader_image_try_resolve turns that name into the image
 * path. A name that already points at a file is taken as it is, so a list may
 * give either form.
 *
 * These are pieces of a loading policy, not a policy: which folder to look in,
 * which list to read and what to do with an image are the caller's
 * (::lh_os_loader_load takes one path at a time).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_LOADER_IMAGE_H
#define LH_OS_LOADER_IMAGE_H

#include <lh/config.h>
#include <lh/core/fs/path.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/loader/image.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether the last segment of @p path ends with this system's shared
 *        library suffix (`.dll` / `.so`), in any case.
 *
 * @param path The path; a path without segments has no suffix.
 */
lh_bool_t
lh_os_loader_image_has_ext(const lh_fs_path_t *path);

/**
 * @brief Whether the last segment of @p path is @p stem followed by this
 *        system's shared library suffix, in any case (`authum.dll` for
 *        `authum`).
 *
 * @param path The path; a path without segments is no image.
 * @param stem The file name without its suffix; checked by
 *             ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_loader_image_is_named(const lh_fs_path_t *path, const lh_str_view_t *stem);

/**
 * @brief The image an entry names: the entry itself when it is a file, else
 *        `<entry>/<last segment of entry><suffix>`.
 *
 * `qproto/qproto.so` (a file) gives itself; `qproto` (a folder) gives
 * `qproto/qproto.so`. The folder form is not checked to exist: the caller
 * opens it, or looks at it with ::lh_os_fs_is_file.
 *
 * @param base  The folder @p entry is relative to, or ::lh_null to take
 *              @p entry as it is (an absolute entry).
 * @param entry The entry; a path without segments names nothing.
 * @param out   Receives the image path.
 * @return ::lh_bool_false when @p entry has no segment or the paths do not join
 *         (::lh_fs_path_join); what @p out holds then is unspecified.
 */
lh_bool_t
lh_os_loader_image_try_resolve(const lh_fs_path_t *base, const lh_fs_path_t *entry,
                               lh_fs_path_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOADER_IMAGE_H */
