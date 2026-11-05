/**
 * @file fields.h
 * @brief Member fields of ::lh_crypto_curve25519_field_t.
 */

#ifndef LH_CRYPTO_CURVE25519_FIELD_FIELDS_H
#define LH_CRYPTO_CURVE25519_FIELD_FIELDS_H

/**
 * @def LH_CRYPTO_CURVE25519_FIELD_LIMBS
 * @brief Limbs of a field element: 10, alternately 26 and 25 bits wide.
 */
#define LH_CRYPTO_CURVE25519_FIELD_LIMBS 10

/**
 * @def lh_crypto_curve25519_field_fields(limb_type)
 * @brief The limbs of a field element.
 *
 * @param limb_type ::lh_s32_t: `limb`, ::LH_CRYPTO_CURVE25519_FIELD_LIMBS signed
 *                  limbs; the value is `limb[0] + limb[1]*2^26 + limb[2]*2^51 + ...`
 *                  (radix 2^25.5), each limb kept small but not fully reduced.
 */
#define lh_crypto_curve25519_field_fields(limb_type)                                               \
    limb_type limb[LH_CRYPTO_CURVE25519_FIELD_LIMBS]

#endif /* LH_CRYPTO_CURVE25519_FIELD_FIELDS_H */
