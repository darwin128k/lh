#include <lh/util/char.h>
#include <lh/util/interval.h>
#include <lh/util/math.h>
#include <lh/uchar.h>
#include <lh/size.h>
#include <lh/char/case/pair.h>
#include <lh/util/array/raw.h>

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
    { 192U, 224U },
    { 193U, 225U },
    { 194U, 226U },
    { 195U, 227U },
    { 196U, 228U },
    { 197U, 229U },
    { 198U, 230U },
    { 199U, 231U },
    { 200U, 232U },
    { 201U, 233U },
    { 202U, 234U },
    { 203U, 235U },
    { 204U, 236U },
    { 205U, 237U },
    { 206U, 238U },
    { 207U, 239U },
    { 208U, 240U },
    { 209U, 241U },
    { 210U, 242U },
    { 211U, 243U },
    { 212U, 244U },
    { 213U, 245U },
    { 214U, 246U },
    { 216U, 248U },
    { 217U, 249U },
    { 218U, 250U },
    { 219U, 251U },
    { 220U, 252U },
    { 221U, 253U },
    { 222U, 254U },
    /* clang-format on */
};
static const lh_usize_t m_char_case_fold_table_size = lh_array_raw_get_size(m_char_case_fold_table);

lh_char_t
lh_char_fold_case(lh_char_t c)
{
    lh_char_case_pair_t r;
    lh_bool_t is_founded = lh_bool_false;

    lh_interval_ropen_binary_search(lh_usize_t, m_char_case_fold_table, m_char_case_fold_table_size,
                                    c, first, r, is_founded);

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
