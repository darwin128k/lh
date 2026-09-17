/**
 * @file next.h
 * @brief Walk delimiter-separated fields in a buffer, one at a time.
 *
 * The one piece shared between anything that currently hand-rolls
 * "find the next delimiter, measure the field, advance the cursor"
 * (::lh_net_ip4_parse, ::lh_net_ip4_socket_addr_parse) — built on the
 * existing ::lh_str_ptr_find_of_char, not a reimplementation of it.
 *
 * ::LH_ATTRIBUTE_FORCE_INLINE — called once per field from a force-inlined
 * caller; see the note in `net/ip.c`.
 */

#ifndef LH_STR_SPLIT_NEXT_H
#define LH_STR_SPLIT_NEXT_H

#include <lh/assert.h>
#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/null.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/util/str/ptr.h>

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
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_split_next(lh_str_cptr str, lh_usize_t str_size, lh_char_t delim, lh_usize_t *pos,
                      lh_str_cptr *field, lh_usize_t *field_size, lh_bool_t *had_delim)
{
    lh_usize_t start;
    lh_str_cptr delim_at;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(pos);
    lh_assert_runtime_ref(field);
    lh_assert_runtime_ref(field_size);
    lh_assert_runtime_ref(had_delim);

    start = *pos;
    if (start >= str_size)
    {
        return lh_bool_false;
    }

    delim_at = lh_str_ptr_find_of_char(str + start, str_size - start, delim);

    *field = str + start;
    if (delim_at != lh_null)
    {
        *field_size = lh_cast_static(lh_usize_t, (delim_at - (str + start)));
        *had_delim = lh_bool_true;
        *pos = start + *field_size + 1U;
    }
    else
    {
        *field_size = str_size - start;
        *had_delim = lh_bool_false;
        *pos = str_size;
    }

    return lh_bool_true;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_SPLIT_NEXT_H */
