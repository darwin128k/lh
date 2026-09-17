/**
 * @file next.h
 * @brief Walk delimiter-separated fields in a buffer, one at a time.
 *
 * The one piece shared between anything that currently hand-rolls
 * "find the next delimiter, measure the field, advance the cursor"
 * (::lh_net_ip4_parse, ::lh_net_ip4_socket_addr_parse) — built on
 * ::lh_str_ptr_find_of_char in `lh/util/str/ptr.h`, not a second scanner.
 */

#ifndef LH_STR_SPLIT_NEXT_H
#define LH_STR_SPLIT_NEXT_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Extract the next @p delim-separated field starting at `*pos`.
 *
 * The field is everything from `*pos` up to (but not including) the next
 * @p delim, or up to @p str_size if @p delim does not occur again. `*pos`
 * is advanced past the field and its delimiter (if any), ready for the
 * next call — so calling this in a loop until it returns
 * ::lh_bool_false walks every field in @p str.
 *
 * @param str        Buffer to scan.
 * @param str_size   Total number of characters in @p str.
 * @param delim      Delimiter character.
 * @param pos        In/out cursor. Start at `0` for the first field.
 * @param field      Out: start of the field found.
 * @param field_size Out: length of the field found (`0` for an empty
 *                   field, e.g. two delimiters in a row).
 * @param had_delim  Out: ::lh_bool_true if a @p delim terminated this
 *                   field; ::lh_bool_false if the field ran to
 *                   @p str_size instead (nothing left to split after it).
 *
 * @return ::lh_bool_true if a field was produced, ::lh_bool_false if
 *         `*pos` was already at @p str_size (nothing left to read).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_ptr_split_next(lh_str_cptr str, lh_usize_t str_size, lh_char_t delim, lh_usize_t *pos,
                      lh_str_cptr *field, lh_usize_t *field_size, lh_bool_t *had_delim);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_SPLIT_NEXT_H */
