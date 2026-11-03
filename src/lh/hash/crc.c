/**
 * @file crc.c
 * @brief Reflected CRC kernels and gzip CRC-32.
 */

#include <lh/hash/crc.h>
#include <lh/byte/ptr.h>
#include <lh/math.h>

lh_u32_t
lh_hash_crc_reflected_byte(lh_u32_t crc, lh_byte_t byte, lh_u32_t polynomial)
{
    crc = lh_bit_xor(crc, lh_cast_static(lh_u32_t, byte));
    for (lh_uint_t bit = 0U; lh_math_lt(bit, 8U); bit = lh_math_add_one(bit))
    {
        const lh_bool_t low = !lh_math_is_zero(lh_bit_and(crc, 1U));
        crc = lh_bit_shr(crc, 1U);
        if (low)
        {
            crc = lh_bit_xor(crc, polynomial);
        }
    }
    return crc;
}

lh_u32_t
lh_hash_crc_reflected_update(lh_u32_t crc, const lh_ptr data, lh_usize_t size, lh_u32_t polynomial)
{
    const lh_byte_cptr bytes = lh_ptr_rcast(const lh_byte_t, data);
    for (lh_usize_t i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        crc = lh_hash_crc_reflected_byte(crc, bytes[i], polynomial);
    }
    return crc;
}

lh_u32_t
lh_hash_crc32_update(lh_u32_t crc, const lh_ptr data, lh_usize_t size)
{
    return lh_bit_xor(lh_hash_crc_reflected_update(lh_bit_xor(crc, 0xFFFFFFFFU), data, size,
                                                   LH_HASH_CRC32_POLYNOMIAL),
                      0xFFFFFFFFU);
}

lh_u32_t
lh_hash_crc32(const lh_ptr data, lh_usize_t size)
{
    return lh_hash_crc32_update(0U, data, size);
}
