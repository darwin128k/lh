#include <lh/os/system/fs/dir.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/char/slash.h>
#include <lh/null.h>
#include <lh/os/alloc.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/win/fs/kind.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/runtime/allocator.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

struct lh_os_system_fs_dir_state
{
    lh_os_system_win_handle_t find;
    lh_os_system_win_find_data_t data;
    lh_bool_t ready; /* data holds an entry not yet reported (FindFirstFile's). */
};

LH_ATTRIBUTE_STATIC
struct lh_os_system_fs_dir_state *
lh_os_system_fs_dir_state(lh_os_system_fs_dir_handle_t handle)
{
    lh_assert_runtime_ref(handle);
    return lh_ptr_cast(struct lh_os_system_fs_dir_state, handle);
}

lh_os_system_fs_dir_handle_t
lh_os_system_fs_dir_open(lh_str_cptr path)
{
    lh_str_t pattern;

    lh_assert_runtime_ref(path);
    struct lh_os_system_fs_dir_state *const state =
        lh_ptr_cast(struct lh_os_system_fs_dir_state, lh_os_alloc(sizeof(*state)));
    if (lh_null_eq(state))
    {
        return LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID;
    }

    /* FindFirstFile lists a pattern, not a directory: "<path>\*". */
    const lh_str_view_t view = lh_str_view_make(path);
    lh_str_init_by_view(lh_addr_of(pattern), view);
    if (!lh_str_view_is_empty(lh_addr_of(view)) && !lh_char_is_slash(lh_str_view_get_last_char(lh_addr_of(view))) &&
        !lh_char_is_backslash(lh_str_view_get_last_char(lh_addr_of(view))))
    {
        lh_str_push_back(lh_addr_of(pattern), lh_char_map_backslash);
    }
    lh_str_push_back(lh_addr_of(pattern), '*');

    state->find = FindFirstFileA(lh_str_get_data(lh_addr_of(pattern)), lh_addr_of(state->data));
    lh_str_deinit(lh_addr_of(pattern));
    if (lh_math_eq(state->find, LH_OS_SYSTEM_WIN_INVALID_HANDLE))
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
    struct lh_os_system_fs_dir_state *const state = lh_os_system_fs_dir_state(handle);

    (void)FindClose(state->find);
    lh_runtime_allocator_free(state);
}

lh_ssize_t
lh_os_system_fs_dir_read(lh_os_system_fs_dir_handle_t handle, lh_str_cptr *name, lh_fs_kind_t *kind)
{
    lh_assert_runtime_ref(name);
    lh_assert_runtime_ref(kind);
    struct lh_os_system_fs_dir_state *const state = lh_os_system_fs_dir_state(handle);

    if (!state->ready)
    {
        if (!FindNextFileA(state->find, lh_addr_of(state->data)))
        {
            if (lh_math_eq(GetLastError(), LH_OS_SYSTEM_WIN_ERROR_NO_MORE_FILES))
            {
                return 0;
            }
            lh_os_system_error_capture();
            return -1;
        }
    }
    state->ready = lh_bool_false;
    lh_ptr_deref(name) = state->data.cFileName;
    lh_ptr_deref(kind) = lh_os_system_fs_kind_from_attrs(
        state->data.dwFileAttributes,
        lh_os_system_fs_is_symlink_tag(state->data.dwFileAttributes, state->data.dwReserved0));
    return lh_cast_static(lh_ssize_t, lh_str_ptr_len(state->data.cFileName));
}

lh_usize_t
lh_os_system_fs_dir_name_max(void)
{
    return lh_cast_static(lh_usize_t, lh_math_sub(LH_OS_SYSTEM_WIN_MAX_PATH, 1));
}
