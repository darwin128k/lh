#include <lh/util/wstr/ptr.h>
#include <lh/char/map.h>
#include <lh/memory.h>
#include <lh/util/math.h>
#include <lh/util/wchar.h>
#include <lh/util/return.h>
#include <lh/assert.h>

static const lh_wchar_t m_whitespace_chars[] = {
    lh_wstr_ptr_cat_va(lh_char_map_space), lh_wstr_ptr_cat_va(lh_char_map_lf),
    lh_wstr_ptr_cat_va(lh_char_map_cr),    lh_wstr_ptr_cat_va(lh_char_map_ht),
    lh_wstr_ptr_cat_va(lh_char_map_vt),    lh_wstr_ptr_cat_va(lh_char_map_nul)};

static const lh_usize_t m_whitespace_size = lh_wstr_ptr_get_size(m_whitespace_chars);

const lh_wstr_ptr
lh_wstr_ptr_find_char(const lh_wstr_ptr str, lh_usize_t size, lh_wchar_t val)
{
    return lh_memory_find_step(str, size * LH_WCHAR_T_SIZE, lh_addr_of(val), LH_WCHAR_T_SIZE,
                               LH_WCHAR_T_SIZE);
}

const lh_wstr_ptr
lh_wstr_ptr_rfind_char(const lh_wstr_ptr str, lh_usize_t size, lh_wchar_t val)
{
    return lh_memory_rfind_step(str, size * LH_WCHAR_T_SIZE, lh_addr_of(val), LH_WCHAR_T_SIZE,
                                LH_WCHAR_T_SIZE);
}

lh_wstr_ptr
lh_wstr_ptr_set(lh_wstr_ptr str, lh_usize_t str_size, lh_wchar_t ch)
{
    return lh_wstr_ptr_set_pattern(str, str_size, lh_addr_of(ch), 1);
}

#define LH_STR_PTR_PP_T lh_wstr_ptr
#define LH_STR_PTR_PP_CHAR_T lh_wchar_t
#define LH_STR_PTR_PP_CHAR_SIZE LH_WCHAR_T_SIZE
#define LH_STR_PTR_PP_FN2(suffix) lh_wstr_ptr##suffix
#define LH_STR_PTR_PP_FN(suffix) LH_STR_PTR_PP_FN2(suffix)
#define LH_STR_PTR_PP_FOLD_CASE(c) lh_wchar_fold_case(c)
#define LH_STR_PTR_PP_NUL lh_wstr_ptr_cat_va(lh_char_map_nul)
#define LH_STR_PTR_PP_INVALID LH_WSTR_PTR_INVALID
#define LH_STR_PTR_PP_FIND_CHAR(s, n, c) lh_wstr_ptr_find_char(s, n, c)
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
