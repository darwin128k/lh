#include <gtest/gtest.h>

#include <lh/str/utf8.h>

namespace
{

constexpr lh_u32_t k_cyrillic = 0x041AU;

} // namespace

TEST(str_utf8, a_code_that_is_not_encodable_is_refused)
{
    lh_char_t out[4];

    EXPECT_EQ(lh_str_utf8_get_encoded_size(0), 0U);       /* the terminator */
    EXPECT_EQ(lh_str_utf8_get_encoded_size(0xD800U), 0U); /* a surrogate half */
    EXPECT_EQ(lh_str_utf8_get_encoded_size(0xDFFFU), 0U);
    EXPECT_EQ(lh_str_utf8_get_encoded_size(0x110000U), 0U); /* above the last one */
    EXPECT_EQ(lh_str_utf8_encode(out, 4U, 0xD800U), 0U);
    /* A code point that does not fit is refused whole, never half of one. */
    EXPECT_EQ(lh_str_utf8_encode(out, 1U, k_cyrillic), 0U);
    EXPECT_EQ(lh_str_utf8_get_encoded_size(k_cyrillic), 2U);
    EXPECT_EQ(lh_str_utf8_get_encoded_size(0x41U), 1U);
    EXPECT_EQ(lh_str_utf8_get_encoded_size(0x800U), 3U);
    EXPECT_EQ(lh_str_utf8_get_encoded_size(0x10000U), 4U);
}

TEST(str_utf8, the_size_of_a_character_is_read_off_its_lead_byte_and_not_off_its_code)
{
    /* The two questions, side by side, because answering one with the other is the
       bug: `0xD0` and `0xE0` are the lead bytes of a two-byte and a three-byte
       character, and **as code points both are below 0x800**, so the wrong function
       says 2 for both. It is right about the Cyrillic one and wrong about the euro. */
    EXPECT_EQ(lh_str_utf8_get_lead_size(0x41U), 1U); /* 'A' */
    EXPECT_EQ(lh_str_utf8_get_lead_size(0x7FU), 1U); /* DEL, still one byte */
    EXPECT_EQ(lh_str_utf8_get_lead_size(0xD0U), 2U); /* U+041A */
    EXPECT_EQ(lh_str_utf8_get_lead_size(0xE0U), 3U); /* U+20AC */
    EXPECT_EQ(lh_str_utf8_get_lead_size(0xEDU), 3U); /* U+D000 range, still three */
    EXPECT_EQ(lh_str_utf8_get_lead_size(0xF0U), 4U); /* U+1F600 */
    EXPECT_EQ(lh_str_utf8_get_lead_size(0xF4U), 4U);

    /* A continuation byte, a stray byte above the range and a zero are one byte:
       the walk is one byte at a time over these, and skipping more would step over a
       character somebody can see. */
    EXPECT_EQ(lh_str_utf8_get_lead_size(0x80U), 1U);
    EXPECT_EQ(lh_str_utf8_get_lead_size(0xBFU), 1U);
    EXPECT_EQ(lh_str_utf8_get_lead_size(0xF8U), 1U);
    EXPECT_EQ(lh_str_utf8_get_lead_size(0x00U), 1U);

    /* And the honest disagreement, spelled out: what the two functions say about the
       three bytes that are not ASCII. */
    EXPECT_EQ(lh_str_utf8_get_encoded_size(0xD0U), 2U); /* agrees by coincidence */
    EXPECT_EQ(lh_str_utf8_get_encoded_size(0xE0U), 2U); /* **wrong**, and the reason */
    EXPECT_EQ(lh_str_utf8_get_lead_size(0xE0U), 3U);    /* the answer */
}
