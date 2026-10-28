#include <lh/os/system/fs/path.h>
#include <lh/assert.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/error/capture.h>
#include <lh/util/str/ptr.h>
#include <stdlib.h>
#include <limits.h>

lh_fs_path_style_t
lh_os_system_fs_path_style_native(void)
{
    return lh_fs_path_style_posix;
}

lh_bool_t
lh_os_system_fs_path_canonical(lh_str_cptr path, lh_str_t *out)
{
    lh_assert_runtime_ref(path);
    lh_assert_runtime_ref(out);
    if (!*path)
    {
        lh_os_set_last_error_lit(lh_os_error_code_path_empty, "path is empty");
        return lh_bool_false;
    }
    lh_char_t resolved[PATH_MAX];
    if (lh_null_eq(realpath(path, resolved)))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_str_clear(out);
    lh_str_append(out, resolved, lh_str_ptr_len(resolved));
    return lh_bool_true;
}
