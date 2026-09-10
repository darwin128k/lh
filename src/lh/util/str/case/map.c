#include <lh/util/str/ptr.h>
#include <lh/util/interval.h>
#include <lh/size.h>
#include <lh/char/case/pair.h>
#include <lh/util/array/ptr.h>
#include <lh/assert.h>
#include <lh/cast/static.h>

static const lh_char_case_pair_t m_char_to_lower_table[] = {
    /* clang-format off */
    { 65U, 97U },
    { 66U, 98U },
    { 67U, 99U },
    { 68U, 100U },
    { 69U, 101U },
    { 70U, 102U },
    { 71U, 103U },
    { 72U, 104U },
    { 73U, 105U },
    { 74U, 106U },
    { 75U, 107U },
    { 76U, 108U },
    { 77U, 109U },
    { 78U, 110U },
    { 79U, 111U },
    { 80U, 112U },
    { 81U, 113U },
    { 82U, 114U },
    { 83U, 115U },
    { 84U, 116U },
    { 85U, 117U },
    { 86U, 118U },
    { 87U, 119U },
    { 88U, 120U },
    { 89U, 121U },
    { 90U, 122U },
    /* 128..255 written as their lh_char_t (signed 8-bit) values: v - 256 for v in [128, 255],
     * so the initializer is well-defined instead of relying on implementation-defined narrowing
     * of an out-of-range unsigned literal into a signed char. */
    { -64, -32 },
    { -63, -31 },
    { -62, -30 },
    { -61, -29 },
    { -60, -28 },
    { -59, -27 },
    { -58, -26 },
    { -57, -25 },
    { -56, -24 },
    { -55, -23 },
    { -54, -22 },
    { -53, -21 },
    { -52, -20 },
    { -51, -19 },
    { -50, -18 },
    { -49, -17 },
    { -48, -16 },
    { -47, -15 },
    { -46, -14 },
    { -45, -13 },
    { -44, -12 },
    { -43, -11 },
    { -42, -10 },
    { -40, -8 },
    { -39, -7 },
    { -38, -6 },
    { -37, -5 },
    { -36, -4 },
    { -35, -3 },
    { -34, -2 },
    /* clang-format on */
};
static const lh_usize_t m_char_to_lower_table_size = lh_array_ptr_get_size(m_char_to_lower_table);

static const lh_char_case_pair_t m_char_to_upper_table[] = {
    /* clang-format off */
    { 97U, 65U },
    { 98U, 66U },
    { 99U, 67U },
    { 100U, 68U },
    { 101U, 69U },
    { 102U, 70U },
    { 103U, 71U },
    { 104U, 72U },
    { 105U, 73U },
    { 106U, 74U },
    { 107U, 75U },
    { 108U, 76U },
    { 109U, 77U },
    { 110U, 78U },
    { 111U, 79U },
    { 112U, 80U },
    { 113U, 81U },
    { 114U, 82U },
    { 115U, 83U },
    { 116U, 84U },
    { 117U, 85U },
    { 118U, 86U },
    { 119U, 87U },
    { 120U, 88U },
    { 121U, 89U },
    { 122U, 90U },
    /* 128..255 written as their lh_char_t (signed 8-bit) values: v - 256 for v in [128, 255],
     * so the initializer is well-defined instead of relying on implementation-defined narrowing
     * of an out-of-range unsigned literal into a signed char. */
    { -32, -64 },
    { -31, -63 },
    { -30, -62 },
    { -29, -61 },
    { -28, -60 },
    { -27, -59 },
    { -26, -58 },
    { -25, -57 },
    { -24, -56 },
    { -23, -55 },
    { -22, -54 },
    { -21, -53 },
    { -20, -52 },
    { -19, -51 },
    { -18, -50 },
    { -17, -49 },
    { -16, -48 },
    { -15, -47 },
    { -14, -46 },
    { -13, -45 },
    { -12, -44 },
    { -11, -43 },
    { -10, -42 },
    { -8, -40 },
    { -7, -39 },
    { -6, -38 },
    { -5, -37 },
    { -4, -36 },
    { -3, -35 },
    { -2, -34 },
    /* clang-format on */
};
static const lh_usize_t m_char_to_upper_table_size = lh_array_ptr_get_size(m_char_to_upper_table);

lh_str_ptr
lh_str_ptr_to_lower(lh_str_ptr str, lh_usize_t n)
{
    lh_assert_runtime_ref(str);
    lh_str_ptr p = str;

    while (n--)
    {
        lh_char_t c = lh_ptr_deref(p);

        lh_char_case_pair_t r;
        lh_bool_t is_founded = lh_bool_false;

        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_lower_table,
                                        m_char_to_lower_table_size, lh_cast_static(lh_usize_t, c),
                                        first, r, is_founded);

        *p++ = is_founded ? r.second : c;
    }

    return p;
}

lh_str_ptr
lh_str_ptr_to_upper(lh_str_ptr str, lh_usize_t n)
{
    lh_assert_runtime_ref(str);
    lh_str_ptr p = str;

    while (n--)
    {
        lh_char_t c = lh_ptr_deref(p);

        lh_char_case_pair_t r;
        lh_bool_t is_founded = lh_bool_false;

        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_upper_table,
                                        m_char_to_upper_table_size, lh_cast_static(lh_usize_t, c),
                                        first, r, is_founded);

        *p++ = is_founded ? r.second : c;
    }

    return p;
}
