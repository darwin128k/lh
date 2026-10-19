#include <lh/ini/error.h>

const lh_str_ptr
lh_ini_error_get_message(lh_ini_error_t error)
{
    switch (error)
    {
    case lh_ini_error_none:
        return "no error";
    case lh_ini_error_bad_section:
        return "section header is not [name]";
    case lh_ini_error_empty_section:
        return "section header has no name";
    case lh_ini_error_missing_equals:
        return "line is not key = value";
    case lh_ini_error_empty_key:
        return "pair has no key";
    case lh_ini_error_no_section:
        return "pair before the first section";
    default:
        return "unknown error";
    }
}
