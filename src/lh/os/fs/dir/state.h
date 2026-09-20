/**
 * @file state.h
 * @brief Heap cursor behind ::lh_os_fs_dir_t's handle. Not public.
 */

#ifndef LH_OS_FS_DIR_STATE_H
#define LH_OS_FS_DIR_STATE_H

#include <lh/os/fs/dir.h>
#include <lh/bool.h>
#include <lh/cast/reinterpret.h>
#include <lh/char.h>
#include <lh/compiler/os.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    ifndef IO_REPARSE_TAG_SYMLINK
#        define IO_REPARSE_TAG_SYMLINK 0xA000000CUL
#    endif
#else
#    include <dirent.h>
#endif

struct lh_os_fs_dir_state
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    HANDLE find;
    WIN32_FIND_DATAA data;
#else
    DIR *dir;
    unsigned char type;
    lh_char_t pending[LH_OS_FS_DIR_NAME_MAX + 1U];
#endif
    lh_bool_t ready;
};

static inline void
lh_os_fs_dir_put_state(lh_os_fs_dir_t *self, struct lh_os_fs_dir_state *state)
{
    lh_ptr_deref(lh_os_fs_dir_get_handle(self)) =
        lh_cast_reinterpret(lh_os_fs_file_handle_t, state);
}

static inline struct lh_os_fs_dir_state *
lh_os_fs_dir_get_state(lh_os_fs_dir_t *self)
{
    return lh_cast_reinterpret(struct lh_os_fs_dir_state *,
                               lh_ptr_deref(lh_os_fs_dir_get_handle(self)));
}

#endif /* LH_OS_FS_DIR_STATE_H */
