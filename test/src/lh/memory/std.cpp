#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/memory/std.h>
#include <lh/null.h>

#include <vector>

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

/*
 * lh_memory_std_compare scans LH_ALGORITHM_COMPARE_BLOCK (16) elements at a
 * time branchlessly before falling back to a precise scan of whichever block
 * turned out to differ — these cases exercise that block loop specifically
 * (equal/mismatching spans crossing and landing on block boundaries), which
 * the n=3/n=4 cases above never reach.
 */

TEST(memory_std_compare, equal_across_multiple_blocks)
{
    std::vector<lh_uchar_t> a(40);
    std::vector<lh_uchar_t> b(40);
    for (lh_usize_t i = 0; i < a.size(); ++i)
    {
        a[i] = b[i] = static_cast<lh_uchar_t>(i);
    }
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_std_compare, equal_at_exact_block_boundary)
{
    std::vector<lh_uchar_t> a(16, 0x7A);
    std::vector<lh_uchar_t> b(16, 0x7A);
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_std_compare, mismatch_at_first_byte_of_first_block)
{
    std::vector<lh_uchar_t> a(16, 0);
    std::vector<lh_uchar_t> b(16, 0);
    a[0] = 9;
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[0]));
}

TEST(memory_std_compare, mismatch_at_last_byte_of_first_block)
{
    std::vector<lh_uchar_t> a(16, 0);
    std::vector<lh_uchar_t> b(16, 0);
    a[15] = 9;
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[15]));
}

TEST(memory_std_compare, mismatch_at_first_byte_of_second_block)
{
    std::vector<lh_uchar_t> a(32, 0);
    std::vector<lh_uchar_t> b(32, 0);
    a[16] = 9;
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[16]));
}

TEST(memory_std_compare, mismatch_in_tail_after_full_blocks)
{
    std::vector<lh_uchar_t> a(20, 0);
    std::vector<lh_uchar_t> b(20, 0);
    a[17] = 9;
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[17]));
}

/*
 * lh_memory_std_compare dispatches, at runtime, to the widest tier this CPU
 * actually has: AVX2 (32 bytes/step) if available, else SSE2 (16 bytes/step) if
 * available, else the scalar LH_ALGORITHM_COMPARE_BLOCK path above — through the
 * same public entry point, so these cases exercise whichever tier this machine
 * resolves to for real. On the machine this suite was authored and last run on
 * (checked directly with __builtin_cpu_supports), that's SSE2: no AVX2, but SSE2 is
 * baseline on x86-64 and needs no runtime check at all. Either way the result must
 * be identical regardless of tier, which is the actual property under test. Sized
 * and positioned specifically to land mismatches in different lanes/chunks that the
 * <=40-byte cases above never reach: 96 bytes = three full 32-byte AVX2 chunks (and
 * six full 16-byte SSE2 chunks).
 */

TEST(memory_std_compare, equal_across_multiple_avx2_chunks)
{
    std::vector<lh_uchar_t> a(96);
    std::vector<lh_uchar_t> b(96);
    for (lh_usize_t i = 0; i < a.size(); ++i)
    {
        a[i] = b[i] = static_cast<lh_uchar_t>(i * 7);
    }
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_std_compare, mismatch_at_first_lane_of_first_avx2_chunk)
{
    std::vector<lh_uchar_t> a(96, 0);
    std::vector<lh_uchar_t> b(96, 0);
    a[0] = 9;
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[0]));
}

TEST(memory_std_compare, mismatch_at_last_lane_of_first_avx2_chunk)
{
    std::vector<lh_uchar_t> a(96, 0);
    std::vector<lh_uchar_t> b(96, 0);
    a[31] = 9;
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[31]));
}

TEST(memory_std_compare, mismatch_in_middle_avx2_chunk)
{
    std::vector<lh_uchar_t> a(96, 0);
    std::vector<lh_uchar_t> b(96, 0);
    a[50] = 9; // chunk 1 (bytes 32..63): first two chunks must compare equal first
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[50]));
}

TEST(memory_std_compare, mismatch_in_scalar_tail_after_avx2_chunks)
{
    std::vector<lh_uchar_t> a(100, 0); // 96 = 3 AVX2 chunks, 4-byte scalar tail
    std::vector<lh_uchar_t> b(100, 0);
    a[98] = 9;
    const lh_ptr d = lh_memory_std_compare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[98]));
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

/*
 * lh_memory_std_rcompare's own SSE2/AVX2 tiers (src/lh/memory/std.c) scan from the
 * end: each 16-/32-byte block covers the range ending at (and including) the
 * current position, and — since the scan direction is high-to-low addresses — a
 * mismatch is resolved via a *highest*-set-bit scan within the block, not a lowest-
 * set-bit one like lh_memory_std_compare. These cases mirror the
 * lh_memory_std_compare block/tail/AVX2-chunk cases above, adapted for that: the
 * "first-scanned" block/chunk here is the one nearest the *end* of the range, and
 * the scalar tail this falls back to (after consuming whole blocks from the end)
 * sits at the *start* of the range instead of the end.
 */

TEST(memory_std_rcompare, equal_across_multiple_blocks)
{
    std::vector<lh_uchar_t> a(40);
    std::vector<lh_uchar_t> b(40);
    for (lh_usize_t i = 0; i < a.size(); ++i)
    {
        a[i] = b[i] = static_cast<lh_uchar_t>(i);
    }
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_std_rcompare, equal_at_exact_block_boundary)
{
    std::vector<lh_uchar_t> a(16, 0x7A);
    std::vector<lh_uchar_t> b(16, 0x7A);
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_std_rcompare, mismatch_at_last_byte_of_block)
{
    std::vector<lh_uchar_t> a(16, 0);
    std::vector<lh_uchar_t> b(16, 0);
    a[15] = 9; // highest offset in the (only) block: found via the highest-set-bit scan
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[15]));
}

TEST(memory_std_rcompare, mismatch_at_first_byte_of_block)
{
    std::vector<lh_uchar_t> a(16, 0);
    std::vector<lh_uchar_t> b(16, 0);
    a[0] = 9; // lowest offset in the (only) block
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[0]));
}

TEST(memory_std_rcompare, mismatch_in_second_scanned_block)
{
    std::vector<lh_uchar_t> a(32, 0);
    std::vector<lh_uchar_t> b(32, 0);
    a[0] = 9; // block [16..31] (scanned first, nearest the end) must compare equal first
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[0]));
}

TEST(memory_std_rcompare, mismatch_in_scalar_tail_after_full_blocks)
{
    std::vector<lh_uchar_t> a(20, 0); // one full 16-byte block (scanned first) + 4-byte tail
    std::vector<lh_uchar_t> b(20, 0);
    a[2] = 9; // tail sits at the *start* of the range for a reverse scan
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[2]));
}

TEST(memory_std_rcompare, equal_across_multiple_avx2_chunks)
{
    std::vector<lh_uchar_t> a(96);
    std::vector<lh_uchar_t> b(96);
    for (lh_usize_t i = 0; i < a.size(); ++i)
    {
        a[i] = b[i] = static_cast<lh_uchar_t>(i * 7);
    }
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_std_rcompare, mismatch_at_last_lane_of_first_scanned_chunk)
{
    std::vector<lh_uchar_t> a(96, 0);
    std::vector<lh_uchar_t> b(96, 0);
    a[95] = 9; // very end of the range: highest lane of the chunk scanned first ([64..95])
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[95]));
}

TEST(memory_std_rcompare, mismatch_at_first_lane_of_first_scanned_chunk)
{
    std::vector<lh_uchar_t> a(96, 0);
    std::vector<lh_uchar_t> b(96, 0);
    a[64] = 9; // lowest offset of the chunk scanned first ([64..95])
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[64]));
}

TEST(memory_std_rcompare, mismatch_in_middle_avx2_chunk)
{
    std::vector<lh_uchar_t> a(96, 0);
    std::vector<lh_uchar_t> b(96, 0);
    a[50] = 9; // chunk [64..95] (scanned first) must compare equal first
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[50]));
}

TEST(memory_std_rcompare, mismatch_in_scalar_tail_after_avx2_chunks)
{
    std::vector<lh_uchar_t> a(100, 0); // 96 = 3 AVX2 chunks (scanned first), 4-byte tail
    std::vector<lh_uchar_t> b(100, 0);
    a[1] = 9; // tail sits at the *start* of the range for a reverse scan
    const lh_ptr d = lh_memory_std_rcompare(a.data(), b.data(), a.size());
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&a[1]));
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

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
