#include <lh/os/system/fs/dir.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/char/slash.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/error/capture.h>
#include <lh/runtime/allocator.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifndef IO_REPARSE_TAG_SYMLINK
#    define IO_REPARSE_TAG_SYMLINK 0xA000000CUL
#endif

struct lh_os_system_fs_dir_state
{
    HANDLE find;
    WIN32_FIND_DATAA data;
    lh_bool_t ready; /* data holds an entry not yet reported (FindFirstFile's). */
};

LH_ATTRIBUTE_STATIC
struct lh_os_system_fs_dir_state *
lh_os_system_fs_dir_state(lh_os_system_fs_dir_handle_t handle)
{
    lh_assert_runtime_ref(handle);
    return lh_ptr_cast(struct lh_os_system_fs_dir_state, handle);
}

LH_ATTRIBUTE_STATIC
lh_fs_kind_t
lh_os_system_fs_dir_kind(const WIN32_FIND_DATAA *data)
{
    if (!lh_math_is_zero(lh_bit_and(data->dwFileAttributes, FILE_ATTRIBUTE_REPARSE_POINT)) &&
        lh_math_eq(data->dwReserved0, IO_REPARSE_TAG_SYMLINK))
    {
        return lh_fs_kind_symlink;
    }
    if (!lh_math_is_zero(lh_bit_and(data->dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY)))
    {
        return lh_fs_kind_dir;
    }
    return lh_fs_kind_file;
}

lh_os_system_fs_dir_handle_t
lh_os_system_fs_dir_open(lh_str_cptr path)
{
    struct lh_os_system_fs_dir_state *state;
    lh_str_view_t view;
    lh_str_t pattern;

    lh_assert_runtime_ref(path);
    state = lh_ptr_cast(struct lh_os_system_fs_dir_state, lh_runtime_allocator_alloc(sizeof(*state)));
    if (lh_null_eq(state))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_out_of_memory, lh_os_error_desc_lit("out of memory")));
        return LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID;
    }

    /* FindFirstFile lists a pattern, not a directory: "<path>\*". */
    view = lh_str_view_make(path);
    lh_str_init(lh_addr_of(pattern));
    lh_str_append_view(lh_addr_of(pattern), view);
    if (!lh_str_view_is_empty(lh_addr_of(view)) && !lh_char_is_slash(lh_str_view_get_last_char(lh_addr_of(view))) &&
        !lh_char_is_backslash(lh_str_view_get_last_char(lh_addr_of(view))))
    {
        lh_str_push_back(lh_addr_of(pattern), lh_char_map_backslash);
    }
    lh_str_push_back(lh_addr_of(pattern), '*');

    state->find = FindFirstFileA(lh_str_get_data(lh_addr_of(pattern)), lh_addr_of(state->data));
    lh_str_deinit(lh_addr_of(pattern));
    if (lh_math_eq(state->find, INVALID_HANDLE_VALUE))
    {
        lh_os_system_error_capture();
        lh_runtime_allocator_free(state);
        return LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID;
    }
    state->ready = lh_bool_true;
    return state;
}

void
lh_os_system_fs_dir_close(lh_os_system_fs_dir_handle_t handle)
{
    struct lh_os_system_fs_dir_state *state;

    state = lh_os_system_fs_dir_state(handle);
    (void)FindClose(state->find);
    lh_runtime_allocator_free(state);
}

lh_ssize_t
lh_os_system_fs_dir_read(lh_os_system_fs_dir_handle_t handle, lh_str_cptr *name, lh_fs_kind_t *kind)
{
    struct lh_os_system_fs_dir_state *state;

    lh_assert_runtime_ref(name);
    lh_assert_runtime_ref(kind);
    state = lh_os_system_fs_dir_state(handle);

    if (!state->ready)
    {
        if (!FindNextFileA(state->find, lh_addr_of(state->data)))
        {
            if (lh_math_eq(GetLastError(), ERROR_NO_MORE_FILES))
            {
                return 0;
            }
            lh_os_system_error_capture();
            return -1;
        }
    }
    state->ready = lh_bool_false;
    lh_ptr_deref(name) = state->data.cFileName;
    lh_ptr_deref(kind) = lh_os_system_fs_dir_kind(lh_addr_of(state->data));
    return lh_cast_static(lh_ssize_t, lh_str_ptr_len(state->data.cFileName));
}

lh_usize_t
lh_os_system_fs_dir_name_max(void)
{
    return lh_cast_static(lh_usize_t, lh_math_sub(MAX_PATH, 1));
}
