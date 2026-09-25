#include <lh/os/fs/path.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/fs/path.h>

lh_str_cptr
lh_os_fs_path_to_cstr(const lh_fs_path_t *path, lh_str_t *scratch)
{
    if (lh_fs_path_is_empty(path))
    {
        lh_str_init(scratch);
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty, lh_os_error_desc_lit("path is empty")));
        return lh_null;
    }
    return lh_fs_path_to_cstr(path, lh_os_system_fs_path_style_native(), scratch);
}
