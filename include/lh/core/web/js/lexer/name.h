/**
 * @file name.h
 * @brief ASCII JavaScript identifier-name classification, independent of the engine.
 *
 * These small lexical predicates are shared by the optional lexer and by
 * serializers. They recognize a conservative ASCII subset of JavaScript
 * IdentifierName, including keywords, without owning or copying text. The web
 * serializer uses them to emit ordinary names directly and reserves computed
 * properties for other names and `__proto__`. On Windows GCC 16.1 Release/LTO,
 * a two-field object measured 132 ns with computed keys and 90 ns with this
 * check; complete page construction measured 17.75 and 17.62 microseconds.
 * These paired medians describe that machine, not a universal guarantee.
 */

#ifndef LH_WEB_JS_LEXER_NAME_H
#define LH_WEB_JS_LEXER_NAME_H

#include <lh/core/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether a byte can start an ASCII JavaScript identifier name.
 * @param ch Byte to test.
 * @return True for an ASCII letter, dollar sign or underscore; false otherwise.
 */
lh_bool_t
lh_web_js_lexer_is_name_start(lh_char_t ch);

/**
 * @brief Test whether a byte can continue an ASCII JavaScript identifier name.
 * @param ch Byte to test.
 * @return True for a name-start byte or an ASCII digit; false otherwise.
 */
lh_bool_t
lh_web_js_lexer_is_name_part(lh_char_t ch);

/**
 * @brief Test a complete ASCII JavaScript IdentifierName, including keywords.
 *
 * Rejects empty and non-ASCII names even if JavaScript allows their Unicode
 * form; callers may use another representation for those names.
 * @param text Valid borrowed string view; neither it nor its bytes are modified.
 * @return True when every byte forms an ASCII IdentifierName, false otherwise.
 */
lh_bool_t
lh_web_js_lexer_name_is_ascii_identifier(const lh_str_view_t *text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_JS_LEXER_NAME_H */
