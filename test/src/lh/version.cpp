#include <gtest/gtest.h>

#include <string>

#include <lh/version.h>
#include <lh/version/initializer.h>

namespace
{

TEST(version_set, roundtrip_via_getters)
{
    lh_version_t ver{};
    lh_version_set(&ver, 2, 5, 9);
    EXPECT_EQ(lh_version_get_major(&ver), 2);
    EXPECT_EQ(lh_version_get_minor(&ver), 5);
    EXPECT_EQ(lh_version_get_patch(&ver), 9);
}

TEST(version_set, matches_pack_with_all_pointers)
{
    lh_version_t a{};
    lh_version_t b{};
    lh_version_major_t maj = 10;
    lh_version_minor_t min = 20;
    lh_version_patch_t pat = 30;
    lh_version_set(&a, maj, min, pat);
    lh_version_pack(&b, &maj, &min, &pat);
    EXPECT_EQ(lh_version_get_major(&a), lh_version_get_major(&b));
    EXPECT_EQ(lh_version_get_minor(&a), lh_version_get_minor(&b));
    EXPECT_EQ(lh_version_get_patch(&a), lh_version_get_patch(&b));
}

TEST(version_pack, updates_only_major_when_others_null)
{
    lh_version_t ver = lh_version_initializer(1, 2, 3);
    lh_version_major_t new_major = 99;
    lh_version_pack(&ver, &new_major, nullptr, nullptr);
    EXPECT_EQ(lh_version_get_major(&ver), 99);
    EXPECT_EQ(lh_version_get_minor(&ver), 2);
    EXPECT_EQ(lh_version_get_patch(&ver), 3);
}

TEST(version_pack, updates_only_minor_when_others_null)
{
    lh_version_t ver = lh_version_initializer(1, 2, 3);
    lh_version_minor_t new_minor = 88;
    lh_version_pack(&ver, nullptr, &new_minor, nullptr);
    EXPECT_EQ(lh_version_get_major(&ver), 1);
    EXPECT_EQ(lh_version_get_minor(&ver), 88);
    EXPECT_EQ(lh_version_get_patch(&ver), 3);
}

TEST(version_pack, updates_only_patch_when_others_null)
{
    lh_version_t ver = lh_version_initializer(1, 2, 3);
    lh_version_patch_t new_patch = 77;
    lh_version_pack(&ver, nullptr, nullptr, &new_patch);
    EXPECT_EQ(lh_version_get_major(&ver), 1);
    EXPECT_EQ(lh_version_get_minor(&ver), 2);
    EXPECT_EQ(lh_version_get_patch(&ver), 77);
}

TEST(version_pack, no_op_when_all_component_pointers_null)
{
    lh_version_t ver = lh_version_initializer(4, 5, 6);
    lh_version_pack(&ver, nullptr, nullptr, nullptr);
    EXPECT_EQ(lh_version_get_major(&ver), 4);
    EXPECT_EQ(lh_version_get_minor(&ver), 5);
    EXPECT_EQ(lh_version_get_patch(&ver), 6);
}

TEST(version_unpack, skips_null_output_pointers)
{
    const lh_version_t ver = lh_version_initializer(7, 8, 9);
    lh_version_major_t major = 0;
    lh_version_unpack(&ver, &major, nullptr, nullptr);
    EXPECT_EQ(major, 7);

    lh_version_minor_t minor = 0;
    lh_version_unpack(&ver, nullptr, &minor, nullptr);
    EXPECT_EQ(minor, 8);

    lh_version_patch_t patch = 0;
    lh_version_unpack(&ver, nullptr, nullptr, &patch);
    EXPECT_EQ(patch, 9);
}

TEST(version_unpack, writes_all_outputs_when_non_null)
{
    const lh_version_t ver = lh_version_initializer(11, 12, 13);
    lh_version_major_t major = 0;
    lh_version_minor_t minor = 0;
    lh_version_patch_t patch = 0;
    lh_version_unpack(&ver, &major, &minor, &patch);
    EXPECT_EQ(major, 11);
    EXPECT_EQ(minor, 12);
    EXPECT_EQ(patch, 13);
}

TEST(version_is_at_least, greater_major)
{
    const lh_version_t device = lh_version_initializer(2, 0, 0);
    const lh_version_t min_req = lh_version_initializer(1, 9, 9);
    EXPECT_EQ(lh_version_is_at_least(&device, &min_req), lh_bool_true);
}

TEST(version_is_at_least, same_major_greater_minor)
{
    const lh_version_t device = lh_version_initializer(1, 5, 0);
    const lh_version_t min_req = lh_version_initializer(1, 4, 9);
    EXPECT_EQ(lh_version_is_at_least(&device, &min_req), lh_bool_true);
}

TEST(version_is_at_least, same_major_minor_greater_patch)
{
    const lh_version_t device = lh_version_initializer(1, 2, 10);
    const lh_version_t min_req = lh_version_initializer(1, 2, 9);
    EXPECT_EQ(lh_version_is_at_least(&device, &min_req), lh_bool_true);
}

TEST(version_is_at_least, equal_is_accepted)
{
    const lh_version_t device = lh_version_initializer(3, 4, 5);
    const lh_version_t min_req = lh_version_initializer(3, 4, 5);
    EXPECT_EQ(lh_version_is_at_least(&device, &min_req), lh_bool_true);
}

TEST(version_is_at_least, lower_major_rejected)
{
    const lh_version_t device = lh_version_initializer(0, 9, 9);
    const lh_version_t min_req = lh_version_initializer(1, 0, 0);
    EXPECT_EQ(lh_version_is_at_least(&device, &min_req), lh_bool_false);
}

TEST(version_is_at_least, same_major_lower_minor_rejected)
{
    const lh_version_t device = lh_version_initializer(2, 0, 99);
    const lh_version_t min_req = lh_version_initializer(2, 1, 0);
    EXPECT_EQ(lh_version_is_at_least(&device, &min_req), lh_bool_false);
}

TEST(version_is_at_least, same_major_minor_lower_patch_rejected)
{
    const lh_version_t device = lh_version_initializer(1, 0, 0);
    const lh_version_t min_req = lh_version_initializer(1, 0, 1);
    EXPECT_EQ(lh_version_is_at_least(&device, &min_req), lh_bool_false);
}

TEST(version_equals, matching_components)
{
    const lh_version_t a = lh_version_initializer(1, 2, 3);
    const lh_version_t b = lh_version_initializer(1, 2, 3);
    EXPECT_EQ(lh_version_equals(&a, &b), lh_bool_true);
}

TEST(version_equals, mismatch_rejected)
{
    const lh_version_t a = lh_version_initializer(1, 2, 3);
    const lh_version_t b = lh_version_initializer(1, 2, 4);
    EXPECT_EQ(lh_version_equals(&a, &b), lh_bool_false);
}

TEST(version_is_compatible, stable_same_major_at_least)
{
    const lh_version_t have = lh_version_initializer(1, 4, 0);
    const lh_version_t need = lh_version_initializer(1, 2, 9);
    EXPECT_EQ(lh_version_is_compatible(&have, &need), lh_bool_true);
}

TEST(version_is_compatible, stable_lower_minor_rejected)
{
    const lh_version_t have = lh_version_initializer(1, 1, 9);
    const lh_version_t need = lh_version_initializer(1, 2, 0);
    EXPECT_EQ(lh_version_is_compatible(&have, &need), lh_bool_false);
}

TEST(version_is_compatible, stable_other_major_rejected)
{
    const lh_version_t have = lh_version_initializer(2, 0, 0);
    const lh_version_t need = lh_version_initializer(1, 9, 9);
    EXPECT_EQ(lh_version_is_compatible(&have, &need), lh_bool_false);
}

TEST(version_is_compatible, zero_major_same_minor_newer_patch)
{
    const lh_version_t have = lh_version_initializer(0, 3, 1);
    const lh_version_t need = lh_version_initializer(0, 3, 0);
    EXPECT_EQ(lh_version_is_compatible(&have, &need), lh_bool_true);
}

TEST(version_is_compatible, zero_major_other_minor_rejected)
{
    const lh_version_t have = lh_version_initializer(0, 4, 0);
    const lh_version_t need = lh_version_initializer(0, 3, 0);
    EXPECT_EQ(lh_version_is_compatible(&have, &need), lh_bool_false);
}

TEST(version_is_compatible, zero_major_older_patch_rejected)
{
    const lh_version_t have = lh_version_initializer(0, 3, 0);
    const lh_version_t need = lh_version_initializer(0, 3, 1);
    EXPECT_EQ(lh_version_is_compatible(&have, &need), lh_bool_false);
}

TEST(version_parse, parses_core_semver)
{
    lh_version_t ver{};
    ASSERT_EQ(lh_version_parse("1.2.3", 5, &ver), lh_bool_true);
    EXPECT_EQ(lh_version_get_major(&ver), 1);
    EXPECT_EQ(lh_version_get_minor(&ver), 2);
    EXPECT_EQ(lh_version_get_patch(&ver), 3);
}

TEST(version_parse, parses_zero_and_max)
{
    lh_version_t ver{};
    ASSERT_EQ(lh_version_parse("0.0.0", 5, &ver), lh_bool_true);
    EXPECT_EQ(lh_version_equals(&ver, &ver), lh_bool_true);

    ASSERT_EQ(lh_version_parse("65535.255.255", 13, &ver), lh_bool_true);
    EXPECT_EQ(lh_version_get_major(&ver), 65535);
    EXPECT_EQ(lh_version_get_minor(&ver), 255);
    EXPECT_EQ(lh_version_get_patch(&ver), 255);
}

TEST(version_parse, rejects_leading_zeros)
{
    lh_version_t ver = lh_version_initializer(9, 9, 9);
    EXPECT_EQ(lh_version_parse("01.2.3", 6, &ver), lh_bool_false);
    EXPECT_EQ(lh_version_get_major(&ver), 9);
}

TEST(version_parse, rejects_wrong_component_count)
{
    lh_version_t ver{};
    EXPECT_EQ(lh_version_parse("1.2", 3, &ver), lh_bool_false);
    EXPECT_EQ(lh_version_parse("1.2.3.4", 7, &ver), lh_bool_false);
}

TEST(version_parse, rejects_prerelease_and_build)
{
    lh_version_t ver{};
    EXPECT_EQ(lh_version_parse("1.2.3-rc.1", 10, &ver), lh_bool_false);
    EXPECT_EQ(lh_version_parse("1.2.3+build", 11, &ver), lh_bool_false);
}

TEST(version_parse, rejects_out_of_range)
{
    lh_version_t ver{};
    EXPECT_EQ(lh_version_parse("65536.0.0", 9, &ver), lh_bool_false);
    EXPECT_EQ(lh_version_parse("1.256.0", 7, &ver), lh_bool_false);
    EXPECT_EQ(lh_version_parse("1.0.256", 7, &ver), lh_bool_false);
}

TEST(version_format, writes_core_semver)
{
    const lh_version_t ver = lh_version_initializer(1, 2, 3);
    lh_char_t buf[LH_VERSION_TEXT_MAX];
    const lh_usize_t written = lh_version_format(&ver, buf, sizeof(buf));
    EXPECT_EQ(written, 5U);
    EXPECT_EQ(std::string(buf, written), "1.2.3");
}

TEST(version_format, writes_max_and_refuses_short_buffer)
{
    const lh_version_t ver = lh_version_initializer(65535, 255, 255);
    lh_char_t buf[LH_VERSION_TEXT_MAX];
    EXPECT_EQ(lh_version_format(&ver, buf, LH_VERSION_TEXT_MAX), 13U);
    EXPECT_EQ(std::string(buf, 13), "65535.255.255");
    EXPECT_EQ(lh_version_format(&ver, buf, 12), 0U);
}

TEST(version_format, roundtrip_via_parse)
{
    const lh_version_t src = lh_version_initializer(12, 0, 9);
    lh_char_t buf[LH_VERSION_TEXT_MAX];
    lh_version_t dst{};
    const lh_usize_t n = lh_version_format(&src, buf, sizeof(buf));
    ASSERT_NE(n, 0U);
    ASSERT_EQ(lh_version_parse(buf, n, &dst), lh_bool_true);
    EXPECT_EQ(lh_version_equals(&src, &dst), lh_bool_true);
}

} // namespace
