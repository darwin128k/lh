/**
 * @file local.h
 * @brief Shared pieces of the `lh_os_fs_path_*` implementation. Not public.
 */

#ifndef LH_OS_FS_PATH_LOCAL_H
#define LH_OS_FS_PATH_LOCAL_H

#include <lh/os/fs/path.h>
#include <lh/assert.h>
#include <lh/attribute/force_inline.h>
#include <lh/compiler/os.h>
#include <lh/char/slash.h>
#include <lh/os.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>

typedef struct
{
    lh_usize_t offset;
    lh_usize_t size;
} lh_os_fs_path_span_t;

LH_ATTRIBUTE_FORCE_INLINE
lh_str_cptr
lh_os_fs_path_cstr(const lh_os_fs_path_t *self)
{
    return lh_str_get_data(lh_os_fs_path_get_text_as_const(self));
}

LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_os_fs_path_require(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_os_fs_path_is_empty(self))
    {
        lh_os_set_last_error(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty"));
        return lh_bool_false;
    }
    return lh_bool_true;
}

LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_os_fs_path_is_sep(lh_char_t ch)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return (lh_char_is_backslash(ch) || lh_char_is_slash(ch)) ? lh_bool_true : lh_bool_false;
#else
    return lh_char_is_slash(ch);
#endif
}

LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_os_fs_path_is_sep_at(const lh_str_view_t *text, lh_usize_t i)
{
    return lh_os_fs_path_is_sep(lh_str_view_get_char_from_begin(text, i));
}

#endif /* LH_OS_FS_PATH_LOCAL_H */
