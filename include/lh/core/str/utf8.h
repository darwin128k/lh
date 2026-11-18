/**
 * @file utf8.h
 * @brief How many bytes a character takes in UTF-8, and its bytes.
 *
 * The encoding side of UTF-8, for anything that has to put a code point into
 * a buffer: a text field typing a character, a JSON `\u` escape. Reading a
 * code point back is the caller's business — a renderer walking a string
 * reads differently from a validator — but the size of a character by its
 * lead byte (::lh_str_utf8_get_lead_size) is the same question for all of
 * them.
 */

#ifndef LH_STR_UTF8_H
#define LH_STR_UTF8_H

#include <lh/core/attribute/force_inline.h>
#include <lh/core/bool.h>
#include <lh/core/size.h>
#include <lh/core/util/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How many bytes @p code takes in UTF-8: 0 when it is not encodable.
 *
 * A code point is encodable when it is not a surrogate (`0xD800..0xDFFF`), not
 * above `0x10FFFF`, and not `0` — which is the terminator, and writing it into a
 * buffer as if it were a character would end the text in the middle of it.
 */
lh_u32_t
lh_str_utf8_get_encoded_size(lh_u32_t code);

/**
 * @brief How many bytes the character whose **first byte** is @p lead takes in
 *        UTF-8: 1, 2, 3 or 4.
 *
 * Not the same question as ::lh_str_utf8_get_encoded_size, and answering one with the
 * other is a bug that only shows up outside ASCII. That one takes a **code point**; this
 * one takes the **lead byte**, which is what a walk over a buffer somebody else filled
 * in is actually holding.
 *
 * The two agree by coincidence for every two-byte character -- `0xD0` as a code point
 * is below `0x800` and answers 2, which is right for the Cyrillic letter that `0xD0`
 * starts -- so a suite written with Cyrillic in it stays green over the wrong call.
 * `0xE0` starts a **three**-byte character and is itself a code point below `0x800`,
 * so the other function answers 2 and a field deleting that character cuts two bytes
 * of three and leaves the rest of a code point to be read as the next character.
 *
 * One byte for anything that is not a lead byte: a continuation byte where a lead
 * byte should be is a broken string, and stepping over more than one byte would walk
 * past a character somebody can see.
 */
lh_u32_t
lh_str_utf8_get_lead_size(lh_byte_t lead);

/**
 * @brief Whether @p byte continues a character (`10xxxxxx`) rather than starting
 *        one.
 */
lh_bool_t
lh_str_utf8_is_continuation(lh_byte_t byte);

/**
 * @brief How many bytes the character that **ends** at byte @p end of @p text
 *        takes: 1, 2, 3 or 4.
 *
 * The walk backwards that ::lh_str_utf8_get_lead_size cannot do: from @p end it
 * steps back over continuation bytes to the lead byte, never before the start of
 * the text and never over more than three. What a cursor does when it moves left,
 * and what a backspace deletes.
 *
 * @param text The text; must not be null.
 * @param end  A byte offset in @p text that ends a character; at least `1`.
 * @return The size of that character; `1` for a broken string, so a walk over
 *         one moves a byte at a time.
 */
lh_u32_t
lh_str_utf8_get_prev_size(const lh_char_t *text, lh_usize_t end);

/**
 * @brief Write @p code into @p out as UTF-8 and return how many bytes it took.
 *
 * Writes nothing and returns `0` when @p code is not encodable or @p bytes is too
 * few, so a buffer that is one byte short gets a refused character rather than half
 * of one — half a code point is not a character, and the next read over it would
 * walk off the end of the text.
 */
lh_u32_t
lh_str_utf8_encode(lh_char_t *out, lh_u32_t bytes, lh_u32_t code);

/**
 * @brief Read the rest of a character whose lead byte @p lead was already read and
 *        stepped over, and move @p cursor past it.
 *
 * The rare half of ::lh_str_utf8_get_next_code, out of line: the length comes from
 * the lead byte (::lh_str_utf8_get_lead_size) and every continuation byte adds six
 * bits. A @p lead that is not a lead byte comes back as itself; a character cut
 * short by the end of the string comes back as far as it was read, and the cursor
 * stays on the byte that is not a continuation.
 *
 * @param cursor Points just past @p lead; moved past the character.
 * @param lead   The first byte of the character, `0x80` or above.
 * @return The code point.
 */
lh_u32_t
lh_str_utf8_read_after_lead(const lh_char_t **cursor, lh_byte_t lead);

/**
 * @brief Read the character at @p cursor and move the cursor past it.
 *
 * The decoder of a walk over a UTF-8 string, one code point at a time. The end of
 * the string and ASCII — nearly every character of most text — are decided here,
 * in line (::LH_ATTRIBUTE_FORCE_INLINE): measuring a line reads every character,
 * and a call per character costs more than the work. Anything longer goes to
 * ::lh_str_utf8_read_after_lead.
 *
 * @param cursor Points at the position to read; moved past the character. Must not
 *               be null, nor point at null.
 * @return The code point; `0` at the terminator, where the cursor does not move.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_str_utf8_get_next_code(const lh_char_t **cursor)
{
    const lh_byte_t first = lh_cast_static(lh_byte_t, lh_ptr_deref(lh_ptr_deref(cursor)));

    if (first == 0U)
    {
        return 0U;
    }
    ++lh_ptr_deref(cursor);
    if (first < 0x80U)
    {
        return lh_cast_static(lh_u32_t, first);
    }
    return lh_str_utf8_read_after_lead(cursor, first);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_UTF8_H */
