/**
 * @file fields.h
 * @brief Member fields of ::lh_crypto_ed25519_point_precomp_t.
 */

#ifndef LH_CRYPTO_ED25519_POINT_PRECOMP_FIELDS_H
#define LH_CRYPTO_ED25519_POINT_PRECOMP_FIELDS_H

/**
 * @def lh_crypto_ed25519_point_precomp_fields(field_type)
 * @brief A point with Z = 1 ready to add: `yp` = y + x, `ym` = y - x,
 *        `t2` = 2 d x y.
 *
 * @param field_type ::lh_crypto_curve25519_field_t.
 */
#define lh_crypto_ed25519_point_precomp_fields(field_type)                                         \
    field_type yp;                                                                                 \
    field_type ym;                                                                                 \
    field_type t2

#endif /* LH_CRYPTO_ED25519_POINT_PRECOMP_FIELDS_H */
