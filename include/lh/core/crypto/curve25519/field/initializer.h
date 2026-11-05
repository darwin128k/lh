/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_crypto_curve25519_field_t.
 */

#ifndef LH_CRYPTO_CURVE25519_FIELD_INITIALIZER_H
#define LH_CRYPTO_CURVE25519_FIELD_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_crypto_curve25519_field_initializer(l0, l1, l2, l3, l4, l5, l6, l7, l8, l9)
 * @brief A field element from its ten limbs, lowest first: for constants.
 */
#define lh_crypto_curve25519_field_initializer(l0, l1, l2, l3, l4, l5, l6, l7, l8, l9)             \
    lh_initializer(lh_initializer(l0, l1, l2, l3, l4, l5, l6, l7, l8, l9))

#endif /* LH_CRYPTO_CURVE25519_FIELD_INITIALIZER_H */
