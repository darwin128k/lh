/*
 * AUTO-GENERATED — do not edit. Regenerate with tools/gen_char_case_map.py.
 * Source: Unicode UCD UnicodeData.txt (Unicode 16.0.0), fields 12-13
 * (simple uppercase / simple lowercase), restricted to source and mapped scalars
 * both in 0..255 (lh_char_t is one code unit) — see the script docstring for what
 * that excludes (e.g. MICRO SIGN, LATIN SMALL LETTER Y WITH DIAERESIS).
 *
 * Unicode data copyright Unicode, Inc.; see https://www.unicode.org/terms_of_use.html
 *
 * Only 128..255: ASCII is mapped inline (lh/core/util/char/case/map.h). Two
 * implementations selected by LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
 * (include/lh/config.h) — see that option's doc comment in cmake/library_options.cmake.
 */

#include <lh/core/util/char/case/map.h>
#include <lh/config.h>
#include <lh/core/math.h>

#if LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE

static const lh_char_t m_char_to_lower_dense[128] = {
    /* clang-format off */
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

static const lh_char_t m_char_to_upper_dense[128] = {
    /* clang-format off */
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

lh_char_t
lh_char_to_lower_nonascii(lh_char_t c)
{
    return m_char_to_lower_dense[lh_math_sub(lh_cast_static(lh_uchar_t, c), 128U)];
}

lh_char_t
lh_char_to_upper_nonascii(lh_char_t c)
{
    return m_char_to_upper_dense[lh_math_sub(lh_cast_static(lh_uchar_t, c), 128U)];
}

#else /* !LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE */

#    include <lh/core/char/case/pair.h>
#    include <lh/core/util/array/ptr.h>
#    include <lh/core/util/interval.h>

static const lh_char_case_pair_t m_char_to_lower_table[] = {
    /* clang-format off */
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

static const lh_char_case_pair_t m_char_to_upper_table[] = {
    /* clang-format off */
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

lh_char_t
lh_char_to_lower_nonascii(lh_char_t c)
{
    lh_char_case_pair_t r;
    lh_bool_t is_founded = lh_bool_false;

    lh_interval_ropen_binary_search(lh_usize_t, m_char_to_lower_table,
                                    lh_array_ptr_get_size(m_char_to_lower_table),
                                    lh_cast_static(lh_usize_t, c), first, r, is_founded);
    return is_founded ? r.second : c;
}

lh_char_t
lh_char_to_upper_nonascii(lh_char_t c)
{
    lh_char_case_pair_t r;
    lh_bool_t is_founded = lh_bool_false;

    lh_interval_ropen_binary_search(lh_usize_t, m_char_to_upper_table,
                                    lh_array_ptr_get_size(m_char_to_upper_table),
                                    lh_cast_static(lh_usize_t, c), first, r, is_founded);
    return is_founded ? r.second : c;
}

#endif /* LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE */
