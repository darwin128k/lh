/**
 * @file error.c
 * @brief Implementation of `lh/web/xml/error.h`.
 */

#include <lh/web/xml/error.h>

const lh_str_ptr
lh_web_xml_error_get_message(lh_web_xml_error_t error)
{
    switch (error)
    {
    case lh_web_xml_error_none:
        return "no error";
    case lh_web_xml_error_unexpected_end:
        return "text ends inside markup";
    case lh_web_xml_error_unexpected_char:
        return "unexpected character";
    case lh_web_xml_error_bad_name:
        return "malformed name";
    case lh_web_xml_error_bad_reference:
        return "unknown or malformed reference";
    case lh_web_xml_error_bad_comment:
        return "-- inside a comment";
    case lh_web_xml_error_mismatched_tag:
        return "end tag does not match the open element";
    case lh_web_xml_error_duplicate_attr:
        return "attribute repeated in one tag";
    case lh_web_xml_error_too_deep:
        return "nested too deep";
    case lh_web_xml_error_no_root:
        return "no root element";
    case lh_web_xml_error_trailing:
        return "content outside the root element";
    default:
        return "unknown error";
    }
}
