#include <lh/core/http/target/error.h>

const lh_str_ptr
lh_http_target_error_get_name(lh_http_target_error_t error)
{
    switch (error)
    {
    case lh_http_target_error_none:
        return "none";
    case lh_http_target_error_bad_target:
        return "bad_target";
    case lh_http_target_error_bad_encoding:
        return "bad_encoding";
    case lh_http_target_error_encoded_separator:
        return "encoded_separator";
    case lh_http_target_error_bad_char:
        return "bad_char";
    case lh_http_target_error_too_long:
        return "uri_too_long";
    case lh_http_target_error_short_name:
        return "short_name";
    case lh_http_target_error_empty_component:
        return "empty_component";
    case lh_http_target_error_hidden_or_dot:
        return "hidden_or_dot";
    case lh_http_target_error_trailing_dot_or_space:
        return "trailing_dot_or_space";
    default:
        return "unknown";
    }
}
