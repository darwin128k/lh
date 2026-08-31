#include <lh/util/str/ptr.h>
#include <lh/char/map.h>
#include <lh/memory.h>
#include <lh/util/char.h>
#include <lh/util/math.h>
#include <lh/util/return.h>
#include <lh/util/addr.h>
#include <lh/assert.h>

static const lh_char_t m_whitespace_chars[] = {lh_char_map_space, lh_char_map_lf, lh_char_map_cr,
                                               lh_char_map_ht,    lh_char_map_vt, lh_char_map_nul};

static const lh_usize_t m_whitespace_size = lh_str_ptr_get_size(m_whitespace_chars);

const lh_str_ptr
lh_str_ptr_find_of_char(const lh_str_ptr str, lh_usize_t size, lh_char_t ch)
{
    return lh_memory_find(str, size, lh_addr_of(ch), LH_CHAR_T_SIZE);
}

const lh_str_ptr
lh_str_ptr_rfind_of_char(const lh_str_ptr str, lh_usize_t size, lh_char_t ch)
{
    return lh_memory_rfind(str, size, lh_addr_of(ch), LH_CHAR_T_SIZE);
}

lh_str_ptr
lh_str_ptr_set(lh_str_ptr str, lh_usize_t size, lh_char_t ch)
{
    return lh_memory_set(str, size, ch);
}

#define LH_STR_PTR_PP_T lh_str_ptr
#define LH_STR_PTR_PP_CHAR_T lh_char_t
#define LH_STR_PTR_PP_CHAR_SIZE LH_CHAR_T_SIZE
#define LH_STR_PTR_PP_FN2(suffix) lh_str_ptr##suffix
#define LH_STR_PTR_PP_FN(suffix) LH_STR_PTR_PP_FN2(suffix)
#define LH_STR_PTR_PP_FOLD_CASE(c) lh_char_fold_case(c)
#define LH_STR_PTR_PP_NUL lh_char_map_nul
#define LH_STR_PTR_PP_INVALID LH_STR_PTR_INVALID
#define LH_STR_PTR_PP_FIND_CHAR(s, n, c) lh_str_ptr_find_of_char(s, n, c)
#define LH_STR_PTR_PP_WHITESPACE_CHARS m_whitespace_chars
#define LH_STR_PTR_PP_WHITESPACE_SIZE m_whitespace_size

#include "../str_ptr_generic.inc"

#undef LH_STR_PTR_PP_T
#undef LH_STR_PTR_PP_CHAR_T
#undef LH_STR_PTR_PP_CHAR_SIZE
#undef LH_STR_PTR_PP_FN2
#undef LH_STR_PTR_PP_FN
#undef LH_STR_PTR_PP_FOLD_CASE
#undef LH_STR_PTR_PP_NUL
#undef LH_STR_PTR_PP_INVALID
#undef LH_STR_PTR_PP_FIND_CHAR
#undef LH_STR_PTR_PP_WHITESPACE_CHARS
#undef LH_STR_PTR_PP_WHITESPACE_SIZE
