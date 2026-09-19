#include <lh/os/fs/dir.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/fs/path.h>
#include <lh/runtime/allocator.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    ifndef IO_REPARSE_TAG_SYMLINK
#        define IO_REPARSE_TAG_SYMLINK 0xA000000CUL
#    endif
#else
#    include <dirent.h>
#    include <errno.h>
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

static void
lh_os_fs_dir_fail_null(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("path is null"));
}

static void
lh_os_fs_dir_fail_empty(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("path is empty"));
}

static void
lh_os_fs_dir_fail_too_small(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("name buffer is too small"));
}

static void
lh_os_fs_dir_fail_too_long(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("name is too long"));
}

static void
lh_os_fs_dir_fail_not_open(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("directory is not open"));
}

static void
lh_os_fs_dir_fail_oom(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("out of memory"));
}

static lh_bool_t
lh_os_fs_dir_is_dot(lh_str_cptr name)
{
    return lh_cast_static(lh_bool_t, name[0] == '.' &&
                                         (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')));
}

static struct lh_os_fs_dir_state *
lh_os_fs_dir_state(lh_os_fs_dir_t *self)
{
    return lh_ptr_cast(struct lh_os_fs_dir_state, self->handle);
}

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

void
lh_os_fs_dir_init(lh_os_fs_dir_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_FS_DIR_HANDLE_INVALID;
}

lh_bool_t
lh_os_fs_dir_open(lh_os_fs_dir_t *self, lh_str_cptr path)
{
    struct lh_os_fs_dir_state *state;

    lh_assert_runtime_ref(self);

    if (lh_null_eq(path))
    {
        lh_os_fs_dir_fail_null();
        return lh_bool_false;
    }
    if (path[0] == '\0')
    {
        lh_os_fs_dir_fail_empty();
        return lh_bool_false;
    }

    state = lh_ptr_cast(struct lh_os_fs_dir_state, lh_runtime_allocator_alloc(sizeof(*state)));
    if (lh_null_eq(state))
    {
        lh_os_fs_dir_fail_oom();
        return lh_bool_false;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        lh_char_t pattern[4096];

        if (!lh_os_fs_path_join(pattern, sizeof(pattern), path, "*"))
        {
            lh_runtime_allocator_free(state);
            return lh_bool_false;
        }

        state->find = FindFirstFileA(pattern, lh_addr_of(state->data));
        if (state->find == INVALID_HANDLE_VALUE)
        {
            lh_os_capture_last_error();
            lh_runtime_allocator_free(state);
            return lh_bool_false;
        }
        state->ready = lh_bool_true;
    }
#else
    state->dir = opendir(path);
    if (lh_null_eq(state->dir))
    {
        lh_os_capture_last_error();
        lh_runtime_allocator_free(state);
        return lh_bool_false;
    }
    state->ready = lh_bool_false;
#endif

    self->handle = state;
    return lh_bool_true;
}

void
lh_os_fs_dir_close(lh_os_fs_dir_t *self)
{
    struct lh_os_fs_dir_state *state;

    lh_assert_runtime_ref(self);

    if (lh_null_eq(self->handle))
    {
        return;
    }

    state = lh_os_fs_dir_state(self);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    FindClose(state->find);
#else
    (void)closedir(state->dir);
#endif
    lh_runtime_allocator_free(state);
    self->handle = LH_OS_FS_DIR_HANDLE_INVALID;
}

lh_os_fs_dir_handle_t
lh_os_fs_dir_get_handle(const lh_os_fs_dir_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_bool_t
lh_os_fs_dir_is_valid(const lh_os_fs_dir_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_bool_t, lh_null_ne(self->handle));
}

static lh_ssize_t
lh_os_fs_dir_copy_name(lh_str_cptr name, lh_str_ptr out, lh_usize_t out_size,
                       lh_os_fs_dir_entry_kind_t kind, lh_os_fs_dir_entry_kind_t *kind_out)
{
    lh_usize_t n;

    n = lh_str_ptr_len(name);
    if (n > LH_OS_FS_DIR_NAME_MAX)
    {
        lh_os_fs_dir_fail_too_long();
        return -1;
    }
    if (n + 1U > out_size)
    {
        lh_os_fs_dir_fail_too_small();
        return -1;
    }
    lh_str_ptr_copy(out, n, name, n);
    out[n] = '\0';
    if (lh_null_ne(kind_out))
    {
        *kind_out = kind;
    }
    return lh_cast_static(lh_ssize_t, n);
}

lh_ssize_t
lh_os_fs_dir_read(lh_os_fs_dir_t *self, lh_str_ptr out, lh_usize_t out_size,
                  lh_os_fs_dir_entry_kind_t *kind)
{
    struct lh_os_fs_dir_state *state;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);

    if (lh_null_eq(self->handle))
    {
        lh_os_fs_dir_fail_not_open();
        return -1;
    }

    state = lh_os_fs_dir_state(self);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    for (;;)
    {
        lh_ssize_t n;

        if (!state->ready)
        {
            if (!FindNextFileA(state->find, lh_addr_of(state->data)))
            {
                if (GetLastError() == ERROR_NO_MORE_FILES)
                {
                    return 0;
                }
                lh_os_capture_last_error();
                return -1;
            }
            state->ready = lh_bool_true;
        }
        if (lh_os_fs_dir_is_dot(state->data.cFileName))
        {
            state->ready = lh_bool_false;
            continue;
        }
        n = lh_os_fs_dir_copy_name(state->data.cFileName, out, out_size,
                                   lh_os_fs_dir_kind_win(lh_addr_of(state->data)), kind);
        if (n >= 0)
        {
            state->ready = lh_bool_false;
        }
        else if (lh_str_ptr_len(state->data.cFileName) > LH_OS_FS_DIR_NAME_MAX)
        {
            state->ready = lh_bool_false;
        }
        return n;
    }
#else
    for (;;)
    {
        lh_str_cptr name;
        unsigned char type;
        lh_ssize_t n;

        if (!state->ready)
        {
            struct dirent *entry;

            errno = 0;
            entry = readdir(state->dir);
            if (lh_null_eq(entry))
            {
                if (errno != 0)
                {
                    lh_os_capture_last_error();
                    return -1;
                }
                return 0;
            }
            name = entry->d_name;
            type = entry->d_type;
        }
        else
        {
            name = state->pending;
            type = state->type;
        }

        if (lh_os_fs_dir_is_dot(name))
        {
            state->ready = lh_bool_false;
            continue;
        }

        n = lh_os_fs_dir_copy_name(name, out, out_size, lh_os_fs_dir_kind_posix(type), kind);
        if (n >= 0)
        {
            state->ready = lh_bool_false;
            return n;
        }
        if (lh_str_ptr_len(name) > LH_OS_FS_DIR_NAME_MAX)
        {
            state->ready = lh_bool_false;
            return n;
        }
        if (!state->ready)
        {
            lh_usize_t pn = lh_str_ptr_len(name);

            lh_str_ptr_copy(state->pending, pn, name, pn);
            state->pending[pn] = '\0';
            state->type = type;
            state->ready = lh_bool_true;
        }
        return n;
    }
#endif
}
