#include <lh/util/str/ptr.h>
#include <lh/util/interval.h>
#include <lh/runtime/assert.h>
#include <lh/size.h>
#include <lh/char/case/pair.h>
#include <lh/util/array/raw.h>

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
static const lh_usize_t m_char_to_lower_table_size = lh_array_raw_get_size(m_char_to_lower_table);

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
    { 224U, 192U },
    { 225U, 193U },
    { 226U, 194U },
    { 227U, 195U },
    { 228U, 196U },
    { 229U, 197U },
    { 230U, 198U },
    { 231U, 199U },
    { 232U, 200U },
    { 233U, 201U },
    { 234U, 202U },
    { 235U, 203U },
    { 236U, 204U },
    { 237U, 205U },
    { 238U, 206U },
    { 239U, 207U },
    { 240U, 208U },
    { 241U, 209U },
    { 242U, 210U },
    { 243U, 211U },
    { 244U, 212U },
    { 245U, 213U },
    { 246U, 214U },
    { 248U, 216U },
    { 249U, 217U },
    { 250U, 218U },
    { 251U, 219U },
    { 252U, 220U },
    { 253U, 221U },
    { 254U, 222U },
    /* clang-format on */
};
static const lh_usize_t m_char_to_upper_table_size = lh_array_raw_get_size(m_char_to_upper_table);

lh_str_ptr
lh_str_ptr_to_lower(lh_str_ptr str, lh_usize_t n)
{
    lh_runtime_assert_ref(str);
    lh_str_ptr p = str;

    while (n--)
    {
        lh_char_t c = lh_ptr_deref(p);

        lh_char_case_pair_t r;
        lh_bool_t is_founded = lh_bool_false;

        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_lower_table,
                                        m_char_to_lower_table_size, c, first, r, is_founded);

        *p++ = is_founded ? r.second : c;
    }

    return p;
}

lh_str_ptr
lh_str_ptr_to_upper(lh_str_ptr str, lh_usize_t n)
{
    lh_runtime_assert_ref(str);
    lh_str_ptr p = str;

    while (n--)
    {
        lh_char_t c = lh_ptr_deref(p);

        lh_char_case_pair_t r;
        lh_bool_t is_founded = lh_bool_false;

        lh_interval_ropen_binary_search(lh_usize_t, m_char_to_upper_table,
                                        m_char_to_upper_table_size, c, first, r, is_founded);

        *p++ = is_founded ? r.second : c;
    }

    return p;
}
