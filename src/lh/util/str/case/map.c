/*
 * AUTO-GENERATED — do not edit. Regenerate with tools/gen_str_case_map.py.
 * Source: Unicode UCD UnicodeData.txt (Unicode 16.0.0), fields 12-13
 * (simple uppercase / simple lowercase), restricted to source and mapped scalars
 * both in 0..255 (lh_char_t is one code unit) — see the script docstring for what
 * that excludes (e.g. MICRO SIGN, LATIN SMALL LETTER Y WITH DIAERESIS).
 *
 * Unicode data copyright Unicode, Inc.; see https://www.unicode.org/terms_of_use.html
 *
 * Two implementations selected by LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
 * (include/lh/config.h) — see that option's doc comment in cmake/library_options.cmake.
 */

#include <lh/util/str/ptr.h>
#include <lh/util/interval.h>
#include <lh/size.h>
#include <lh/char/case/pair.h>
#include <lh/util/array/ptr.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/config.h>
#include <lh/foreach.h>

#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE

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
    -128, -127, -126, -125, -124, -123, -122, -121, -120, -119, -118, -117, -116, -115, -114, -113,
    -112, -111, -110, -109, -108, -107, -106, -105, -104, -103, -102, -101, -100, -99, -98, -97,
    -96, -95, -94, -93, -92, -91, -90, -89, -88, -87, -86, -85, -84, -83, -82, -81,
    -80, -79, -78, -77, -76, -75, -74, -73, -72, -71, -70, -69, -68, -67, -66, -65,
    -32, -31, -30, -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17,
    -16, -15, -14, -13, -12, -11, -10, -41, -8, -7, -6, -5, -4, -3, -2, -33,
    -32, -31, -30, -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17,
    -16, -15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1,
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
    -128, -127, -126, -125, -124, -123, -122, -121, -120, -119, -118, -117, -116, -115, -114, -113,
    -112, -111, -110, -109, -108, -107, -106, -105, -104, -103, -102, -101, -100, -99, -98, -97,
    -96, -95, -94, -93, -92, -91, -90, -89, -88, -87, -86, -85, -84, -83, -82, -81,
    -80, -79, -78, -77, -76, -75, -74, -73, -72, -71, -70, -69, -68, -67, -66, -65,
    -64, -63, -62, -61, -60, -59, -58, -57, -56, -55, -54, -53, -52, -51, -50, -49,
    -48, -47, -46, -45, -44, -43, -42, -41, -40, -39, -38, -37, -36, -35, -34, -33,
    -64, -63, -62, -61, -60, -59, -58, -57, -56, -55, -54, -53, -52, -51, -50, -49,
    -48, -47, -46, -45, -44, -43, -42, -9, -40, -39, -38, -37, -36, -35, -34, -1,
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

    lh_foreach(lh_char_t, p, str, n)
    {
        lh_char_t c = lh_ptr_deref(p);

#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
        *p = m_char_to_lower_dense[lh_cast_static(lh_uchar_t, c)];
#else
        lh_char_case_pair_t r;
        lh_bool_t is_founded = lh_bool_false;

        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_lower_table,
                                        m_char_to_lower_table_size, lh_cast_static(lh_usize_t, c),
                                        first, r, is_founded);

        *p = is_founded ? r.second : c;
#endif
    }

    return str + n;
}

lh_str_ptr
lh_str_ptr_to_upper(lh_str_ptr str, lh_usize_t n)
{
    lh_assert_runtime_ref(str);

    lh_foreach(lh_char_t, p, str, n)
    {
        lh_char_t c = lh_ptr_deref(p);

#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
        *p = m_char_to_upper_dense[lh_cast_static(lh_uchar_t, c)];
#else
        lh_char_case_pair_t r;
        lh_bool_t is_founded = lh_bool_false;

        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_upper_table,
                                        m_char_to_upper_table_size, lh_cast_static(lh_usize_t, c),
                                        first, r, is_founded);

        *p = is_founded ? r.second : c;
#endif
    }

    return str + n;
}
