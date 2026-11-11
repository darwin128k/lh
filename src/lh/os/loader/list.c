#include <lh/os/loader/list.h>
#include <lh/core/assert.h>
#include <lh/core/char/map.h>
#include <lh/core/math.h>
#include <lh/core/str/line/reader.h>
#include <lh/os/system/name.h>

/* The characters a whole-line comment starts with. */
static const lh_char_t m_os_loader_list_comments[] = {lh_char_map_semicolon, lh_char_map_hash};

lh_bool_t
lh_os_loader_list_is_skipped(const lh_str_view_t *line)
{
    return lh_cast_static(
        lh_bool_t, lh_str_view_is_empty(line) ||
                       lh_str_ptr_contains_char(m_os_loader_list_comments,
                                                lh_array_ptr_get_size(m_os_loader_list_comments),
                                                lh_str_view_get_first_char(line)));
}

lh_bool_t
lh_os_loader_list_try_get_section(const lh_str_view_t *line, lh_str_view_t *out)
{
    lh_assert_runtime_ref(out);

    const lh_usize_t size = lh_str_view_get_size(line);

    if (lh_math_lt(size, 2U) ||
        lh_math_ne(lh_str_view_get_first_char(line), lh_char_map_lbracket) ||
        lh_math_ne(lh_str_view_get_last_char(line), lh_char_map_rbracket))
    {
        return lh_bool_false;
    }

    lh_str_view_t name;

    lh_str_view_from_offset(line, 1U, lh_math_sub(size, 2U), lh_addr_of(name));
    lh_str_view_trim(lh_addr_of(name));

    const lh_str_view_t brackets = lh_str_view_lit("[]");

    if (lh_str_view_is_empty(lh_addr_of(name)) ||
        lh_str_view_contains_of(lh_addr_of(name), lh_addr_of(brackets)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = name;
    return lh_bool_true;
}

lh_bool_t
lh_os_loader_list_is_section_active(const lh_str_view_t *section)
{
    if (lh_str_view_is_empty(section))
    {
        return lh_bool_true;
    }

    lh_str_view_t system;

    lh_os_system_get_name(lh_addr_of(system));

    const lh_str_view_t any = lh_str_view_lit("any");
    const lh_str_view_t modules = lh_str_view_lit("modules");

    return lh_cast_static(lh_bool_t,
                          lh_str_view_equals(section, lh_addr_of(system), lh_bool_true) ||
                              lh_str_view_equals(section, lh_addr_of(any), lh_bool_true) ||
                              lh_str_view_equals(section, lh_addr_of(modules), lh_bool_true));
}

lh_bool_t
lh_os_loader_list_try_read(const lh_str_view_t *text, lh_os_loader_list_cb cb, lh_ptr user,
                           lh_usize_t *bad_line)
{
    lh_assert_runtime_ref(text);
    lh_assert_runtime_ref(cb);
    lh_assert_runtime_ref(bad_line);

    lh_str_view_t rest = lh_ptr_deref(text);

    lh_str_view_skip_bom(lh_addr_of(rest));

    lh_str_line_reader_t reader;

    lh_str_line_reader_init(lh_addr_of(reader), rest);

    lh_str_view_t section;

    lh_str_view_init_empty(lh_addr_of(section));

    lh_str_view_t line;

    while (lh_str_line_reader_next(lh_addr_of(reader), lh_addr_of(line)))
    {
        lh_str_view_trim(lh_addr_of(line));
        if (lh_os_loader_list_is_skipped(lh_addr_of(line)))
        {
            continue;
        }
        if (lh_math_eq(lh_str_view_get_first_char(lh_addr_of(line)), lh_char_map_lbracket))
        {
            if (!lh_os_loader_list_try_get_section(lh_addr_of(line), lh_addr_of(section)))
            {
                lh_ptr_deref(bad_line) = lh_str_line_reader_get_number(lh_addr_of(reader));
                return lh_bool_false;
            }
            continue;
        }
        if (lh_os_loader_list_is_section_active(lh_addr_of(section)))
        {
            cb(user, lh_addr_of(line));
        }
    }
    return lh_bool_true;
}
