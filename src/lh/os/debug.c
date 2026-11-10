#include <lh/os/debug.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/char/map.h>
#include <lh/core/str/format/text.h>
#include <lh/core/util/return.h>
#include <lh/core/util/str/ptr.h>

/* Room for one record on the debugger: the level, the text and the line end. */
#define LH_OS_DEBUG_LINE_MAX 1024U

lh_bool_t
lh_os_debug_print_line(lh_str_cptr text)
{
    const lh_char_t line_end[3] = {lh_char_map_cr, lh_char_map_lf, lh_char_map_nul};

    lh_return_if(lh_null_eq(text), lh_bool_false);
    return lh_os_system_debug_print(text) && lh_os_system_debug_print(line_end) ? lh_bool_true
                                                                                : lh_bool_false;
}

lh_ssize_t
lh_os_debug_log(lh_self_ptr self, lh_logger_level_t level, lh_str_cptr fmt, va_list args)
{
    lh_char_t text[LH_OS_DEBUG_LINE_MAX];
    lh_char_t line[LH_OS_DEBUG_LINE_MAX + 16U];
    lh_usize_t size;

    LH_ATTRIBUTE_UNUSED(self);
    size = lh_str_ptr_format_cstr_v(text, sizeof(text), fmt, args);
    size = lh_str_ptr_format_cstr(line, sizeof(line), "[%s] %s", lh_logger_level_get_name(level),
                                  lh_math_is_zero(size) ? fmt : text);
    lh_return_ifn(lh_os_debug_print_line(line), -1);
    return lh_cast_static(lh_ssize_t, size);
}
