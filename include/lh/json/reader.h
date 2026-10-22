/**
 * @file reader.h
 * @brief Read a JSON text into an ::lh_json_node_t tree (::lh_json_reader_t).
 *
 * A recursive-descent reader of RFC 8259 JSON, the job of cJSON's
 * `cJSON_Parse`, built from small public steps (`read_number`,
 * `read_string_into`, ...) that a caller can also use on their own. It is
 * strict where cJSON is lenient:
 *
 * - Numbers follow the grammar exactly: no `01`, `1.`, `.5`, `+1`, `0x1F`.
 * - A control character (below U+0020) must be escaped inside a string.
 * - `\u` escapes are decoded to UTF-8, surrogate pairs joined; a lone
 *   surrogate and `\u0000` are errors (strings here end at a NUL).
 * - Only space, tab, LF and CR are whitespace; nothing but whitespace may
 *   follow the value.
 * - A UTF-8 byte order mark at the start of the text is skipped.
 *
 * Duplicate keys are kept in order, as cJSON keeps them. The first error
 * stops the read: ::lh_json_reader_get_error says what and
 * ::lh_json_reader_get_pos where (the offset of the offending character).
 *
 * @see lh/json/node.h for an example.
 */

#ifndef LH_JSON_READER_H
#define LH_JSON_READER_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/json/error.h>
#include <lh/json/node.h>
#include <lh/json/reader/fields.h>
#include <lh/size.h>
#include <lh/str/view.h>

#ifndef LH_JSON_READER_DEPTH_MAX
/**
 * @def LH_JSON_READER_DEPTH_MAX
 * @brief How many arrays and objects may be open at once; deeper is
 *        ::lh_json_error_too_deep. Each level is a C stack frame on the way
 *        in and on ::lh_json_node_deinit, so a small stack wants it lower.
 *        Same default as cJSON's `CJSON_NESTING_LIMIT`.
 */
#    define LH_JSON_READER_DEPTH_MAX 1000U
#endif

/**
 * @struct lh_json_reader
 * @brief Text plus position. Fields via ::lh_json_reader_fields.
 */
struct lh_json_reader
{
    lh_json_reader_fields(lh_str_view_t, lh_usize_t, lh_json_error_t);
};
/** @copybrief lh_json_reader */
typedef struct lh_json_reader lh_json_reader_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── accessors ───────────────────────────────────────────────────────────── */

/** The whole text @p self reads. */
const lh_str_view_t *
lh_json_reader_get_text(const lh_json_reader_t *self);

/** Make @p text the whole text @p self reads. Does not move the position. */
lh_void
lh_json_reader_set_text(lh_json_reader_t *self, lh_str_view_t text);

/** The offset of the next character to read; after an error, of the bad one. */
lh_usize_t
lh_json_reader_get_pos(const lh_json_reader_t *self);

/** Make @p pos the offset of the next character to read. */
lh_void
lh_json_reader_set_pos(lh_json_reader_t *self, lh_usize_t pos);

/** How many arrays and objects are open where @p self stands. */
lh_usize_t
lh_json_reader_get_depth(const lh_json_reader_t *self);

/** Make @p depth how many arrays and objects are open. */
lh_void
lh_json_reader_set_depth(lh_json_reader_t *self, lh_usize_t depth);

/**
 * @brief The error that stopped the read, or ::lh_json_error_none.
 */
lh_json_error_t
lh_json_reader_get_error(const lh_json_reader_t *self);

/** Make @p error the error of @p self; anything but ::lh_json_error_none stops the read. */
lh_void
lh_json_reader_set_error(lh_json_reader_t *self, lh_json_error_t error);

/**
 * @brief Stop on @p error: this and every later read returns false.
 *
 * @return ::lh_bool_false, so a step can `return lh_json_reader_fail(...)`.
 */
lh_bool_t
lh_json_reader_fail(lh_json_reader_t *self, lh_json_error_t error);

/**
 * @brief True when a syntax error stopped the read.
 */
lh_bool_t
lh_json_reader_has_error(const lh_json_reader_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Start reading @p text from its first character.
 *
 * The reader owns nothing; @p text must outlive it. The tree it builds
 * copies what it keeps, so the text may go once the read is done.
 */
lh_void
lh_json_reader_init(lh_json_reader_t *self, lh_str_view_t text);

/* ── cursor ──────────────────────────────────────────────────────────────── */

/** True when @p self has read the whole text. */
lh_bool_t
lh_json_reader_is_end(const lh_json_reader_t *self);

/** The character @p ahead places past the position; NUL past the end of the text. */
lh_char_t
lh_json_reader_peek(const lh_json_reader_t *self, lh_usize_t ahead);

/** The character at the position; NUL at the end of the text. */
lh_char_t
lh_json_reader_get_char(const lh_json_reader_t *self);

/** Move the position @p count characters on. */
lh_void
lh_json_reader_advance(lh_json_reader_t *self, lh_usize_t count);

/** True for the four characters JSON calls whitespace: space, tab, LF, CR. */
lh_bool_t
lh_json_reader_is_whitespace(lh_char_t ch);

/** Move past any whitespace at the position. */
lh_void
lh_json_reader_skip_whitespace(lh_json_reader_t *self);

/**
 * @brief Stop on what is at the position: ::lh_json_error_unexpected_end at
 *        the end of the text, ::lh_json_error_unexpected_char otherwise.
 *
 * @return ::lh_bool_false.
 */
lh_bool_t
lh_json_reader_fail_unexpected(lh_json_reader_t *self);

/** Move past @p ch if it is at the position; false, and no move, if not. */
lh_bool_t
lh_json_reader_skip_char(lh_json_reader_t *self, lh_char_t ch);

/** Move past @p word if the text continues with it; false, and no move, if not. */
lh_bool_t
lh_json_reader_skip_word(lh_json_reader_t *self, lh_str_view_t word);

/**
 * @brief Open one more array or object: ::lh_json_error_too_deep past
 *        ::LH_JSON_READER_DEPTH_MAX.
 *
 * @return ::lh_bool_true if the read may go deeper.
 */
lh_bool_t
lh_json_reader_enter(lh_json_reader_t *self);

/** Close the innermost open array or object. */
lh_void
lh_json_reader_leave(lh_json_reader_t *self);

/* ── values ──────────────────────────────────────────────────────────────── */

/**
 * @brief Read `null`, `true` or `false` into @p node.
 *
 * @return ::lh_bool_false on any other word (::lh_json_error_bad_literal).
 */
lh_bool_t
lh_json_reader_read_literal(lh_json_reader_t *self, lh_json_node_t *node);

/** Move past the decimal digits at the position; how many there were. */
lh_usize_t
lh_json_reader_skip_digits(lh_json_reader_t *self);

/**
 * @brief Read a number into @p node, keeping its text as written.
 *
 * `-? (0 | [1-9][0-9]*) (. [0-9]+)? ([eE] [+-]? [0-9]+)?`, nothing else.
 *
 * @return ::lh_bool_false on a malformed number (::lh_json_error_bad_number).
 */
lh_bool_t
lh_json_reader_read_number(lh_json_reader_t *self, lh_json_node_t *node);

/**
 * @brief Read the four hex digits of a `\u` escape into @p code.
 *
 * @return ::lh_bool_false when there are not four (::lh_json_error_bad_unicode).
 */
lh_bool_t
lh_json_reader_read_hex4(lh_json_reader_t *self, lh_u32_t *code);

/**
 * @brief Read what follows `\u` — one escape, or the two of a surrogate
 *        pair — and append the code point to @p out as UTF-8.
 *
 * @return ::lh_bool_false on a lone surrogate or `\u0000`
 *         (::lh_json_error_bad_unicode).
 */
lh_bool_t
lh_json_reader_read_unicode(lh_json_reader_t *self, lh_str_t *out);

/**
 * @brief Read what follows a backslash in a string and append the character
 *        it stands for to @p out.
 *
 * @return ::lh_bool_false on an unknown escape (::lh_json_error_bad_escape).
 */
lh_bool_t
lh_json_reader_read_escape(lh_json_reader_t *self, lh_str_t *out);

/**
 * @brief Read the string at the position, quotes included, and append its
 *        characters, unescaped, to @p out.
 *
 * @return ::lh_bool_false on a raw control character, a bad escape, or a
 *         missing closing quote.
 */
lh_bool_t
lh_json_reader_read_string_into(lh_json_reader_t *self, lh_str_t *out);

/** ::lh_json_reader_read_string_into the text of @p node, which becomes a string. */
lh_bool_t
lh_json_reader_read_string(lh_json_reader_t *self, lh_json_node_t *node);

/**
 * @brief Read the array at the position into @p node, each element a new
 *        child.
 */
lh_bool_t
lh_json_reader_read_array(lh_json_reader_t *self, lh_json_node_t *node);

/**
 * @brief Read the object at the position into @p node, each member a new
 *        child carrying its key.
 */
lh_bool_t
lh_json_reader_read_object(lh_json_reader_t *self, lh_json_node_t *node);

/**
 * @brief Read the value that starts at the position into @p node, whatever
 *        kind it is.
 */
lh_bool_t
lh_json_reader_read_value(lh_json_reader_t *self, lh_json_node_t *node);

/**
 * @brief Read the whole text as one value into @p node.
 *
 * @p node must be freshly initialized (::lh_json_node_init). On false it
 * may hold what was read before the error; ::lh_json_node_deinit it either
 * way.
 *
 * @return ::lh_bool_true if the text is one JSON value with nothing but
 *         whitespace around it; ::lh_bool_false otherwise (see
 *         ::lh_json_reader_get_error and ::lh_json_reader_get_pos).
 */
lh_bool_t
lh_json_reader_read(lh_json_reader_t *self, lh_json_node_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_JSON_READER_H */
