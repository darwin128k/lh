/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_crypto_ed25519_point_precomp_t.
 */

#ifndef LH_CRYPTO_ED25519_POINT_PRECOMP_INITIALIZER_H
#define LH_CRYPTO_ED25519_POINT_PRECOMP_INITIALIZER_H

#include <lh/core/crypto/curve25519/field/initializer.h>

/**
 * @def lh_crypto_ed25519_point_precomp_initializer(yp, ym, t2)
 * @brief A precomputed point from three field initializers (the base tables).
 */
#define lh_crypto_ed25519_point_precomp_initializer(yp, ym, t2) lh_initializer(yp, ym, t2)

#endif /* LH_CRYPTO_ED25519_POINT_PRECOMP_INITIALIZER_H */
