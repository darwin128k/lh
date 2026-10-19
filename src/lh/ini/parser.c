#include <lh/ini/parser.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/char/map.h>
#include <lh/math/ops.h>
#include <lh/null.h>
#include <lh/str/split/once.h>
#include <lh/util/array/ptr.h>
#include <lh/util/str/ptr.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_ATTRIBUTE_STATIC
lh_str_line_reader_t *
lh_ini_parser_get_reader_mut(lh_ini_parser_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->reader);
}

lh_usize_t
lh_ini_parser_get_line(const lh_ini_parser_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_str_line_reader_get_number(lh_addr_of(self->reader));
}

lh_str_view_t
lh_ini_parser_get_section(const lh_ini_parser_t *self)
{
    lh_assert_runtime_ref(self);
    return self->section;
}

LH_ATTRIBUTE_STATIC
lh_void
lh_ini_parser_set_section(lh_ini_parser_t *self, lh_str_view_t section)
{
    lh_assert_runtime_ref(self);
    self->section = section;
}

lh_ini_error_t
lh_ini_parser_get_error(const lh_ini_parser_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error;
}

LH_ATTRIBUTE_STATIC
lh_void
lh_ini_parser_set_error(lh_ini_parser_t *self, lh_ini_error_t error)
{
    lh_assert_runtime_ref(self);
    self->error = error;
}

/* Stop on @p error: this and every later next returns false. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_ini_parser_fail(lh_ini_parser_t *self, lh_ini_error_t error)
{
    lh_ini_parser_set_error(self, error);
    return lh_bool_false;
}

lh_bool_t
lh_ini_parser_has_error(const lh_ini_parser_t *self)
{
    return lh_cast_static(lh_bool_t, lh_math_ne(lh_ini_parser_get_error(self), lh_ini_error_none));
}

/* ── init ────────────────────────────────────────────────────────────────── */

lh_void
lh_ini_parser_init(lh_ini_parser_t *self, lh_str_view_t text)
{
    lh_str_view_skip_bom(lh_addr_of(text));
    lh_str_line_reader_init(lh_ini_parser_get_reader_mut(self), text);
    lh_ini_parser_set_section(self, lh_str_view_empty());
    lh_ini_parser_set_error(self, lh_ini_error_none);
}

/* ── lines ───────────────────────────────────────────────────────────────── */

/* What starts a whole-line comment. */
static const lh_char_t m_ini_parser_comments[] = {lh_char_map_semicolon, lh_char_map_hash};

/* An empty line or a whole-line comment: nothing to read. @p line is trimmed. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_ini_parser_is_skipped(const lh_str_view_t *line)
{
    return lh_cast_static(lh_bool_t,
                          lh_str_view_is_empty(line) ||
                              lh_str_ptr_contains_char(m_ini_parser_comments,
                                                       lh_array_ptr_get_size(m_ini_parser_comments),
                                                       lh_str_view_get_first_char(line)));
}

/* `[name]`: @p line is trimmed and starts with `[`. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_ini_parser_read_section(lh_ini_parser_t *self, const lh_str_view_t *line,
                           lh_ini_entry_t *entry)
{
    const lh_usize_t size = lh_str_view_get_size(line);
    lh_str_view_t name;
    const lh_str_view_t brackets = lh_str_view_lit("[]");

    if (lh_math_lt(size, 2U) || lh_math_ne(lh_str_view_get_last_char(line), lh_char_map_rbracket))
    {
        return lh_ini_parser_fail(self, lh_ini_error_bad_section);
    }

    lh_str_view_from_offset(line, 1U, lh_math_sub(size, 2U), lh_addr_of(name));

    lh_str_view_trim(lh_addr_of(name));
    if (lh_str_view_is_empty(lh_addr_of(name)))
    {
        return lh_ini_parser_fail(self, lh_ini_error_empty_section);
    }

    if (lh_str_view_contains_of(lh_addr_of(name), lh_addr_of(brackets)))
    {
        return lh_ini_parser_fail(self, lh_ini_error_bad_section);
    }
    lh_ini_parser_set_section(self, name);
    lh_ptr_deref(entry) = lh_ini_entry_make_section(name);
    return lh_bool_true;
}

/* `key = value`: @p line is trimmed, not empty and not a header. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_ini_parser_read_pair(lh_ini_parser_t *self, const lh_str_view_t *line, lh_ini_entry_t *entry)
{
    lh_str_view_t key;
    lh_str_view_t value;
    lh_str_view_t section;

    if (!lh_str_view_split_once(line, lh_char_map_equals, lh_addr_of(key), lh_addr_of(value)))
    {
        return lh_ini_parser_fail(self, lh_ini_error_missing_equals);
    }
    lh_str_view_trim(lh_addr_of(key));
    if (lh_str_view_is_empty(lh_addr_of(key)))
    {
        return lh_ini_parser_fail(self, lh_ini_error_empty_key);
    }

    section = lh_ini_parser_get_section(self);

    /* A header never has an empty name, so an empty section is "none yet". */
    if (lh_str_view_is_empty(lh_addr_of(section)))
    {
        return lh_ini_parser_fail(self, lh_ini_error_no_section);
    }

    lh_str_view_trim(lh_addr_of(value));
    lh_ptr_deref(entry) = lh_ini_entry_make_pair(section, key, value);
    return lh_bool_true;
}

lh_bool_t
lh_ini_parser_next(lh_ini_parser_t *self, lh_ini_entry_t *entry)
{
    lh_str_view_t line;

    lh_assert_runtime_ref(entry);
    if (lh_ini_parser_has_error(self))
    {
        return lh_bool_false;
    }

    while (lh_str_line_reader_next(lh_ini_parser_get_reader_mut(self), lh_addr_of(line)))
    {
        lh_str_view_trim(lh_addr_of(line));
        if (lh_ini_parser_is_skipped(lh_addr_of(line)))
        {
            continue;
        }
        return lh_math_eq(lh_str_view_get_first_char(lh_addr_of(line)), lh_char_map_lbracket)
                   ? lh_ini_parser_read_section(self, lh_addr_of(line), entry)
                   : lh_ini_parser_read_pair(self, lh_addr_of(line), entry);
    }
    return lh_bool_false;
}
