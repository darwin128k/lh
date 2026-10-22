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

#include <lh/byte.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

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
 * @brief Write @p code into @p out as UTF-8 and return how many bytes it took.
 *
 * Writes nothing and returns `0` when @p code is not encodable or @p bytes is too
 * few, so a buffer that is one byte short gets a refused character rather than half
 * of one — half a code point is not a character, and the next read over it would
 * walk off the end of the text.
 */
lh_u32_t
lh_str_utf8_encode(lh_char_t *out, lh_u32_t bytes, lh_u32_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_UTF8_H */
