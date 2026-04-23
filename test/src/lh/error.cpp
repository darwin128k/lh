#include <gtest/gtest.h>

#include <lh/error.h>
#include <lh/error/initializer.h>
#include <lh/expect/death.h>

namespace {

TEST(error_set, roundtrip_code_and_desc) {
    lh_error_t err{};
    lh_error_set(&err, 42, "msg");
    EXPECT_EQ(lh_error_get_code(&err), 42);
    EXPECT_STREQ(lh_error_get_desc(&err), "msg");
}

TEST(error_pack, updates_only_code_when_desc_pointer_null) {
    lh_error_t err = lh_error_initializer(1, "keep");
    const lh_error_code_t new_code = 99;
    lh_error_pack(&err, &new_code, nullptr);
    EXPECT_EQ(lh_error_get_code(&err), 99);
    EXPECT_STREQ(lh_error_get_desc(&err), "keep");
}

TEST(error_pack, updates_only_desc_when_code_pointer_null) {
    lh_error_t err = lh_error_initializer(7, "old");
    lh_error_desc_t new_desc = "new";
    lh_error_pack(&err, nullptr, &new_desc);
    EXPECT_EQ(lh_error_get_code(&err), 7);
    EXPECT_STREQ(lh_error_get_desc(&err), "new");
}

TEST(error_pack, no_op_when_both_input_pointers_null) {
    lh_error_t err = lh_error_initializer(1, "unchanged");
    lh_error_pack(&err, nullptr, nullptr);
    EXPECT_EQ(lh_error_get_code(&err), 1);
    EXPECT_STREQ(lh_error_get_desc(&err), "unchanged");
}

TEST(error_unpack, skips_null_output_pointers) {
    const lh_error_t err = lh_error_initializer(3, "x");
    lh_error_code_t code = 0;
    lh_error_unpack(&err, &code, nullptr);
    EXPECT_EQ(code, 3);

    lh_error_desc_t desc = nullptr;
    lh_error_unpack(&err, nullptr, &desc);
    EXPECT_STREQ(desc, "x");
}

TEST(error_unpack, writes_both_outputs_when_non_null) {
    const lh_error_t err = lh_error_initializer(9, "both");
    lh_error_code_t code = 0;
    lh_error_desc_t desc = nullptr;
    lh_error_unpack(&err, &code, &desc);
    EXPECT_EQ(code, 9);
    EXPECT_STREQ(desc, "both");
}

TEST(error_set, null_desc_roundtrip) {
    lh_error_t err{};
    lh_error_set(&err, 3, nullptr);
    EXPECT_EQ(lh_error_get_code(&err), 3);
    EXPECT_EQ(lh_error_get_desc(&err), nullptr);
}

TEST(error_assign, copies_from_other) {
    const lh_error_t src = lh_error_initializer(11, "src");
    lh_error_t dst = lh_error_initializer(0, nullptr);
    lh_error_assign(&dst, &src);
    EXPECT_EQ(lh_error_get_code(&dst), 11);
    EXPECT_STREQ(lh_error_get_desc(&dst), "src");
}

TEST(error_clear, resets_to_ok_and_null_desc) {
    lh_error_t err = lh_error_initializer(5, "gone");
    lh_error_clear(&err);
    EXPECT_EQ(lh_error_get_code(&err), lh_error_code_ok);
    EXPECT_EQ(lh_error_get_desc(&err), nullptr);
}

TEST(error_init, matches_set) {
    lh_error_t a{};
    lh_error_t b{};
    lh_error_init(&a, 8, "eight");
    lh_error_set(&b, 8, "eight");
    EXPECT_EQ(lh_error_get_code(&a), lh_error_get_code(&b));
    EXPECT_STREQ(lh_error_get_desc(&a), lh_error_get_desc(&b));
}

TEST(error_init_by_other, matches_assign) {
    const lh_error_t src = lh_error_initializer(4, "four");
    lh_error_t a{};
    lh_error_t b{};
    lh_error_init_by_other(&a, &src);
    lh_error_assign(&b, &src);
    EXPECT_EQ(lh_error_get_code(&a), lh_error_get_code(&b));
    EXPECT_STREQ(lh_error_get_desc(&a), lh_error_get_desc(&b));
}

TEST(error_init_by_empty, clears_like_clear) {
    lh_error_t a = lh_error_initializer(2, "two");
    lh_error_t b = lh_error_initializer(2, "two");
    lh_error_init_by_empty(&a);
    lh_error_clear(&b);
    EXPECT_EQ(lh_error_get_code(&a), lh_error_get_code(&b));
    EXPECT_EQ(lh_error_get_desc(&a), lh_error_get_desc(&b));
}

TEST(error_get_code_and_clear, returns_previous_then_ok) {
    lh_error_t err = lh_error_initializer(100, "z");
    EXPECT_EQ(lh_error_get_code_and_clear(&err), 100);
    EXPECT_EQ(lh_error_get_code(&err), lh_error_code_ok);
    EXPECT_EQ(lh_error_get_desc(&err), nullptr);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(error_death, pack_null_self) {
    lh_error_code_t c = 1;
    lh_error_desc_t d = nullptr;
    LH_EXPECT_DEATH(lh_error_pack(nullptr, &c, &d));
}

TEST(error_death, unpack_null_self) {
    lh_error_code_t c = 0;
    LH_EXPECT_DEATH(lh_error_unpack(nullptr, &c, nullptr));
}

TEST(error_death, assign_null_self) {
    lh_error_t other = lh_error_empty_initializer();
    LH_EXPECT_DEATH(lh_error_assign(nullptr, &other));
}

TEST(error_death, assign_null_other) {
    lh_error_t dst{};
    LH_EXPECT_DEATH(lh_error_assign(&dst, nullptr));
}

TEST(error_death, init_null_self) {
    LH_EXPECT_DEATH(lh_error_init(nullptr, 1, nullptr));
}

TEST(error_death, init_by_other_null_self) {
    lh_error_t src = lh_error_empty_initializer();
    LH_EXPECT_DEATH(lh_error_init_by_other(nullptr, &src));
}

TEST(error_death, init_by_empty_null_self) {
    LH_EXPECT_DEATH(lh_error_init_by_empty(nullptr));
}

TEST(error_death, set_null_self) {
    LH_EXPECT_DEATH(lh_error_set(nullptr, 0, nullptr));
}

TEST(error_death, clear_null_self) {
    LH_EXPECT_DEATH(lh_error_clear(nullptr));
}

TEST(error_death, get_code_null_self) {
    LH_EXPECT_DEATH(lh_error_get_code(nullptr));
}

TEST(error_death, get_code_and_clear_null_self) {
    LH_EXPECT_DEATH(lh_error_get_code_and_clear(nullptr));
}

TEST(error_death, get_desc_null_self) {
    LH_EXPECT_DEATH(lh_error_get_desc(nullptr));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
