#include <gtest/gtest.h>

#include <lh/util/wchar.h>
#include <lh/util/wstr/ptr.h>

namespace
{

TEST(wchar_case_map, ascii_lower_upper_roundtrip)
{
    EXPECT_EQ(lh_wchar_to_lower(L'A'), L'a');
    EXPECT_EQ(lh_wchar_to_lower(L'z'), L'z');
    EXPECT_EQ(lh_wchar_to_upper(L'a'), L'A');
    EXPECT_EQ(lh_wchar_to_upper(L'9'), L'9');
    EXPECT_EQ(lh_wchar_to_upper(lh_wchar_to_lower(L'M')), L'M');
}

TEST(wchar_case_map, cyrillic_simple_mapping)
{
    const lh_wchar_t cap = static_cast<lh_wchar_t>(0x0410);
    const lh_wchar_t low = static_cast<lh_wchar_t>(0x0430);
    EXPECT_EQ(lh_wchar_to_lower(cap), low);
    EXPECT_EQ(lh_wchar_to_upper(low), cap);
}

TEST(wstr_raw_to_lower, maps_prefix_in_place)
{
    lh_wchar_t buf[] = {L'H', L'i', L'!', L'\0'};
    lh_wstr_cptr end = lh_wstr_ptr_to_lower(buf, 3);
    EXPECT_EQ(end, buf + 3);
    EXPECT_EQ(buf[0], L'h');
    EXPECT_EQ(buf[1], L'i');
    EXPECT_EQ(buf[2], L'!');
}

TEST(wstr_raw_to_upper, maps_prefix_in_place)
{
    lh_wchar_t buf[] = {L'a', L'B', L'\0'};
    lh_wstr_cptr end = lh_wstr_ptr_to_upper(buf, 2);
    EXPECT_EQ(end, buf + 2);
    EXPECT_EQ(buf[0], L'A');
    EXPECT_EQ(buf[1], L'B');
}

} // namespace
