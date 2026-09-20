#include <lh/os/fs/path.h>
#include <lh/assert.h>
#include <lh/char/letter.h>
#include <lh/char/map.h>
#include <lh/char/slash.h>
#include <lh/compiler/os.h>
#include <lh/index.h>
#include <lh/memory/view.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>

void
lh_os_fs_path_init(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    self->root_kind = lh_os_fs_path_root_kind_none;
    self->root_drive = 0;
    lh_str_list_init(lh_addr_of(self->segments));
}

void
lh_os_fs_path_deinit(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    lh_str_list_deinit(lh_addr_of(self->segments));
}

void
lh_os_fs_path_clear(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    self->root_kind = lh_os_fs_path_root_kind_none;
    self->root_drive = 0;
    lh_str_list_clear(lh_os_fs_path_get_segments(self));
}

void
lh_os_fs_path_assign(lh_os_fs_path_t *self, const lh_os_fs_path_t *other)
{
    lh_assert_runtime_ref(other);
    if (self == other)
    {
        return;
    }
    self->root_kind = lh_os_fs_path_get_root_kind(other);
    self->root_drive = lh_os_fs_path_get_root_drive(other);
    lh_str_list_assign(lh_os_fs_path_get_segments(self), lh_os_fs_path_get_segments_as_const(other));
}

lh_os_fs_path_root_kind_t
lh_os_fs_path_get_root_kind(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root_kind;
}

lh_char_t
lh_os_fs_path_get_root_drive(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root_drive;
}

lh_str_list_t *
lh_os_fs_path_get_segments(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->segments);
}

const lh_str_list_t *
lh_os_fs_path_get_segments_as_const(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->segments);
}

lh_usize_t
lh_os_fs_path_get_segment_count(const lh_os_fs_path_t *self)
{
    return lh_str_list_get_size(lh_os_fs_path_get_segments_as_const(self));
}

lh_bool_t
lh_os_fs_path_is_empty(const lh_os_fs_path_t *self)
{
    return (lh_os_fs_path_get_root_kind(self) == lh_os_fs_path_root_kind_none &&
            lh_str_list_is_empty(lh_os_fs_path_get_segments_as_const(self)))
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_os_fs_path_is_absolute(const lh_os_fs_path_t *self)
{
    return (lh_os_fs_path_get_root_kind(self) != lh_os_fs_path_root_kind_none) ? lh_bool_true
                                                                                : lh_bool_false;
}

lh_bool_t
lh_os_fs_path_is_root(const lh_os_fs_path_t *self)
{
    return (lh_os_fs_path_is_absolute(self) && lh_os_fs_path_get_segment_count(self) == 0U)
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_os_fs_path_is_drive(lh_str_view_t part)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_str_view_get_size(lh_addr_of(part)) != 2U)
    {
        return lh_bool_false;
    }
    return (lh_char_is_letter(lh_str_view_get_char_from_begin(lh_addr_of(part), 0U)) &&
            lh_str_view_get_char_from_begin(lh_addr_of(part), 1U) == lh_char_map_colon)
               ? lh_bool_true
               : lh_bool_false;
#else
    (void)part;
    return lh_bool_false;
#endif
}

static lh_usize_t
lh_os_fs_path_take_drive(lh_os_fs_path_t *self, const lh_str_view_t *text, lh_usize_t n)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    lh_str_view_t drive;

    if (n < 2U)
    {
        return 0U;
    }
    drive = lh_memory_view_make_from_offset(text, 0U, 2U);
    if (!lh_os_fs_path_is_drive(drive))
    {
        return 0U;
    }
    self->root_kind = lh_os_fs_path_root_kind_drive;
    self->root_drive = lh_str_view_get_char_from_begin(lh_addr_of(drive), 0U);
    return (2U < n && lh_char_is_path_sep(lh_str_view_get_char_from_begin(text, 2U))) ? 3U : 2U;
#else
    (void)self;
    (void)text;
    (void)n;
    return 0U;
#endif
}

void
lh_os_fs_path_set(lh_os_fs_path_t *self, lh_str_view_t text)
{
    lh_usize_t n;
    lh_usize_t pos;
    lh_str_view_t piece;
    lh_bool_t had_delim;

    lh_os_fs_path_clear(self);
    if (lh_str_view_is_empty(lh_addr_of(text)))
    {
        return;
    }
    n = lh_str_view_get_size(lh_addr_of(text));
    pos = lh_os_fs_path_take_drive(self, lh_addr_of(text), n);
    if (pos == 0U && lh_char_is_path_sep(lh_str_view_get_char_from_begin(lh_addr_of(text), 0U)))
    {
        self->root_kind = lh_os_fs_path_root_kind_posix;
        pos = 1U;
    }
    {
        static const lh_char_t seps[] = {
            lh_char_map_slash,
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
            lh_char_map_backslash,
#endif
        };
        while (lh_str_view_split_next_of(lh_addr_of(text), seps, sizeof(seps) / sizeof(seps[0]),
                                         lh_addr_of(pos), lh_addr_of(piece), lh_addr_of(had_delim)))
        {
            if (!lh_str_view_is_empty(lh_addr_of(piece)))
            {
                lh_str_list_push_back(lh_os_fs_path_get_segments(self), piece);
            }
        }
    }
}

lh_bool_t
lh_os_fs_path_join(lh_os_fs_path_t *self, const lh_os_fs_path_t *dir, const lh_os_fs_path_t *name)
{
    lh_assert_runtime_ref(dir);
    lh_assert_runtime_ref(name);
    if (lh_os_fs_path_get_segment_count(name) == 0U)
    {
        lh_os_fs_path_clear(self);
        return lh_bool_false;
    }

    if (self == name)
    {
        lh_os_fs_path_t tmp;

        lh_os_fs_path_init(lh_addr_of(tmp));
        lh_os_fs_path_assign(lh_addr_of(tmp), name);
        if (self != dir)
        {
            lh_os_fs_path_assign(self, dir);
        }
        lh_str_list_append(lh_os_fs_path_get_segments(self),
                           lh_os_fs_path_get_segments_as_const(lh_addr_of(tmp)));
        lh_os_fs_path_deinit(lh_addr_of(tmp));
        return lh_bool_true;
    }

    if (self != dir)
    {
        lh_os_fs_path_assign(self, dir);
    }
    lh_str_list_append(lh_os_fs_path_get_segments(self), lh_os_fs_path_get_segments_as_const(name));
    return lh_bool_true;
}
