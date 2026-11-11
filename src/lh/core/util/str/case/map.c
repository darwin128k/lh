/**
 * @file map.c
 * @brief ::lh_str_ptr_to_lower / ::lh_str_ptr_to_upper: case mapping of a buffer in place.
 *
 * Built on ::lh_char_to_lower / ::lh_char_to_upper (lh/core/util/char/case/map.h),
 * which own the Unicode tables. Words of pure ASCII are mapped eight (or four) bytes
 * at a time with SWAR: the case bit is set or cleared in the lanes that hold a letter.
 * Unaligned edges and words with a byte from 128..255 go one character at a time.
 */

#include <lh/core/util/str/case/map.h>
#include <lh/core/assert.h>
#include <lh/core/bit/swar.h>
#include <lh/core/memory/word.h>
#include <lh/core/util/char/case/map.h>

lh_str_ptr
lh_str_ptr_to_lower(lh_str_ptr str, lh_usize_t n)
{
    lh_assert_runtime_ref(str);

    {
        const lh_usize_t w = sizeof(lh_usize_t);
        lh_byte_ptr p = lh_ptr_cast(lh_byte_t, str);
        lh_byte_ptr end = p + n;

        while (p < end && !lh_ptr_is_aligned(p, w))
        {
            lh_ptr_deref(lh_ptr_rcast(lh_char_t, p)) =
                lh_char_to_lower(lh_ptr_deref(lh_ptr_rcast(lh_char_t, p)));
            ++p;
        }
        {
            const lh_usize_t sub_a =
                lh_bit_swar_broadcast(lh_cast_static(lh_usize_t, lh_char_map_capital_a));
            const lh_usize_t add_z = lh_bit_swar_broadcast(
                lh_cast_static(lh_usize_t, lh_char_map_del - lh_char_map_capital_z));

            while (p + (w + w) <= end)
            {
                const lh_usize_t v0 = lh_memory_word_load(p);
                const lh_usize_t v1 = lh_memory_word_load(p + w);

                if (lh_math_is_zero(lh_bit_and(lh_bit_or(v0, v1), LH_BIT_SWAR_HIGH)))
                {
                    /* 0x20, the ASCII case bit, in the lanes that hold a letter. */
                    const lh_usize_t case0 = lh_bit_shr(
                        lh_bit_swar_unmarked(lh_bit_swar_outside_mask(v0, sub_a, add_z)), 2U);
                    const lh_usize_t case1 = lh_bit_shr(
                        lh_bit_swar_unmarked(lh_bit_swar_outside_mask(v1, sub_a, add_z)), 2U);

                    lh_memory_word_store(p, lh_bit_or(v0, case0));
                    lh_memory_word_store(p + w, lh_bit_or(v1, case1));
                    p += w + w;
                    continue;
                }
                break;
            }
            while (p + w <= end)
            {
                const lh_usize_t v = lh_memory_word_load(p);
                lh_usize_t lane;

                if (lh_math_is_zero(lh_bit_and(v, LH_BIT_SWAR_HIGH)))
                {
                    const lh_usize_t case_bits = lh_bit_shr(
                        lh_bit_swar_unmarked(lh_bit_swar_outside_mask(v, sub_a, add_z)), 2U);

                    lh_memory_word_store(p, lh_bit_or(v, case_bits));
                }
                else
                {
                    for (lane = 0U; lane < w; ++lane)
                    {
                        lh_ptr_deref(lh_ptr_rcast(lh_char_t, p + lane)) =
                            lh_char_to_lower(lh_ptr_deref(lh_ptr_rcast(lh_char_t, p + lane)));
                    }
                }
                p += w;
            }
        }
        while (p < end)
        {
            lh_ptr_deref(lh_ptr_rcast(lh_char_t, p)) =
                lh_char_to_lower(lh_ptr_deref(lh_ptr_rcast(lh_char_t, p)));
            ++p;
        }
    }

    return str + n;
}

lh_str_ptr
lh_str_ptr_to_upper(lh_str_ptr str, lh_usize_t n)
{
    lh_assert_runtime_ref(str);

    {
        const lh_usize_t w = sizeof(lh_usize_t);
        lh_byte_ptr p = lh_ptr_cast(lh_byte_t, str);
        lh_byte_ptr end = p + n;

        while (p < end && !lh_ptr_is_aligned(p, w))
        {
            lh_ptr_deref(lh_ptr_rcast(lh_char_t, p)) =
                lh_char_to_upper(lh_ptr_deref(lh_ptr_rcast(lh_char_t, p)));
            ++p;
        }
        {
            const lh_usize_t sub_a =
                lh_bit_swar_broadcast(lh_cast_static(lh_usize_t, lh_char_map_small_a));
            const lh_usize_t add_z = lh_bit_swar_broadcast(
                lh_cast_static(lh_usize_t, lh_char_map_del - lh_char_map_small_z));

            while (p + (w + w) <= end)
            {
                const lh_usize_t v0 = lh_memory_word_load(p);
                const lh_usize_t v1 = lh_memory_word_load(p + w);

                if (lh_math_is_zero(lh_bit_and(lh_bit_or(v0, v1), LH_BIT_SWAR_HIGH)))
                {
                    /* 0x20, the ASCII case bit, in the lanes that hold a letter. */
                    const lh_usize_t case0 = lh_bit_shr(
                        lh_bit_swar_unmarked(lh_bit_swar_outside_mask(v0, sub_a, add_z)), 2U);
                    const lh_usize_t case1 = lh_bit_shr(
                        lh_bit_swar_unmarked(lh_bit_swar_outside_mask(v1, sub_a, add_z)), 2U);

                    lh_memory_word_store(p, lh_bit_and(v0, lh_bit_not(case0)));
                    lh_memory_word_store(p + w, lh_bit_and(v1, lh_bit_not(case1)));
                    p += w + w;
                    continue;
                }
                break;
            }
            while (p + w <= end)
            {
                const lh_usize_t v = lh_memory_word_load(p);
                lh_usize_t lane;

                if (lh_math_is_zero(lh_bit_and(v, LH_BIT_SWAR_HIGH)))
                {
                    const lh_usize_t case_bits = lh_bit_shr(
                        lh_bit_swar_unmarked(lh_bit_swar_outside_mask(v, sub_a, add_z)), 2U);

                    lh_memory_word_store(p, lh_bit_and(v, lh_bit_not(case_bits)));
                }
                else
                {
                    for (lane = 0U; lane < w; ++lane)
                    {
                        lh_ptr_deref(lh_ptr_rcast(lh_char_t, p + lane)) =
                            lh_char_to_upper(lh_ptr_deref(lh_ptr_rcast(lh_char_t, p + lane)));
                    }
                }
                p += w;
            }
        }
        while (p < end)
        {
            lh_ptr_deref(lh_ptr_rcast(lh_char_t, p)) =
                lh_char_to_upper(lh_ptr_deref(lh_ptr_rcast(lh_char_t, p)));
            ++p;
        }
    }

    return str + n;
}
