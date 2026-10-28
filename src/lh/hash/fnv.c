/**
 * @file fnv.c
 * @brief Implementation of `lh/hash/fnv.h`.
 */

#include <lh/hash/fnv.h>
#include <lh/byte/ptr.h>
#include <lh/math.h>

lh_u32_t
lh_hash_fnv1a_32_byte(lh_u32_t hash, lh_byte_t byte)
{
    return lh_math_mul(lh_bit_xor(hash, lh_cast_static(lh_u32_t, byte)), LH_HASH_FNV1A_32_PRIME);
}

lh_u32_t
lh_hash_fnv1a_32_update(lh_u32_t hash, const lh_ptr data, lh_usize_t size)
{
    const lh_byte_cptr bytes = lh_ptr_rcast(const lh_byte_t, data);
    lh_usize_t i;

    for (i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        hash = lh_hash_fnv1a_32_byte(hash, bytes[i]);
    }
    return hash;
}

lh_u32_t
lh_hash_fnv1a_32(const lh_ptr data, lh_usize_t size)
{
    return lh_hash_fnv1a_32_update(LH_HASH_FNV1A_32_OFFSET, data, size);
}
