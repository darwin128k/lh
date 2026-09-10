#include <lh/util/char.h>
#include <lh/util/interval.h>
#include <lh/uchar.h>
#include <lh/size.h>
#include <lh/char/case/pair.h>
#include <lh/util/array/ptr.h>
#include <lh/cast/static.h>

static const lh_char_case_pair_t m_char_case_fold_table[] = {
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
static const lh_usize_t m_char_case_fold_table_size = lh_array_ptr_get_size(m_char_case_fold_table);

lh_char_t
lh_char_fold_case(lh_char_t c)
{
    lh_char_case_pair_t r;
    lh_bool_t is_founded = lh_bool_false;

    /* Key cast to lh_usize_t (the macro's own `type`) so it undergoes the same signed -> unsigned
     * conversion as the table's lh_char_t field does inside the macro; matches by construction
     * instead of mixing signed/unsigned operands in the comparison. */
    lh_interval_ropen_binary_search(lh_usize_t, m_char_case_fold_table, m_char_case_fold_table_size,
                                    lh_cast_static(lh_usize_t, c), first, r, is_founded);

    if (is_founded)
    {
        if (lh_math_gt(r.second, LH_CHAR_T_MAX))
        {
            return c;
        }
        return r.second;
    }
    return c;
}
