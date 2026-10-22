#include <lh/json/error.h>

const lh_str_ptr
lh_json_error_get_message(lh_json_error_t error)
{
    switch (error)
    {
    case lh_json_error_none:
        return "no error";
    case lh_json_error_unexpected_end:
        return "text ends inside a value";
    case lh_json_error_unexpected_char:
        return "unexpected character";
    case lh_json_error_bad_literal:
        return "word is not null, true or false";
    case lh_json_error_bad_number:
        return "malformed number";
    case lh_json_error_bad_string:
        return "control character in a string";
    case lh_json_error_bad_escape:
        return "unknown escape in a string";
    case lh_json_error_bad_unicode:
        return "bad \\u escape";
    case lh_json_error_too_deep:
        return "nested too deep";
    case lh_json_error_trailing:
        return "text after the value";
    default:
        return "unknown error";
    }
}
