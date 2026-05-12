#include <lh/util/wchar.h>
#include <lh/util/wstr/ptr.h>

lh_wchar_t
lh_wchar_to_lower(lh_wchar_t c)
{
    lh_wstr_ptr_to_lower(&c, 1);
    return c;
}

lh_wchar_t
lh_wchar_to_upper(lh_wchar_t c)
{
    lh_wstr_ptr_to_upper(&c, 1);
    return c;
}