/**
 * @file map.h
 * @brief A mapped view of an open file (::lh_os_fs_file_map_t).
 *
 * Unix `mmap` is the model. Windows `CreateFileMapping` / `MapViewOfFile`
 * maps onto it. This is a value, not a field of ::lh_os_fs_file_t: the
 * handle can close while the map lives; unmap is a separate call.
 *
 * Offset need not be page-aligned; the OS region is aligned down and
 * ::lh_os_fs_file_map_get_data starts at the requested byte.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_FS_FILE_MAP_H
#define LH_OS_FS_FILE_MAP_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/fs/file/map/fields.h>
#include <lh/os/fs/file/map/prot.h>
#include <lh/ptr.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/fs/file/map.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

struct lh_os_fs_file;

/**
 * @struct lh_os_fs_file_map
 * @brief One mmap-shaped view of a file.
 */
struct lh_os_fs_file_map
{
    lh_os_fs_file_map_fields(lh_ptr, lh_usize_t);
};

/**
 * @typedef lh_os_fs_file_map_t
 * @brief Alias for `struct lh_os_fs_file_map`.
 */
typedef struct lh_os_fs_file_map lh_os_fs_file_map_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty map: null data, size `0`. Does not touch the OS.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_map_init(lh_os_fs_file_map_t *self);

/**
 * @brief Unmap @p self (if mapped) and return it to empty.
 *
 * Safe to call on an already-empty map.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_fs_file_map_unmap(lh_os_fs_file_map_t *self);

LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_os_fs_file_map_get_data(const lh_os_fs_file_map_t *self);

LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_os_fs_file_map_get_size(const lh_os_fs_file_map_t *self);

/**
 * @brief True when @p self currently holds an OS mapping (`base` not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_map_is_valid(const lh_os_fs_file_map_t *self);

/**
 * @brief Map @p size bytes of @p file starting at @p offset (`mmap`).
 *
 * @p size `0` means from @p offset to EOF. Write protection needs a
 * read-write handle (::lh_os_fs_file_mode_readwrite).
 *
 * @return ::lh_bool_true on success. On false, ::lh_os_get_last_error.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_fs_file_map(const struct lh_os_fs_file *file, lh_u64_t offset, lh_u64_t size,
                  lh_os_fs_file_map_prot_t prot, lh_os_fs_file_map_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_FS_FILE_MAP_H */
