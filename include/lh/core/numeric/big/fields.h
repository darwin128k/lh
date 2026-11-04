/**
 * @file fields.h
 * @brief Member fields of ::lh_numeric_big_t.
 */

#ifndef LH_NUMERIC_BIG_FIELDS_H
#define LH_NUMERIC_BIG_FIELDS_H

/**
 * @def lh_numeric_big_fields(word_type, word_count)
 * @brief The words of an unsigned big number, least significant first.
 *
 * @param word_type  Type of one word (::lh_u32_t).
 * @param word_count Number of words (::LH_NUMERIC_BIG_WORDS).
 */
#define lh_numeric_big_fields(word_type, word_count) word_type words[word_count]

#endif /* LH_NUMERIC_BIG_FIELDS_H */
