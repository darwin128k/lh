/**
 * @file fields.h
 * @brief Member fields of ::lh_crypto_ed25519_point_t.
 */

#ifndef LH_CRYPTO_ED25519_POINT_FIELDS_H
#define LH_CRYPTO_ED25519_POINT_FIELDS_H

/**
 * @def lh_crypto_ed25519_point_fields(field_type)
 * @brief Extended projective coordinates: x = X / Z, y = Y / Z, T = X Y / Z.
 *
 * @param field_type ::lh_crypto_curve25519_field_t: `x`, `y`, `z`, `t`.
 */
#define lh_crypto_ed25519_point_fields(field_type)                                                 \
    field_type x;                                                                                  \
    field_type y;                                                                                  \
    field_type z;                                                                                  \
    field_type t

#endif /* LH_CRYPTO_ED25519_POINT_FIELDS_H */
