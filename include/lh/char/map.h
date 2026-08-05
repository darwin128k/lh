/**
 * @file map.h
 * @brief Named `char` literals for every 7-bit ASCII code unit (0x00–0x7F).
 *
 * Each macro expands to a single-byte character constant suitable
 * for `lh_char_t`, C string literals, and libc text interfaces.
 *
 * Names follow ASCII / Unicode conventions
 * (for example SOH, HT, Latin capital A).
 *
 * @par References
 *   - ISO/IEC 646 / ANSI X3.4 (ASCII)
 *   - Unicode: U+0000–U+007F (Basic Latin)
 */

#ifndef LH_CHAR_MAP_H
#define LH_CHAR_MAP_H

/* ── C0 controls (0x00–0x1F) ────────────────────────────────────────────── */

/**
 * @def lh_char_map_nul
 * @brief Null (NUL); control character at code point 0x00.
 */
#define lh_char_map_nul '\0'

/**
 * @def lh_char_map_soh
 * @brief Start of Heading (SOH); control character at code point 0x01.
 */
#define lh_char_map_soh '\x01'

/**
 * @def lh_char_map_stx
 * @brief Start of Text (STX); control character at code point 0x02.
 */
#define lh_char_map_stx '\x02'

/**
 * @def lh_char_map_etx
 * @brief End of Text (ETX); control character at code point 0x03.
 */
#define lh_char_map_etx '\x03'

/**
 * @def lh_char_map_eot
 * @brief End of Transmission (EOT); control character at code point 0x04.
 */
#define lh_char_map_eot '\x04'

/**
 * @def lh_char_map_enq
 * @brief Enquiry (ENQ); control character at code point 0x05.
 */
#define lh_char_map_enq '\x05'

/**
 * @def lh_char_map_ack
 * @brief Acknowledge (ACK); control character at code point 0x06.
 */
#define lh_char_map_ack '\x06'

/**
 * @def lh_char_map_bel
 * @brief Bell (BEL); control character at code point 0x07.
 */
#define lh_char_map_bel '\a'

/**
 * @def lh_char_map_bs
 * @brief Backspace (BS); control character at code point 0x08.
 */
#define lh_char_map_bs '\b'

/**
 * @def lh_char_map_ht
 * @brief Character Tabulation / horizontal tab (HT); control character at 0x09.
 */
#define lh_char_map_ht '\t'

/**
 * @def lh_char_map_lf
 * @brief Line Feed (LF); control character at code point 0x0A.
 */
#define lh_char_map_lf '\n'

/**
 * @def lh_char_map_vt
 * @brief Line Tabulation / vertical tab (VT); control character at 0x0B.
 */
#define lh_char_map_vt '\v'

/**
 * @def lh_char_map_ff
 * @brief Form Feed (FF); control character at code point 0x0C.
 */
#define lh_char_map_ff '\f'

/**
 * @def lh_char_map_cr
 * @brief Carriage Return (CR); control character at code point 0x0D.
 */
#define lh_char_map_cr '\r'

/**
 * @def lh_char_map_so
 * @brief Shift Out (SO); control character at code point 0x0E.
 */
#define lh_char_map_so '\x0e'

/**
 * @def lh_char_map_si
 * @brief Shift In (SI); control character at code point 0x0F.
 */
#define lh_char_map_si '\x0f'

/**
 * @def lh_char_map_dle
 * @brief Data Link Escape (DLE); control character at code point 0x10.
 */
#define lh_char_map_dle '\x10'

/**
 * @def lh_char_map_dc1
 * @brief Device Control One (DC1); control character at code point 0x11.
 */
#define lh_char_map_dc1 '\x11'

/**
 * @def lh_char_map_dc2
 * @brief Device Control Two (DC2); control character at code point 0x12.
 */
#define lh_char_map_dc2 '\x12'

/**
 * @def lh_char_map_dc3
 * @brief Device Control Three (DC3); control character at code point 0x13.
 */
#define lh_char_map_dc3 '\x13'

/**
 * @def lh_char_map_dc4
 * @brief Device Control Four (DC4); control character at code point 0x14.
 */
#define lh_char_map_dc4 '\x14'

/**
 * @def lh_char_map_nak
 * @brief Negative Acknowledge (NAK); control character at code point 0x15.
 */
#define lh_char_map_nak '\x15'

/**
 * @def lh_char_map_syn
 * @brief Synchronous Idle (SYN); control character at code point 0x16.
 */
#define lh_char_map_syn '\x16'

/**
 * @def lh_char_map_etb
 * @brief End of Transmission Block (ETB); control character at code point 0x17.
 */
#define lh_char_map_etb '\x17'

/**
 * @def lh_char_map_can
 * @brief Cancel (CAN); control character at code point 0x18.
 */
#define lh_char_map_can '\x18'

/**
 * @def lh_char_map_em
 * @brief End of Medium (EM); control character at code point 0x19.
 */
#define lh_char_map_em '\x19'

/**
 * @def lh_char_map_sub
 * @brief Substitute (SUB); control character at code point 0x1A.
 */
#define lh_char_map_sub '\x1a'

/**
 * @def lh_char_map_esc
 * @brief Escape (ESC); control character at code point 0x1B.
 */
#define lh_char_map_esc '\x1b'

/**
 * @def lh_char_map_fs
 * @brief Information Separator Four / file separator (FS); code point 0x1C.
 */
#define lh_char_map_fs '\x1c'

/**
 * @def lh_char_map_gs
 * @brief Information Separator Three / group separator (GS); code point 0x1D.
 */
#define lh_char_map_gs '\x1d'

/**
 * @def lh_char_map_rs
 * @brief Information Separator Two / record separator (RS); code point 0x1E.
 */
#define lh_char_map_rs '\x1e'

/**
 * @def lh_char_map_us
 * @brief Information Separator One / unit separator (US); code point 0x1F.
 */
#define lh_char_map_us '\x1f'

/* ── Space and punctuation (0x20–0x2F) ─────────────────────────────────── */

/**
 * @def lh_char_map_space
 * @brief Space (SP); separator at code point 0x20.
 */
#define lh_char_map_space ' '

/**
 * @def lh_char_map_exclamation
 * @brief Exclamation mark; punctuation at code point 0x21.
 */
#define lh_char_map_exclamation '!'

/**
 * @def lh_char_map_dquot
 * @brief Quotation mark; punctuation at code point 0x22.
 */
#define lh_char_map_dquot '"'

/**
 * @def lh_char_map_hash
 * @brief Number sign; punctuation at code point 0x23.
 */
#define lh_char_map_hash '#'

/**
 * @def lh_char_map_dollar
 * @brief Dollar sign; punctuation at code point 0x24.
 */
#define lh_char_map_dollar '$'

/**
 * @def lh_char_map_percent
 * @brief Percent sign; punctuation at code point 0x25.
 */
#define lh_char_map_percent '%'

/**
 * @def lh_char_map_ampersand
 * @brief Ampersand; punctuation at code point 0x26.
 */
#define lh_char_map_ampersand '&'

/**
 * @def lh_char_map_squote
 * @brief Apostrophe / single quote; punctuation at code point 0x27.
 */
#define lh_char_map_squote '\''

/**
 * @def lh_char_map_lparen
 * @brief Left parenthesis; punctuation at code point 0x28.
 */
#define lh_char_map_lparen '('

/**
 * @def lh_char_map_rparen
 * @brief Right parenthesis; punctuation at code point 0x29.
 */
#define lh_char_map_rparen ')'

/**
 * @def lh_char_map_asterisk
 * @brief Asterisk; punctuation at code point 0x2A.
 */
#define lh_char_map_asterisk '*'

/**
 * @def lh_char_map_plus
 * @brief Plus sign; punctuation at code point 0x2B.
 */
#define lh_char_map_plus '+'

/**
 * @def lh_char_map_comma
 * @brief Comma; punctuation at code point 0x2C.
 */
#define lh_char_map_comma ','

/**
 * @def lh_char_map_minus
 * @brief Hyphen-minus; punctuation at code point 0x2D.
 */
#define lh_char_map_minus '-'

/**
 * @def lh_char_map_period
 * @brief Full stop; punctuation at code point 0x2E.
 */
#define lh_char_map_period '.'

/**
 * @def lh_char_map_slash
 * @brief Solidus (slash); punctuation at code point 0x2F.
 */
#define lh_char_map_slash '/'

/* ── Decimal digits (0x30–0x39) ─────────────────────────────────────────── */

/**
 * @def lh_char_map_digit_0
 * @brief Digit zero; decimal numeral at code point 0x30.
 */
#define lh_char_map_digit_0 '0'

/**
 * @def lh_char_map_digit_1
 * @brief Digit one; decimal numeral at code point 0x31.
 */
#define lh_char_map_digit_1 '1'

/**
 * @def lh_char_map_digit_2
 * @brief Digit two; decimal numeral at code point 0x32.
 */
#define lh_char_map_digit_2 '2'

/**
 * @def lh_char_map_digit_3
 * @brief Digit three; decimal numeral at code point 0x33.
 */
#define lh_char_map_digit_3 '3'

/**
 * @def lh_char_map_digit_4
 * @brief Digit four; decimal numeral at code point 0x34.
 */
#define lh_char_map_digit_4 '4'

/**
 * @def lh_char_map_digit_5
 * @brief Digit five; decimal numeral at code point 0x35.
 */
#define lh_char_map_digit_5 '5'

/**
 * @def lh_char_map_digit_6
 * @brief Digit six; decimal numeral at code point 0x36.
 */
#define lh_char_map_digit_6 '6'

/**
 * @def lh_char_map_digit_7
 * @brief Digit seven; decimal numeral at code point 0x37.
 */
#define lh_char_map_digit_7 '7'

/**
 * @def lh_char_map_digit_8
 * @brief Digit eight; decimal numeral at code point 0x38.
 */
#define lh_char_map_digit_8 '8'

/**
 * @def lh_char_map_digit_9
 * @brief Digit nine; decimal numeral at code point 0x39.
 */
#define lh_char_map_digit_9 '9'

/* ── Punctuation (0x3A–0x40) ─────────────────────────────────────────────── */

/**
 * @def lh_char_map_colon
 * @brief Colon; punctuation at code point 0x3A.
 */
#define lh_char_map_colon ':'

/**
 * @def lh_char_map_semicolon
 * @brief Semicolon; punctuation at code point 0x3B.
 */
#define lh_char_map_semicolon ';'

/**
 * @def lh_char_map_less
 * @brief Less-than sign; punctuation at code point 0x3C.
 */
#define lh_char_map_less '<'

/**
 * @def lh_char_map_equals
 * @brief Equals sign; punctuation at code point 0x3D.
 */
#define lh_char_map_equals '='

/**
 * @def lh_char_map_greater
 * @brief Greater-than sign; punctuation at code point 0x3E.
 */
#define lh_char_map_greater '>'

/**
 * @def lh_char_map_question
 * @brief Question mark; punctuation at code point 0x3F.
 */
#define lh_char_map_question '?'

/**
 * @def lh_char_map_at
 * @brief Commercial at; punctuation at code point 0x40.
 */
#define lh_char_map_at '@'

/* ── Latin capital letters A–Z (0x41–0x5A) ───────────────────────────────── */

/**
 * @def lh_char_map_capital_a
 * @brief Latin capital letter A; code point 0x41.
 */
#define lh_char_map_capital_a 'A'

/**
 * @def lh_char_map_capital_b
 * @brief Latin capital letter B; code point 0x42.
 */
#define lh_char_map_capital_b 'B'

/**
 * @def lh_char_map_capital_c
 * @brief Latin capital letter C; code point 0x43.
 */
#define lh_char_map_capital_c 'C'

/**
 * @def lh_char_map_capital_d
 * @brief Latin capital letter D; code point 0x44.
 */
#define lh_char_map_capital_d 'D'

/**
 * @def lh_char_map_capital_e
 * @brief Latin capital letter E; code point 0x45.
 */
#define lh_char_map_capital_e 'E'

/**
 * @def lh_char_map_capital_f
 * @brief Latin capital letter F; code point 0x46.
 */
#define lh_char_map_capital_f 'F'

/**
 * @def lh_char_map_capital_g
 * @brief Latin capital letter G; code point 0x47.
 */
#define lh_char_map_capital_g 'G'

/**
 * @def lh_char_map_capital_h
 * @brief Latin capital letter H; code point 0x48.
 */
#define lh_char_map_capital_h 'H'

/**
 * @def lh_char_map_capital_i
 * @brief Latin capital letter I; code point 0x49.
 */
#define lh_char_map_capital_i 'I'

/**
 * @def lh_char_map_capital_j
 * @brief Latin capital letter J; code point 0x4A.
 */
#define lh_char_map_capital_j 'J'

/**
 * @def lh_char_map_capital_k
 * @brief Latin capital letter K; code point 0x4B.
 */
#define lh_char_map_capital_k 'K'

/**
 * @def lh_char_map_capital_l
 * @brief Latin capital letter L; code point 0x4C.
 */
#define lh_char_map_capital_l 'L'

/**
 * @def lh_char_map_capital_m
 * @brief Latin capital letter M; code point 0x4D.
 */
#define lh_char_map_capital_m 'M'

/**
 * @def lh_char_map_capital_n
 * @brief Latin capital letter N; code point 0x4E.
 */
#define lh_char_map_capital_n 'N'

/**
 * @def lh_char_map_capital_o
 * @brief Latin capital letter O; code point 0x4F.
 */
#define lh_char_map_capital_o 'O'

/**
 * @def lh_char_map_capital_p
 * @brief Latin capital letter P; code point 0x50.
 */
#define lh_char_map_capital_p 'P'

/**
 * @def lh_char_map_capital_q
 * @brief Latin capital letter Q; code point 0x51.
 */
#define lh_char_map_capital_q 'Q'

/**
 * @def lh_char_map_capital_r
 * @brief Latin capital letter R; code point 0x52.
 */
#define lh_char_map_capital_r 'R'

/**
 * @def lh_char_map_capital_s
 * @brief Latin capital letter S; code point 0x53.
 */
#define lh_char_map_capital_s 'S'

/**
 * @def lh_char_map_capital_t
 * @brief Latin capital letter T; code point 0x54.
 */
#define lh_char_map_capital_t 'T'

/**
 * @def lh_char_map_capital_u
 * @brief Latin capital letter U; code point 0x55.
 */
#define lh_char_map_capital_u 'U'

/**
 * @def lh_char_map_capital_v
 * @brief Latin capital letter V; code point 0x56.
 */
#define lh_char_map_capital_v 'V'

/**
 * @def lh_char_map_capital_w
 * @brief Latin capital letter W; code point 0x57.
 */
#define lh_char_map_capital_w 'W'

/**
 * @def lh_char_map_capital_x
 * @brief Latin capital letter X; code point 0x58.
 */
#define lh_char_map_capital_x 'X'

/**
 * @def lh_char_map_capital_y
 * @brief Latin capital letter Y; code point 0x59.
 */
#define lh_char_map_capital_y 'Y'

/**
 * @def lh_char_map_capital_z
 * @brief Latin capital letter Z; code point 0x5A.
 */
#define lh_char_map_capital_z 'Z'

/* ── Punctuation (0x5B–0x60) ───────────────────────────────────────────── */

/**
 * @def lh_char_map_lbracket
 * @brief Left square bracket; punctuation at code point 0x5B.
 */
#define lh_char_map_lbracket '['

/**
 * @def lh_char_map_backslash
 * @brief Reverse solidus (backslash); punctuation at code point 0x5C.
 */
#define lh_char_map_backslash '\\'

/**
 * @def lh_char_map_rbracket
 * @brief Right square bracket; punctuation at code point 0x5D.
 */
#define lh_char_map_rbracket ']'

/**
 * @def lh_char_map_caret
 * @brief Circumflex accent / caret; punctuation at code point 0x5E.
 */
#define lh_char_map_caret '^'

/**
 * @def lh_char_map_underscore
 * @brief Low line (underscore); punctuation at code point 0x5F.
 */
#define lh_char_map_underscore '_'

/**
 * @def lh_char_map_grave
 * @brief Grave accent; punctuation at code point 0x60.
 */
#define lh_char_map_grave '`'

/* ── Latin small letters a–z (0x61–0x7A) ─────────────────────────────────── */

/**
 * @def lh_char_map_small_a
 * @brief Latin small letter A; code point 0x61.
 */
#define lh_char_map_small_a 'a'

/**
 * @def lh_char_map_small_b
 * @brief Latin small letter B; code point 0x62.
 */
#define lh_char_map_small_b 'b'

/**
 * @def lh_char_map_small_c
 * @brief Latin small letter C; code point 0x63.
 */
#define lh_char_map_small_c 'c'

/**
 * @def lh_char_map_small_d
 * @brief Latin small letter D; code point 0x64.
 */
#define lh_char_map_small_d 'd'

/**
 * @def lh_char_map_small_e
 * @brief Latin small letter E; code point 0x65.
 */
#define lh_char_map_small_e 'e'

/**
 * @def lh_char_map_small_f
 * @brief Latin small letter F; code point 0x66.
 */
#define lh_char_map_small_f 'f'

/**
 * @def lh_char_map_small_g
 * @brief Latin small letter G; code point 0x67.
 */
#define lh_char_map_small_g 'g'

/**
 * @def lh_char_map_small_h
 * @brief Latin small letter H; code point 0x68.
 */
#define lh_char_map_small_h 'h'

/**
 * @def lh_char_map_small_i
 * @brief Latin small letter I; code point 0x69.
 */
#define lh_char_map_small_i 'i'

/**
 * @def lh_char_map_small_j
 * @brief Latin small letter J; code point 0x6A.
 */
#define lh_char_map_small_j 'j'

/**
 * @def lh_char_map_small_k
 * @brief Latin small letter K; code point 0x6B.
 */
#define lh_char_map_small_k 'k'

/**
 * @def lh_char_map_small_l
 * @brief Latin small letter L; code point 0x6C.
 */
#define lh_char_map_small_l 'l'

/**
 * @def lh_char_map_small_m
 * @brief Latin small letter M; code point 0x6D.
 */
#define lh_char_map_small_m 'm'

/**
 * @def lh_char_map_small_n
 * @brief Latin small letter N; code point 0x6E.
 */
#define lh_char_map_small_n 'n'

/**
 * @def lh_char_map_small_o
 * @brief Latin small letter O; code point 0x6F.
 */
#define lh_char_map_small_o 'o'

/**
 * @def lh_char_map_small_p
 * @brief Latin small letter P; code point 0x70.
 */
#define lh_char_map_small_p 'p'

/**
 * @def lh_char_map_small_q
 * @brief Latin small letter Q; code point 0x71.
 */
#define lh_char_map_small_q 'q'

/**
 * @def lh_char_map_small_r
 * @brief Latin small letter R; code point 0x72.
 */
#define lh_char_map_small_r 'r'

/**
 * @def lh_char_map_small_s
 * @brief Latin small letter S; code point 0x73.
 */
#define lh_char_map_small_s 's'

/**
 * @def lh_char_map_small_t
 * @brief Latin small letter T; code point 0x74.
 */
#define lh_char_map_small_t 't'

/**
 * @def lh_char_map_small_u
 * @brief Latin small letter U; code point 0x75.
 */
#define lh_char_map_small_u 'u'

/**
 * @def lh_char_map_small_v
 * @brief Latin small letter V; code point 0x76.
 */
#define lh_char_map_small_v 'v'

/**
 * @def lh_char_map_small_w
 * @brief Latin small letter W; code point 0x77.
 */
#define lh_char_map_small_w 'w'

/**
 * @def lh_char_map_small_x
 * @brief Latin small letter X; code point 0x78.
 */
#define lh_char_map_small_x 'x'

/**
 * @def lh_char_map_small_y
 * @brief Latin small letter Y; code point 0x79.
 */
#define lh_char_map_small_y 'y'

/**
 * @def lh_char_map_small_z
 * @brief Latin small letter Z; code point 0x7A.
 */
#define lh_char_map_small_z 'z'

/* ── Punctuation (0x7B–0x7E) ─────────────────────────────────────────── */

/**
 * @def lh_char_map_lbrace
 * @brief Left curly bracket; punctuation at code point 0x7B.
 */
#define lh_char_map_lbrace '{'

/**
 * @def lh_char_map_pipe
 * @brief Vertical line; punctuation at code point 0x7C.
 */
#define lh_char_map_pipe '|'

/**
 * @def lh_char_map_rbrace
 * @brief Right curly bracket; punctuation at code point 0x7D.
 */
#define lh_char_map_rbrace '}'

/**
 * @def lh_char_map_tilde
 * @brief Tilde; punctuation at code point 0x7E.
 */
#define lh_char_map_tilde '~'

/* ── Delete (0x7F) ──────────────────────────────────────────────────────── */

/**
 * @def lh_char_map_del
 * @brief Delete (DEL); control character at code point 0x7F.
 */
#define lh_char_map_del '\x7f'

#endif /* LH_CHAR_MAP_H */
