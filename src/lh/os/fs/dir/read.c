#include <lh/os/fs/dir.h>
#include "state.h"
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>
#include <lh/util/str/ptr/dot.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#else
#    include <errno.h>
#endif

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static lh_os_fs_dir_entry_kind_t
lh_os_fs_dir_kind_win(const WIN32_FIND_DATAA *data)
{
    if ((data->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U &&
        data->dwReserved0 == IO_REPARSE_TAG_SYMLINK)
    {
        return lh_os_fs_dir_entry_kind_symlink;
    }
    if ((data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0U)
    {
        return lh_os_fs_dir_entry_kind_dir;
    }
    return lh_os_fs_dir_entry_kind_file;
}
#else
static lh_os_fs_dir_entry_kind_t
lh_os_fs_dir_kind_posix(unsigned char type)
{
#    ifdef DT_LNK
    if (type == DT_LNK)
    {
        return lh_os_fs_dir_entry_kind_symlink;
    }
#    endif
#    ifdef DT_DIR
    if (type == DT_DIR)
    {
        return lh_os_fs_dir_entry_kind_dir;
    }
#    endif
#    ifdef DT_REG
    if (type == DT_REG)
    {
        return lh_os_fs_dir_entry_kind_file;
    }
#    endif
    (void)type;
    return lh_os_fs_dir_entry_kind_other;
}
#endif

static lh_bool_t
lh_os_fs_dir_is_dot_name(lh_str_cptr name)
{
    return (lh_str_ptr_is_dot(name) || lh_str_ptr_is_double_dot(name)) ? lh_bool_true
                                                                       : lh_bool_false;
}

static lh_ssize_t
lh_os_fs_dir_eof(lh_os_fs_path_t *name)
{
    lh_os_fs_path_clear(name);
    return 0;
}

static lh_ssize_t
lh_os_fs_dir_copy_name(lh_str_cptr name, lh_os_fs_path_t *out, lh_os_fs_dir_entry_kind_t kind,
                       lh_os_fs_dir_entry_kind_t *kind_out)
{
    lh_str_view_t view;
    lh_usize_t n;

    view = lh_str_view_make(name);
    n = lh_str_view_is_empty(lh_addr_of(view)) ? 0U : lh_str_view_get_size(lh_addr_of(view));
    if (n > LH_OS_FS_DIR_NAME_MAX)
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_name_too_long,
                             lh_os_error_desc_lit("name is too long")));
        return lh_cast_static(lh_ssize_t, -1);
    }
    if (!lh_os_fs_path_set(out, view))
    {
        return lh_cast_static(lh_ssize_t, -1);
    }
    if (lh_null_ne(kind_out))
    {
        *kind_out = kind;
    }
    return lh_cast_static(lh_ssize_t, n);
}

static lh_ssize_t
lh_os_fs_dir_take_name(struct lh_os_fs_dir_state *state, lh_str_cptr entry_name,
                       lh_os_fs_dir_entry_kind_t kind, lh_os_fs_path_t *name,
                       lh_os_fs_dir_entry_kind_t *kind_out)
{
    lh_ssize_t n;

    n = lh_os_fs_dir_copy_name(entry_name, name, kind, kind_out);
    if (n >= 0 || lh_str_ptr_len(entry_name) > LH_OS_FS_DIR_NAME_MAX)
    {
        state->ready = lh_bool_false;
    }
    return n;
}

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static lh_ssize_t
lh_os_fs_dir_read_native(struct lh_os_fs_dir_state *state, lh_os_fs_path_t *name,
                         lh_os_fs_dir_entry_kind_t *kind)
{
    for (;;)
    {
        if (!state->ready)
        {
            if (!FindNextFileA(state->find, lh_addr_of(state->data)))
            {
                if (GetLastError() == ERROR_NO_MORE_FILES)
                {
                    return lh_os_fs_dir_eof(name);
                }
                lh_os_system_error_capture();
                return lh_cast_static(lh_ssize_t, -1);
            }
            state->ready = lh_bool_true;
        }
        if (lh_os_fs_dir_is_dot_name(state->data.cFileName))
        {
            state->ready = lh_bool_false;
            continue;
        }
        return lh_os_fs_dir_take_name(state, state->data.cFileName,
                                      lh_os_fs_dir_kind_win(lh_addr_of(state->data)), name, kind);
    }
}
#else
static void
lh_os_fs_dir_stash(struct lh_os_fs_dir_state *state, lh_str_cptr entry_name, unsigned char type)
{
    lh_usize_t n;

    n = lh_str_ptr_len(entry_name);
    lh_str_ptr_copy(state->pending, n, entry_name, n);
    state->pending[n] = '\0';
    state->type = type;
    state->ready = lh_bool_true;
}

static lh_bool_t
lh_os_fs_dir_next_posix(struct lh_os_fs_dir_state *state, lh_str_cptr *entry_name,
                        unsigned char *type)
{
    struct dirent *entry;

    if (state->ready)
    {
        *entry_name = state->pending;
        *type = state->type;
        return lh_bool_true;
    }
    errno = 0;
    entry = readdir(state->dir);
    if (lh_null_eq(entry))
    {
        return lh_bool_false;
    }
    *entry_name = entry->d_name;
    *type = entry->d_type;
    return lh_bool_true;
}

static lh_ssize_t
lh_os_fs_dir_read_native(struct lh_os_fs_dir_state *state, lh_os_fs_path_t *name,
                         lh_os_fs_dir_entry_kind_t *kind)
{
    for (;;)
    {
        lh_str_cptr entry_name;
        unsigned char type;
        lh_ssize_t n;

        if (!lh_os_fs_dir_next_posix(state, lh_addr_of(entry_name), lh_addr_of(type)))
        {
            if (errno != 0)
            {
                lh_os_system_error_capture();
                return lh_cast_static(lh_ssize_t, -1);
            }
            return lh_os_fs_dir_eof(name);
        }
        if (lh_os_fs_dir_is_dot_name(entry_name))
        {
            state->ready = lh_bool_false;
            continue;
        }
        n = lh_os_fs_dir_take_name(state, entry_name, lh_os_fs_dir_kind_posix(type), name, kind);
        if (n < 0 && lh_str_ptr_len(entry_name) <= LH_OS_FS_DIR_NAME_MAX && !state->ready)
        {
            lh_os_fs_dir_stash(state, entry_name, type);
        }
        return n;
    }
}
#endif

lh_ssize_t
lh_os_fs_dir_read(lh_os_fs_dir_t *self, lh_os_fs_path_t *name, lh_os_fs_dir_entry_kind_t *kind)
{
    lh_assert_runtime_ref(name);
    if (!lh_os_fs_dir_is_valid(self))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_not_open,
                             lh_os_error_desc_lit("directory is not open")));
        return lh_cast_static(lh_ssize_t, -1);
    }
    return lh_os_fs_dir_read_native(lh_os_fs_dir_get_state(self), name, kind);
}
