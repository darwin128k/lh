#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/memory/std.h>
#include <lh/null.h>

namespace
{

TEST(memory_std_copy, copies_bytes_and_returns_end)
{
    lh_uchar_t dst[8] = {};
    const lh_uchar_t src[] = {1, 2, 3, 4};
    lh_ptr end = lh_memory_std_copy(dst, src, 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 4));
    EXPECT_EQ(dst[0], 1);
    EXPECT_EQ(dst[1], 2);
    EXPECT_EQ(dst[2], 3);
    EXPECT_EQ(dst[3], 4);
}

TEST(memory_std_copy, zero_bytes)
{
    lh_uchar_t dst[1] = {42};
    const lh_uchar_t src[1] = {99};
    lh_ptr end = lh_memory_std_copy(dst, src, 0);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst));
    EXPECT_EQ(dst[0], 42);
}

TEST(memory_std_copy_rev, reverses_order_in_destination)
{
    lh_uchar_t src[] = {1, 2, 3, 4};
    lh_uchar_t dst[4] = {};
    lh_ptr end = lh_memory_std_copy_rev(dst, src, 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 4));
    EXPECT_EQ(dst[0], 4);
    EXPECT_EQ(dst[1], 3);
    EXPECT_EQ(dst[2], 2);
    EXPECT_EQ(dst[3], 1);
}

TEST(memory_std_rcopy, overlapping_backward_copy)
{
    lh_uchar_t v[] = {1, 2, 3, 4, 5};
    lh_ptr r = lh_memory_std_rcopy(v + 1, v, 4);
    EXPECT_EQ(r, static_cast<lh_ptr>(v + 1));
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 1);
    EXPECT_EQ(v[2], 2);
    EXPECT_EQ(v[3], 3);
    EXPECT_EQ(v[4], 4);
}

TEST(memory_std_move, forward_overlap_matches_memmove_example)
{
    lh_uchar_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8};
    lh_ptr end = lh_memory_std_move(buf + 2, buf, 6);
    EXPECT_EQ(end, static_cast<lh_ptr>(buf + 8));
    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[1], 2);
    EXPECT_EQ(buf[2], 1);
    EXPECT_EQ(buf[3], 2);
    EXPECT_EQ(buf[4], 3);
    EXPECT_EQ(buf[5], 4);
    EXPECT_EQ(buf[6], 5);
    EXPECT_EQ(buf[7], 6);
}

TEST(memory_std_move, non_overlapping_acts_like_copy)
{
    lh_uchar_t dst[4] = {};
    const lh_uchar_t src[] = {9, 8, 7, 6};
    lh_ptr end = lh_memory_std_move(dst, src, 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 4));
    EXPECT_EQ(dst[0], 9);
    EXPECT_EQ(dst[3], 6);
}

TEST(memory_std_move, backward_overlap_uses_reverse_path)
{
    lh_uchar_t buf[] = {1, 2, 3, 4, 5};
    lh_ptr end = lh_memory_std_move(buf + 1, buf, 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(buf + 5));
    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[1], 1);
    EXPECT_EQ(buf[2], 2);
    EXPECT_EQ(buf[3], 3);
    EXPECT_EQ(buf[4], 4);
}

TEST(memory_std_set, fills_range)
{
    lh_uchar_t block[16];
    lh_ptr end = lh_memory_std_set(block, 0xAB, 16);
    EXPECT_EQ(end, static_cast<lh_ptr>(block + 16));
    for (lh_usize_t i = 0; i < 16; ++i)
    {
        EXPECT_EQ(block[i], 0xAB);
    }
}

TEST(memory_std_compare, equal_returns_null)
{
    const lh_uchar_t a[] = {1, 2, 3};
    const lh_uchar_t b[] = {1, 2, 3};
    const lh_ptr d = lh_memory_std_compare(a, b, 3);
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_std_compare, returns_first_mismatch_in_lhs)
{
    const lh_uchar_t a[] = {1, 2, 9, 4};
    const lh_uchar_t b[] = {1, 2, 3, 4};
    const lh_ptr d = lh_memory_std_compare(a, b, 4);
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[2]));
    EXPECT_EQ(*static_cast<const lh_uchar_t *>(d), 9);
}

TEST(memory_std_rcompare, equal_returns_null)
{
    const lh_uchar_t a[] = {1, 2, 3};
    const lh_uchar_t b[] = {1, 2, 3};
    const lh_ptr d = lh_memory_std_rcompare(a, b, 3);
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_std_rcompare, returns_tail_mismatch_first)
{
    const lh_uchar_t a[] = {1, 2, 3, 9};
    const lh_uchar_t b[] = {1, 2, 3, 4};
    const lh_ptr d = lh_memory_std_rcompare(a, b, 4);
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[3]));
    EXPECT_EQ(*static_cast<const lh_uchar_t *>(d), 9);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_std_copy_death, null_dst)
{
    lh_uchar_t src[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_copy(lh_null, src, 1));
}

TEST(memory_std_copy_death, null_src)
{
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_std_copy(dst, lh_null, 1));
}

TEST(memory_std_copy_death, both_null)
{
    LH_EXPECT_DEATH(lh_memory_std_copy(lh_null, lh_null, 1));
}

TEST(memory_std_copy_death, null_dst_zero_bytes)
{
    lh_uchar_t src[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_copy(lh_null, src, 0));
}

TEST(memory_std_copy_death, null_src_zero_bytes)
{
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_std_copy(dst, lh_null, 0));
}

TEST(memory_std_copy_death, both_null_zero_bytes)
{
    LH_EXPECT_DEATH(lh_memory_std_copy(lh_null, lh_null, 0));
}

TEST(memory_std_copy_rev_death, null_dst)
{
    lh_uchar_t src[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_copy_rev(lh_null, src, 1));
}

TEST(memory_std_copy_rev_death, null_src)
{
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_std_copy_rev(dst, lh_null, 1));
}

TEST(memory_std_copy_rev_death, both_null)
{
    LH_EXPECT_DEATH(lh_memory_std_copy_rev(lh_null, lh_null, 1));
}

TEST(memory_std_copy_rev_death, null_dst_zero_bytes)
{
    lh_uchar_t src[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_copy_rev(lh_null, src, 0));
}

TEST(memory_std_copy_rev_death, null_src_zero_bytes)
{
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_std_copy_rev(dst, lh_null, 0));
}

TEST(memory_std_copy_rev_death, both_null_zero_bytes)
{
    LH_EXPECT_DEATH(lh_memory_std_copy_rev(lh_null, lh_null, 0));
}

TEST(memory_std_rcopy_death, null_dst)
{
    lh_uchar_t src[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_rcopy(lh_null, src, 1));
}

TEST(memory_std_rcopy_death, null_src)
{
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_std_rcopy(dst, lh_null, 1));
}

TEST(memory_std_rcopy_death, both_null)
{
    LH_EXPECT_DEATH(lh_memory_std_rcopy(lh_null, lh_null, 1));
}

TEST(memory_std_rcopy_death, null_dst_zero_bytes)
{
    lh_uchar_t src[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_rcopy(lh_null, src, 0));
}

TEST(memory_std_rcopy_death, null_src_zero_bytes)
{
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_std_rcopy(dst, lh_null, 0));
}

TEST(memory_std_rcopy_death, both_null_zero_bytes)
{
    LH_EXPECT_DEATH(lh_memory_std_rcopy(lh_null, lh_null, 0));
}

TEST(memory_std_move_death, null_dst)
{
    lh_uchar_t src[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_move(lh_null, src, 1));
}

TEST(memory_std_move_death, null_src)
{
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_std_move(dst, lh_null, 1));
}

TEST(memory_std_move_death, both_null)
{
    LH_EXPECT_DEATH(lh_memory_std_move(lh_null, lh_null, 1));
}

TEST(memory_std_move_death, null_dst_zero_bytes)
{
    lh_uchar_t src[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_move(lh_null, src, 0));
}

TEST(memory_std_move_death, null_src_zero_bytes)
{
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_std_move(dst, lh_null, 0));
}

TEST(memory_std_move_death, both_null_zero_bytes)
{
    LH_EXPECT_DEATH(lh_memory_std_move(lh_null, lh_null, 0));
}

TEST(memory_std_set_death, null_dst)
{
    LH_EXPECT_DEATH(lh_memory_std_set(lh_null, 0, 1));
}

TEST(memory_std_set_death, null_dst_zero_bytes)
{
    LH_EXPECT_DEATH(lh_memory_std_set(lh_null, 0, 0));
}

TEST(memory_std_compare_death, null_lhs)
{
    lh_uchar_t b[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_compare(lh_null, b, 1));
}

TEST(memory_std_compare_death, null_rhs)
{
    lh_uchar_t a[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_compare(a, lh_null, 1));
}

TEST(memory_std_compare_death, both_null)
{
    LH_EXPECT_DEATH(lh_memory_std_compare(lh_null, lh_null, 1));
}

TEST(memory_std_compare_death, null_lhs_zero_bytes)
{
    lh_uchar_t b[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_compare(lh_null, b, 0));
}

TEST(memory_std_compare_death, null_rhs_zero_bytes)
{
    lh_uchar_t a[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_compare(a, lh_null, 0));
}

TEST(memory_std_compare_death, both_null_zero_bytes)
{
    LH_EXPECT_DEATH(lh_memory_std_compare(lh_null, lh_null, 0));
}

TEST(memory_std_rcompare_death, null_lhs)
{
    lh_uchar_t b[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_rcompare(lh_null, b, 1));
}

TEST(memory_std_rcompare_death, null_rhs)
{
    lh_uchar_t a[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_rcompare(a, lh_null, 1));
}

TEST(memory_std_rcompare_death, both_null)
{
    LH_EXPECT_DEATH(lh_memory_std_rcompare(lh_null, lh_null, 1));
}

TEST(memory_std_rcompare_death, null_lhs_zero_bytes)
{
    lh_uchar_t b[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_rcompare(lh_null, b, 0));
}

TEST(memory_std_rcompare_death, null_rhs_zero_bytes)
{
    lh_uchar_t a[] = {1};
    LH_EXPECT_DEATH(lh_memory_std_rcompare(a, lh_null, 0));
}

TEST(memory_std_rcompare_death, both_null_zero_bytes)
{
    LH_EXPECT_DEATH(lh_memory_std_rcompare(lh_null, lh_null, 0));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
