/**
 * @file str/cstr.c
 * @brief The const spellings of ::lh_str_ptr_len and ::lh_str_ptr_equals.
 *
 * Two functions, one call each. The whole file exists because the layer below is written
 * against a **mutable** pointer -- it trims and erases, so it needs one it may write
 * through -- and an application holding a `const char *` should not have to cast that away
 * to ask how long it is.
 *
 * `lh_str_view_init` does the same delegation in the same direction and is the precedent:
 * a view over a `const char *` measures it with `lh_str_ptr_len` rather than scanning
 * again. There is one scan and one comparison in this library and this file is not either.
 */

#include <lh/str/cstr.h>
#include <lh/util/str/ptr.h>

lh_usize_t
lh_str_len(lh_str_cptr str)
{
    /* Nothing is checked here on purpose. `lh_str_ptr_len` terminates on a string with no
       terminator, and `lh_str_ptr_equals` reaches `lh_str_ptr_len` for both operands, so
       both rules -- no terminator, and null among the operands -- already hold one call
       down, where they are tested. A second `if` here would be a second answer to the same
       question, and the two could disagree the day one of them changed. */
    return lh_str_ptr_len((lh_str_ptr)str);
}

lh_bool_t
lh_str_eq(lh_str_cptr lhs, lh_str_cptr rhs, lh_bool_t ignore_case)
{
    return lh_str_ptr_equals((lh_str_ptr)lhs, (lh_str_ptr)rhs, ignore_case);
}
