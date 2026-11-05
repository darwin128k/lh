/**
 * @file fields.h
 * @brief Member fields of ::lh_crypto_ed25519_slide_t.
 */

#ifndef LH_CRYPTO_ED25519_SLIDE_FIELDS_H
#define LH_CRYPTO_ED25519_SLIDE_FIELDS_H

/**
 * @def lh_crypto_ed25519_slide_fields(index_type, digit_type, check_type)
 * @brief Where the next signed digit of a sliding window lies.
 *
 * @param index_type ::lh_s16_t: `next_index`, the bit position of the next digit.
 * @param digit_type ::lh_s8_t: `next_digit`, that digit (odd, below 2^width).
 * @param check_type ::lh_u8_t: `next_check`, the bit at which to look for the
 *                   next window.
 */
#define lh_crypto_ed25519_slide_fields(index_type, digit_type, check_type)                         \
    index_type next_index;                                                                         \
    digit_type next_digit;                                                                         \
    check_type next_check

#endif /* LH_CRYPTO_ED25519_SLIDE_FIELDS_H */
