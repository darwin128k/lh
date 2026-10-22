/**
 * @file minify.h
 * @brief Squeeze a JSON text in place (::lh_json_minify), as cJSON's
 *        `cJSON_Minify` does.
 *
 * Works on the text, not on a tree, so it needs no memory and no valid
 * JSON: whitespace outside strings goes, and so do `//` line comments and
 * `/ * * /` block comments (which JSON has not, but hand-written config
 * files do), so a commented config can be minified and then read. Strings
 * are copied exactly, escapes included.
 */

#ifndef LH_JSON_MINIFY_H
#define LH_JSON_MINIFY_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/** True when the @p size characters at @p text hold @p first then @p second at @p pos. */
lh_bool_t
lh_json_minify_is_at(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos, lh_char_t first,
                     lh_char_t second);

/** The position of the line feed that ends the `//` comment at @p pos, or @p size. */
lh_usize_t
lh_json_minify_skip_line_comment(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos);

/** The position right after the `* /` that ends the block comment at @p pos, or @p size. */
lh_usize_t
lh_json_minify_skip_block_comment(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos);

/**
 * @brief Copy the string that starts at @p read — quotes, escapes and all —
 *        to @p write, and move both past it.
 */
lh_void
lh_json_minify_copy_string(lh_str_ptr text, lh_usize_t size, lh_usize_t *read, lh_usize_t *write);

/**
 * @brief Minify the @p size characters at @p text in place.
 *
 * An unterminated string or block comment runs to the end of the text, as
 * in cJSON: the text was broken before, and the reader will say so.
 *
 * @param text Characters to squeeze; overwritten from the front.
 * @param size How many there are.
 * @return How many characters are left (no NUL is written).
 */
lh_usize_t
lh_json_minify(lh_str_ptr text, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_JSON_MINIFY_H */
