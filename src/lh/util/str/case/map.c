#include <lh/util/str/ptr.h>
#include <lh/util/interval.h>
#include <lh/size.h>
#include <lh/char/case/pair.h>
#include <lh/util/array/ptr.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/config.h>

#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE

/* Dense O(1) lookup: table[(unsigned char)c] is always the mapped byte (identity
 * for bytes with no case mapping — no sentinel/branch needed). Mechanically
 * derived from (and must be kept in sync with) the sparse pair tables below the
 * #else — see that comment for why the extended range is a gap-y, non-uniform
 * +32/-32 offset (ẞ/× have no simple case mapping in Latin-1). 512 bytes total
 * vs ~112 for the sparse tables; see LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE. */
static const lh_char_t m_char_to_lower_dense[256] = {
    /* clang-format off */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    -32, -31, -30, -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17,
    -16, -15, -14, -13, -12, -11, -10, 215, -8, -7, -6, -5, -4, -3, -2, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    /* clang-format on */
};

static const lh_char_t m_char_to_upper_dense[256] = {
    /* clang-format off */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    -64, -63, -62, -61, -60, -59, -58, -57, -56, -55, -54, -53, -52, -51, -50, -49,
    -48, -47, -46, -45, -44, -43, -42, 247, -40, -39, -38, -37, -36, -35, -34, 255,
    /* clang-format on */
};

#else /* !LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE */

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

#endif /* LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE */

lh_str_ptr
lh_str_ptr_to_lower(lh_str_ptr str, lh_usize_t n)
{
    lh_assert_runtime_ref(str);
    lh_str_ptr p = str;

    while (n--)
    {
        lh_char_t c = lh_ptr_deref(p);

#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
        *p++ = m_char_to_lower_dense[(lh_uchar_t)c];
#else
        lh_char_case_pair_t r;
        lh_bool_t is_founded = lh_bool_false;

        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_lower_table,
                                        m_char_to_lower_table_size, lh_cast_static(lh_usize_t, c),
                                        first, r, is_founded);

        *p++ = is_founded ? r.second : c;
#endif
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

#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
        *p++ = m_char_to_upper_dense[(lh_uchar_t)c];
#else
        lh_char_case_pair_t r;
        lh_bool_t is_founded = lh_bool_false;

        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_upper_table,
                                        m_char_to_upper_table_size, lh_cast_static(lh_usize_t, c),
                                        first, r, is_founded);

        *p++ = is_founded ? r.second : c;
#endif
    }

    return p;
}
