#include <lh/str/line/reader.h>
#include <lh/assert.h>
#include <lh/str/split/line.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

const lh_str_view_t *
lh_str_line_reader_get_text(const lh_str_line_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_void
lh_str_line_reader_set_text(lh_str_line_reader_t *self, lh_str_view_t text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

lh_usize_t *
lh_str_line_reader_get_pos_mut(lh_str_line_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pos);
}

lh_void
lh_str_line_reader_set_pos(lh_str_line_reader_t *self, lh_usize_t pos)
{
    lh_assert_runtime_ref(self);
    self->pos = pos;
}

lh_usize_t
lh_str_line_reader_get_number(const lh_str_line_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->number;
}

lh_void
lh_str_line_reader_set_number(lh_str_line_reader_t *self, lh_usize_t number)
{
    lh_assert_runtime_ref(self);
    self->number = number;
}

/* ── read ────────────────────────────────────────────────────────────────── */

lh_void
lh_str_line_reader_init(lh_str_line_reader_t *self, lh_str_view_t text)
{
    lh_str_line_reader_set_text(self, text);
    lh_str_line_reader_set_pos(self, 0U);
    lh_str_line_reader_set_number(self, 0U);
}

lh_bool_t
lh_str_line_reader_next(lh_str_line_reader_t *self, lh_str_view_t *line)
{
    if (!lh_str_view_split_next_line(lh_str_line_reader_get_text(self),
                                     lh_str_line_reader_get_pos_mut(self), line))
    {
        return lh_bool_false;
    }
    lh_str_line_reader_set_number(self, lh_math_add_one(lh_str_line_reader_get_number(self)));
    return lh_bool_true;
}
