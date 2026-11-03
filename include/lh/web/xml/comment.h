/**
 * @file comment.h
 * @brief Which texts an XML comment can hold.
 *
 * A comment is written as `<!--text-->` and cannot be escaped, so XML
 * forbids `--` anywhere in its text and a `-` at its end (which would make
 * `--->`). The writer refuses such a comment and the reader reports one with
 * ::lh_web_xml_error_bad_comment; both ask this function.
 */

#ifndef LH_WEB_XML_COMMENT_H
#define LH_WEB_XML_COMMENT_H

#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when @p text may stand between `<!--` and `-->`: no `--` in it
 *        and no `-` at its end. The empty text is valid.
 */
lh_bool_t
lh_web_xml_comment_is_valid(lh_str_view_t text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_COMMENT_H */
