#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/null.h>
#include <lh/util/str/raw.h>

namespace {

TEST(str_raw_to_lower, maps_prefix_in_place) {
    lh_char_t buf[] = {'H', 'i', '!', '\0'};
    lh_str_cptr end = lh_str_raw_to_lower(buf, 3);
    EXPECT_EQ(end, buf + 3);
    EXPECT_EQ(buf[0], 'h');
    EXPECT_EQ(buf[1], 'i');
    EXPECT_EQ(buf[2], '!');
}

TEST(str_raw_to_upper, maps_prefix_in_place) {
    lh_char_t buf[] = {'a', 'B', '\0'};
    lh_str_cptr end = lh_str_raw_to_upper(buf, 2);
    EXPECT_EQ(end, buf + 2);
    EXPECT_EQ(buf[0], 'A');
    EXPECT_EQ(buf[1], 'B');
}

TEST(str_raw_to_lower, zero_length_is_no_op) {
    lh_char_t buf[] = {'X', 'Y', '\0'};
    lh_str_cptr end = lh_str_raw_to_lower(buf, 0);
    EXPECT_EQ(end, buf);
    EXPECT_EQ(buf[0], 'X');
}

TEST(str_raw_to_lower_upper, ascii_roundtrip) {
    lh_char_t buf[] = {'M', '\0'};
    lh_str_raw_to_lower(buf, 1);
    EXPECT_EQ(buf[0], 'm');
    lh_str_raw_to_upper(buf, 1);
    EXPECT_EQ(buf[0], 'M');
}

TEST(str_raw_to_lower_upper, digits_unchanged) {
    lh_char_t buf[] = {'1', 'a', 'B', '\0'};
    lh_str_raw_to_lower(buf, 3);
    EXPECT_EQ(buf[0], '1');
    EXPECT_EQ(buf[1], 'a');
    EXPECT_EQ(buf[2], 'b');
    lh_str_raw_to_upper(buf, 3);
    EXPECT_EQ(buf[0], '1');
    EXPECT_EQ(buf[1], 'A');
    EXPECT_EQ(buf[2], 'B');
}

TEST(str_raw_to_lower, latin1_capital_letters_to_lower) {
    lh_char_t buf[] = {'\xC0', '\xC1', '\xD6', '\0'}; // À Á Ö
    lh_str_raw_to_lower(buf, 3);
    EXPECT_EQ(static_cast<unsigned char>(buf[0]), 224U); // à
    EXPECT_EQ(static_cast<unsigned char>(buf[1]), 225U); // á
    EXPECT_EQ(static_cast<unsigned char>(buf[2]), 246U); // ö
}

TEST(str_raw_to_upper, latin1_small_letters_to_upper) {
    lh_char_t buf[] = {'\xE0', '\xE1', '\xF6', '\0'}; // à á ö
    lh_str_raw_to_upper(buf, 3);
    EXPECT_EQ(static_cast<unsigned char>(buf[0]), 192U);
    EXPECT_EQ(static_cast<unsigned char>(buf[1]), 193U);
    EXPECT_EQ(static_cast<unsigned char>(buf[2]), 214U);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(str_raw_to_lower_death, null_buffer) {
    LH_EXPECT_DEATH(lh_str_raw_to_lower(reinterpret_cast<lh_str_ptr>(lh_null), 0));
}

TEST(str_raw_to_upper_death, null_buffer) {
    LH_EXPECT_DEATH(lh_str_raw_to_upper(reinterpret_cast<lh_str_ptr>(lh_null), 0));
}

#endif

} // namespace
