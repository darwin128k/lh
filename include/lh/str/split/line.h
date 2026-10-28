/**
 * @file line.h
 * @brief Walk the lines of a text, one at a time, whatever ends them.
 *
 * A line ends at `\n` (POSIX, macOS), `\r\n` (Windows) or a lone `\r`
 * (classic Mac OS); the three may be mixed in one text. `\r\n` is one end,
 * not two — which is why lines are not split with ::lh_str_ptr_split_next_of
 * and a `"\r\n"` delimiter set: that yields an empty field after every
 * Windows line.
 */

#ifndef LH_STR_SPLIT_LINE_H
#define LH_STR_SPLIT_LINE_H

#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Extract the line starting at `*pos`.
 *
 * The line is everything from `*pos` up to (but not including) the next
 * line end, or up to @p str_size if no end follows. `*pos` is advanced past
 * the line and its end, so calling this in a loop until it returns
 * ::lh_bool_false walks every line. A text that ends with a line end has no
 * empty line after it: `"a\n"` is one line, `"a\n\n"` is two (the second
 * empty).
 *
 * Example usage:
 * @code{.c}
 * lh_usize_t pos = 0;
 * lh_str_cptr line;
 * lh_usize_t line_size;
 * while (lh_str_ptr_split_next_line("a\r\nb\nc", 7, &pos, &line, &line_size)) {
 *     // "a", "b", "c"
 * }
 * @endcode
 *
 * @param str       Buffer to scan.
 * @param str_size  Total number of characters in @p str.
 * @param pos       In/out cursor. Start at `0` for the first line.
 * @param line      Out: start of the line found.
 * @param line_size Out: length of the line found, without its end (`0`
 *                  for an empty line).
 *
 * @return ::lh_bool_true if a line was produced, ::lh_bool_false if `*pos`
 *         was already at @p str_size (nothing left to read).
 */
lh_bool_t
lh_str_ptr_split_next_line(const lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                           const lh_str_ptr *line, lh_usize_t *line_size);

/**
 * @brief View wrapper for ::lh_str_ptr_split_next_line.
 *
 * Empty @p self yields ::lh_bool_false. An empty line is an empty view.
 */
lh_bool_t
lh_str_view_split_next_line(const lh_str_view_t *self, lh_usize_t *pos, lh_str_view_t *line);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_SPLIT_LINE_H */
