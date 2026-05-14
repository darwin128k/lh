#include <lh/util/str/ptr.h>
#include <lh/char/map.h>
#include <lh/memory.h>
#include <lh/util/char.h>
#include <lh/util/math.h>
#include <lh/util/return.h>
#include <lh/assert.h>

static const lh_char_t m_whitespace_chars[] = {lh_char_map_space, lh_char_map_lf, lh_char_map_cr,
                                               lh_char_map_ht,    lh_char_map_vt, lh_char_map_nul};

static const lh_usize_t m_whitespace_size = lh_str_ptr_get_size(m_whitespace_chars);

lh_usize_t
lh_str_ptr_index_of_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                            lh_usize_t src_size, lh_bool_t ignore_case)
{
    /* empty needle matches at position 0 by convention */
    lh_return_ifn(src_size, LH_USIZE_T_MIN);
    if (lh_math_lt(str_size, src_size))
    {
        return LH_STR_PTR_INVALID;
    }

    const lh_str_ptr ptr = lh_str_ptr_find(str, str_size, src, src_size, ignore_case);
    return lh_null_ne(ptr) ? lh_ptr_udiff(ptr, str) : LH_STR_PTR_INVALID;
}

const lh_str_ptr
lh_str_ptr_find_of_char(const lh_str_ptr str, lh_usize_t size, lh_char_t ch)
{
    return lh_memory_find(str, size, &ch, LH_CHAR_T_SIZE);
}

const lh_str_ptr
lh_str_ptr_rfind_of_char(const lh_str_ptr str, lh_usize_t size, lh_char_t ch)
{
    return lh_memory_rfind(str, size, &ch, LH_CHAR_T_SIZE);
}

const lh_str_ptr
lh_str_ptr_find_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                         lh_usize_t chars_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(chars);
    lh_return_ifn(chars_size, lh_null);

    for (lh_usize_t i = 0; i < str_size; ++i)
    {
        if (lh_str_ptr_contains_char(chars, chars_size, str[i]))
        {
            return str + i;
        }
    }
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_rfind_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                          lh_usize_t chars_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(chars);
    lh_return_ifn(chars_size, lh_null);

    /* do-while starts with i = str_size; --i would wrap on unsigned 0 */
    lh_return_ifn(str_size, lh_null);
    lh_usize_t i = str_size;
    do
    {
        --i;
        if (lh_str_ptr_contains_char(chars, chars_size, str[i]))
        {
            return str + i;
        }
    } while (i > 0);
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_find_not_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                             lh_usize_t chars_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(chars);

    for (lh_usize_t i = 0; i < str_size; ++i)
    {
        if (!lh_str_ptr_contains_char(chars, chars_size, str[i]))
        {
            return str + i;
        }
    }
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_rfind_not_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                              lh_usize_t chars_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(chars);

    /* do-while starts with i = str_size; --i would wrap on unsigned 0 */
    lh_return_ifn(str_size, lh_null);
    lh_usize_t i = str_size;
    do
    {
        --i;
        if (!lh_str_ptr_contains_char(chars, chars_size, str[i]))
        {
            return str + i;
        }
    } while (i > 0);
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_find_of_null_terminator_by_size(const lh_str_ptr str, lh_usize_t size)
{
    return lh_str_ptr_find_of_char(str, size, lh_char_map_nul);
}

const lh_str_ptr
lh_str_ptr_find_of_null_terminator(const lh_str_ptr str)
{
    lh_usize_t size = lh_addr_diff(LH_USIZE_T_MAX, lh_ptr_to_uaddr(str));
    return lh_str_ptr_find_of_null_terminator_by_size(str, size);
}

lh_usize_t
lh_str_ptr_len(const lh_str_ptr str)
{
    const lh_str_ptr ptr = lh_str_ptr_find_of_null_terminator(str);
    lh_assert_runtime_ifn(lh_ptr_is_set(ptr), lh_runtime_error_code_no_null_terminator);
    return lh_ptr_udiff(ptr, str);
}

lh_str_ptr
lh_str_ptr_copy(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src, lh_usize_t src_size)
{
    return lh_memory_copy(str, str_size, src, src_size);
}

lh_str_ptr
lh_str_ptr_move(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src, lh_usize_t src_size)
{
    return lh_memory_move(str, str_size, src, src_size);
}

lh_str_ptr
lh_str_ptr_set_pattern(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                       lh_usize_t src_size)
{
    return lh_memory_set_pattern(str, str_size, src, src_size);
}

lh_str_ptr
lh_str_ptr_set(lh_str_ptr str, lh_usize_t size, lh_char_t ch)
{
    return lh_memory_set(str, size, ch);
}

const lh_str_ptr
lh_str_ptr_compare_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                                  lh_usize_t src_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(src);

    const lh_usize_t n = lh_math_min(str_size, src_size);
    for (lh_usize_t i = 0; i < n; ++i)
    {
        if (lh_char_fold_case(str[i]) != lh_char_fold_case(src[i]))
        {
            return str + i;
        }
    }
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_rcompare_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                                   lh_usize_t src_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(src);

    const lh_usize_t n = lh_math_min(str_size, src_size);
    for (lh_usize_t k = 0; k < n; ++k)
    {
        const lh_str_ptr lp = str + str_size - 1 - k;
        const lh_str_ptr rp = src + src_size - 1 - k;

        if (lh_char_fold_case(lh_ptr_deref(lp)) != lh_char_fold_case(lh_ptr_deref(rp)))
        {
            return lp;
        }
    }
    return lh_null;
}

const lh_str_ptr
lh_str_ptr_find_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                               lh_usize_t src_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(src);

    lh_return_ifn(str_size, lh_null);
    lh_return_ifn(src_size, lh_null);
    /* lh_math_sub(str_size, src_size) below would underflow without this guard */
    lh_return_if(lh_math_lt(str_size, src_size), lh_null);

    const lh_usize_t last = lh_math_sub(str_size, src_size);
    for (lh_usize_t i = 0; i <= last; ++i)
    {
        if (lh_null_eq(lh_str_ptr_compare_by_ignore_case(str + i, src_size, src, src_size)))
        {
            return str + i;
        }
    }

    return lh_null;
}

const lh_str_ptr
lh_str_ptr_rfind_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                                lh_usize_t src_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(src);

    lh_return_ifn(str_size, lh_null);
    lh_return_ifn(src_size, lh_null);
    /* lh_math_sub(str_size, src_size) below would underflow without this guard */
    lh_return_if(lh_math_lt(str_size, src_size), lh_null);

    lh_usize_t i = lh_math_sub(str_size, src_size);
    do
    {
        if (lh_null_eq(lh_str_ptr_compare_by_ignore_case(str + i, src_size, src, src_size)))
        {
            return str + i;
        }
    } while (i-- > 0);

    return lh_null;
}

const lh_str_ptr
lh_str_ptr_find(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                lh_usize_t src_size, lh_bool_t ignore_case)
{
    return ignore_case ? lh_str_ptr_find_by_ignore_case(str, str_size, src, src_size)
                       : lh_memory_find(str, str_size, src, src_size);
}

const lh_str_ptr
lh_str_ptr_rfind(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                 lh_usize_t src_size, lh_bool_t ignore_case)
{
    return ignore_case ? lh_str_ptr_rfind_by_ignore_case(str, str_size, src, src_size)
                       : lh_memory_rfind(str, str_size, src, src_size);
}

const lh_str_ptr
lh_str_ptr_compare(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                   lh_usize_t src_size, lh_bool_t ignore_case)
{
    return ignore_case ? lh_str_ptr_compare_by_ignore_case(str, str_size, src, src_size)
                       : lh_memory_compare(str, str_size, src, src_size);
}

const lh_str_ptr
lh_str_ptr_rcompare(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                    lh_usize_t src_size, lh_bool_t ignore_case)
{
    return ignore_case ? lh_str_ptr_rcompare_by_ignore_case(str, str_size, src, src_size)
                       : lh_memory_rcompare(str, str_size, src, src_size);
}

lh_bool_t
lh_str_ptr_contains(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    return lh_math_ne(lh_str_ptr_index_of(str, src, ignore_case), LH_STR_PTR_INVALID);
}

lh_bool_t
lh_str_ptr_contains_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                            lh_usize_t src_size, lh_bool_t ignore_case)
{
    return lh_null_ne(lh_str_ptr_find(str, str_size, src, src_size, ignore_case));
}

lh_usize_t
lh_str_ptr_index_of(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    const lh_usize_t str_size = lh_str_ptr_len(str);
    const lh_usize_t src_size = lh_str_ptr_len(src);
    return lh_str_ptr_index_of_by_size(str, str_size, src, src_size, ignore_case);
}

lh_bool_t
lh_str_ptr_starts_with(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    return lh_str_ptr_starts_with_by_size(str, lh_str_ptr_len(str), src, lh_str_ptr_len(src),
                                          ignore_case);
}

lh_bool_t
lh_str_ptr_starts_with_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                               lh_usize_t src_size, lh_bool_t ignore_case)
{
    if (lh_math_lt(str_size, src_size))
    {
        return lh_bool_false;
    }
    return lh_null_eq(lh_str_ptr_compare(str, src_size, src, src_size, ignore_case));
}

lh_bool_t
lh_str_ptr_ends_with(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    return lh_str_ptr_ends_with_by_size(str, lh_str_ptr_len(str), src, lh_str_ptr_len(src),
                                        ignore_case);
}

lh_bool_t
lh_str_ptr_ends_with_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                             lh_usize_t src_size, lh_bool_t ignore_case)
{
    if (lh_math_lt(str_size, src_size))
    {
        return lh_bool_false;
    }
    return lh_null_eq(lh_str_ptr_rcompare(str, str_size, src, src_size, ignore_case));
}

lh_bool_t
lh_str_ptr_equals_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                          lh_usize_t src_size, lh_bool_t ignore_case)
{
    return lh_math_eq(str_size, src_size) &&
           lh_null_eq(lh_str_ptr_compare(str, str_size, src, src_size, ignore_case));
}

lh_bool_t
lh_str_ptr_equals(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case)
{
    return lh_str_ptr_equals_by_size(str, lh_str_ptr_len(str), src, lh_str_ptr_len(src),
                                     ignore_case);
}

lh_bool_t
lh_str_ptr_contains_char(const lh_str_ptr chars, lh_usize_t chars_size, lh_char_t ch)
{
    return lh_null_ne(lh_str_ptr_find_of_char(chars, chars_size, ch));
}

lh_str_ptr
lh_str_ptr_ltrim_custom(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr whitespace_chars,
                        lh_usize_t whitespace_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(whitespace_chars);

    if (!str_size || !whitespace_size)
    {
        return str;
    }

    lh_usize_t offset = 0;
    while (lh_math_lt(offset, str_size) &&
           lh_str_ptr_contains_char(whitespace_chars, whitespace_size, str[offset]))
    {
        ++offset;
    }

    lh_return_ifn(offset, str);
    if (lh_math_eq(offset, str_size))
    {
        str[0] = lh_char_map_nul;
        return str;
    }

    const lh_usize_t trimmed_size = str_size - offset;
    lh_str_ptr_move(str, trimmed_size + 1, str + offset, trimmed_size + 1);
    return str;
}

lh_str_ptr
lh_str_ptr_rtrim_custom(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr whitespace_chars,
                        lh_usize_t whitespace_size)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(whitespace_chars);

    if (!str_size || !whitespace_size)
    {
        return str;
    }

    lh_usize_t end = str_size;
    while (lh_math_is_positive(end) &&
           lh_str_ptr_contains_char(whitespace_chars, whitespace_size, str[end - 1]))
    {
        --end;
    }

    str[end] = lh_char_map_nul;
    return str;
}

lh_str_ptr
lh_str_ptr_trim_custom(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr whitespace_chars,
                       lh_usize_t whitespace_size)
{
    lh_str_ptr_rtrim_custom(str, str_size, whitespace_chars, whitespace_size);
    return lh_str_ptr_ltrim_custom(str, lh_str_ptr_len(str), whitespace_chars, whitespace_size);
}

lh_str_ptr
lh_str_ptr_ltrim_set(lh_str_ptr str, const lh_str_ptr whitespace_chars, lh_usize_t whitespace_size)
{
    return lh_str_ptr_ltrim_custom(str, lh_str_ptr_len(str), whitespace_chars, whitespace_size);
}

lh_str_ptr
lh_str_ptr_rtrim_set(lh_str_ptr str, const lh_str_ptr whitespace_chars, lh_usize_t whitespace_size)
{
    return lh_str_ptr_rtrim_custom(str, lh_str_ptr_len(str), whitespace_chars, whitespace_size);
}

lh_str_ptr
lh_str_ptr_trim_set(lh_str_ptr str, const lh_str_ptr whitespace_chars, lh_usize_t whitespace_size)
{
    return lh_str_ptr_trim_custom(str, lh_str_ptr_len(str), whitespace_chars, whitespace_size);
}

lh_str_ptr
lh_str_ptr_ltrim_auto(lh_str_ptr str, const lh_str_ptr whitespace_chars)
{
    return lh_str_ptr_ltrim_set(str, whitespace_chars, lh_str_ptr_len(whitespace_chars));
}

lh_str_ptr
lh_str_ptr_rtrim_auto(lh_str_ptr str, const lh_str_ptr whitespace_chars)
{
    return lh_str_ptr_rtrim_set(str, whitespace_chars, lh_str_ptr_len(whitespace_chars));
}

lh_str_ptr
lh_str_ptr_trim_auto(lh_str_ptr str, const lh_str_ptr whitespace_chars)
{
    return lh_str_ptr_trim_set(str, whitespace_chars, lh_str_ptr_len(whitespace_chars));
}

lh_str_ptr
lh_str_ptr_ltrim(lh_str_ptr str)
{
    return lh_str_ptr_ltrim_set(str, m_whitespace_chars, m_whitespace_size);
}

lh_str_ptr
lh_str_ptr_rtrim(lh_str_ptr str)
{
    return lh_str_ptr_rtrim_set(str, m_whitespace_chars, m_whitespace_size);
}

lh_str_ptr
lh_str_ptr_trim(lh_str_ptr str)
{
    return lh_str_ptr_trim_set(str, m_whitespace_chars, m_whitespace_size);
}
