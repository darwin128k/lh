/**
 * @file char.h
 * @brief HTTP/1 character classes: which bytes a token, a request target and a
 *        field value may hold (RFC 9110 section 5.6.2, RFC 9112 section 3.2).
 *
 * Built on the LH ASCII predicates (::lh_char_is_letter, ::lh_char_is_digit).
 * The parser tests every byte of a message head through these three bricks and
 * nothing else.
 */

#ifndef LH_HTTP_CHAR_H
#define LH_HTTP_CHAR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether @p c may appear in a token (`tchar`): a method or a field name.
 *
 * @param c Byte to test.
 * @return ::lh_bool_true for an ASCII letter or digit and for
 *         `! # $ % & ' * + - . ^ _ ` | ~`; ::lh_bool_false for anything else.
 */
lh_bool_t
lh_http_is_token_char(lh_char_t c);

/**
 * @brief Whether @p c may appear in a request target: visible ASCII
 *        (`0x21`..`0x7e`).
 *
 * The target stays raw URI text; this brick checks bytes, not URI syntax.
 *
 * @param c Byte to test.
 */
lh_bool_t
lh_http_is_target_char(lh_char_t c);

/**
 * @brief Whether @p c may appear in a field value or a reason phrase.
 *
 * @param c Byte to test.
 * @return ::lh_bool_true for HTAB, SP, visible ASCII and every byte from `0x80`
 *         (non-ASCII octets are kept, not interpreted); ::lh_bool_false for the
 *         other control bytes and DEL.
 */
lh_bool_t
lh_http_is_field_char(lh_char_t c);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_CHAR_H */
