#include <lh/fs/path/style.h>
#include <lh/cast/static.h>
#include <lh/char/map.h>
#include <lh/char/slash.h>
#include <lh/util/math.h>

lh_bool_t
lh_fs_path_style_is_sep(lh_fs_path_style_t style, lh_char_t ch)
{
    if (lh_char_is_slash(ch))
    {
        return lh_bool_true;
    }
    return lh_cast_static(lh_bool_t,
                          lh_math_eq(style, lh_fs_path_style_windows) && lh_char_is_backslash(ch));
}

lh_char_t
lh_fs_path_style_get_sep(lh_fs_path_style_t style)
{
    return lh_math_eq(style, lh_fs_path_style_windows) ? lh_char_map_backslash : lh_char_map_slash;
}
