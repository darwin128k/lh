/**
 * @file precomp.h
 * @brief ::lh_crypto_ed25519_point_precomp_t — a point with Z = 1, precomputed
 *        for addition (Niels coordinates).
 *
 * The form of the base point's multiples in the tables of
 * lh/core/crypto/ed25519/base.h; added with ::lh_crypto_ed25519_point_madd.
 * A value type with no owner.
 */

#ifndef LH_CRYPTO_ED25519_POINT_PRECOMP_H
#define LH_CRYPTO_ED25519_POINT_PRECOMP_H

#include <lh/core/crypto/curve25519/field.h>
#include <lh/core/crypto/ed25519/point/precomp/fields.h>

/**
 * @brief A point with Z = 1 ready to add.
 */
struct lh_crypto_ed25519_point_precomp
{
    /** Members: ::lh_crypto_ed25519_point_precomp_fields. */
    lh_crypto_ed25519_point_precomp_fields(lh_crypto_curve25519_field_t);
};

/** @copybrief lh_crypto_ed25519_point_precomp */
typedef struct lh_crypto_ed25519_point_precomp lh_crypto_ed25519_point_precomp_t;

#endif /* LH_CRYPTO_ED25519_POINT_PRECOMP_H */
