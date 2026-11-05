/**
 * @file cached.h
 * @brief ::lh_crypto_ed25519_point_cached_t — a point prepared for addition.
 *
 * Made from an ::lh_crypto_ed25519_point_t by ::lh_crypto_ed25519_point_cache,
 * added with ::lh_crypto_ed25519_point_add and ::lh_crypto_ed25519_point_sub.
 * A value type with no owner.
 */

#ifndef LH_CRYPTO_ED25519_POINT_CACHED_H
#define LH_CRYPTO_ED25519_POINT_CACHED_H

#include <lh/core/crypto/curve25519/field.h>
#include <lh/core/crypto/ed25519/point/cached/fields.h>

/**
 * @brief A point prepared for addition.
 */
struct lh_crypto_ed25519_point_cached
{
    /** Members: ::lh_crypto_ed25519_point_cached_fields. */
    lh_crypto_ed25519_point_cached_fields(lh_crypto_curve25519_field_t);
};

/** @copybrief lh_crypto_ed25519_point_cached */
typedef struct lh_crypto_ed25519_point_cached lh_crypto_ed25519_point_cached_t;

#endif /* LH_CRYPTO_ED25519_POINT_CACHED_H */
