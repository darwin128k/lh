/**
 * @file fields.h
 * @brief Member fields of ::lh_crypto_ed25519_point_cached_t.
 */

#ifndef LH_CRYPTO_ED25519_POINT_CACHED_FIELDS_H
#define LH_CRYPTO_ED25519_POINT_CACHED_FIELDS_H

/**
 * @def lh_crypto_ed25519_point_cached_fields(field_type)
 * @brief A point ready to add: `yp` = Y + X, `ym` = Y - X, `z` = Z, `t2` = 2 d T.
 *
 * @param field_type ::lh_crypto_curve25519_field_t.
 */
#define lh_crypto_ed25519_point_cached_fields(field_type)                                          \
    field_type yp;                                                                                 \
    field_type ym;                                                                                 \
    field_type z;                                                                                  \
    field_type t2

#endif /* LH_CRYPTO_ED25519_POINT_CACHED_FIELDS_H */
