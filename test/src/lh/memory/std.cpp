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

/*
 * lh_memory_std_copy has a REP MOVSB path on both MSVC (__movsb, unconditional) and
 * GCC/Clang (inline asm, only above LH_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD — see
 * src/lh/memory/std.c) — the n=4 case above never reaches either. 1024 bytes crosses
 * that GCC-side threshold (measured at 512) with room either side; every byte gets a
 * distinct value so any off-by-one in the copied range or its returned end pointer
 * would show up as a mismatch, not just a coincidentally-right total.
 */
TEST(memory_std_copy, copies_bytes_above_rep_movsb_threshold)
{
    const lh_usize_t n = 1024;
    std::vector<lh_uchar_t> src(n);
    std::vector<lh_uchar_t> dst(n, 0);
    for (lh_usize_t i = 0; i < n; ++i)
    {
        src[i] = static_cast<lh_uchar_t>(i);
    }

    lh_ptr end = lh_memory_std_copy(dst.data(), src.data(), n);

    EXPECT_EQ(end, static_cast<lh_ptr>(dst.data() + n));
    EXPECT_EQ(dst, src);
}

/*
 * lh_memory_std_copy's SIMD tier (src/lh/memory/std.c, above
 * LH_MEMORY_STD_SIMD_COPY_THRESHOLD) processes the bulk of a copy in fixed-width
 * blocks — 128 bytes at a time for AVX2's unrolled loop, then 32, then 16 for SSE2 —
 * and hands off whatever is left over (0 to one block width minus one) to
 * lh_algorithm_copy for the tail. A regression here once had that handoff read an
 * uninitialized pointer instead of the real one (a local named the same as a macro-
 * internal variable, shadowing it at its own initializer — see git history for
 * src/lh/memory/std.c), silently leaving the tail bytes untouched for any length that
 * wasn't an exact multiple of every block width in play; it passed every other test in
 * this file because none of them used a length large enough to reach the SIMD tier
 * *and* leave a non-empty tail. This sweeps every tail remainder from 0 to past one
 * full 128-byte block so no particular leftover count can go unexercised again, and
 * checks both that every requested byte is correct and that nothing past the
 * requested range was touched.
 */
TEST(memory_std_copy, exact_bytes_across_every_tail_remainder)
{
    const lh_usize_t base = 512; // LH_MEMORY_STD_SIMD_COPY_THRESHOLD
    const lh_usize_t guard = 8;

    for (lh_usize_t extra = 0; extra <= 200; ++extra)
    {
        const lh_usize_t n = base + extra;
        std::vector<lh_uchar_t> src(n);
        std::vector<lh_uchar_t> dst(n + guard, 0xEE);
        for (lh_usize_t i = 0; i < n; ++i)
        {
            src[i] = static_cast<lh_uchar_t>((i * 37U + 11U) & 0xFFU);
        }

        lh_ptr end = lh_memory_std_copy(dst.data(), src.data(), n);

        ASSERT_EQ(end, static_cast<lh_ptr>(dst.data() + n)) << "n=" << n;
        for (lh_usize_t i = 0; i < n; ++i)
        {
            ASSERT_EQ(dst[i], src[i]) << "n=" << n << " i=" << i;
        }
        for (lh_usize_t i = n; i < dst.size(); ++i)
        {
            ASSERT_EQ(dst[i], 0xEE) << "n=" << n << " i=" << i << " (past requested range)";
        }
    }
}

/*
 * Same tail-remainder concern as the sweep above, but with src/dst each offset by a
 * few bytes from their own allocation's start — the SIMD tiers' loads/stores are all
 * unaligned (_mm_loadu_si128/_mm256_loadu_si256 etc.) so this should make no
 * difference, but it is what actually distinguishes "the tail math is right" from
 * "the tail math happens to be right when both pointers are naturally aligned".
 */
TEST(memory_std_copy, exact_bytes_with_misaligned_src_and_dst)
{
    const lh_usize_t n = 700; // past LH_MEMORY_STD_SIMD_COPY_THRESHOLD, not block-aligned
    const lh_usize_t offsets[] = {0, 1, 3, 7, 15, 17, 31};

    for (lh_usize_t dst_off : offsets)
    {
        for (lh_usize_t src_off : offsets)
        {
            std::vector<lh_uchar_t> src_buf(n + 32);
            std::vector<lh_uchar_t> dst_buf(n + 32, 0xEE);
            lh_uchar_t *src = src_buf.data() + src_off;
            lh_uchar_t *dst = dst_buf.data() + dst_off;
            for (lh_usize_t i = 0; i < n; ++i)
            {
                src[i] = static_cast<lh_uchar_t>((i * 37U + 11U) & 0xFFU);
            }

            lh_memory_std_copy(dst, src, n);

            for (lh_usize_t i = 0; i < n; ++i)
            {
                ASSERT_EQ(dst[i], src[i]) << "dst_off=" << dst_off << " src_off=" << src_off
                                           << " i=" << i;
            }
        }
    }
}

/*
 * Above LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD (2MB), lh_memory_std_copy switches to
 * a non-temporal ("streaming store") AVX2 tier with its own alignment-prologue (copies
 * a short unaligned head with regular stores first, since MOVNTDQ requires a 32-byte-
 * aligned destination) and tail handling — dead code below that threshold, so it needs
 * its own coverage. Sweeps destination/source misalignments across that 32-byte
 * boundary at a few sizes straddling the threshold itself.
 */
TEST(memory_std_copy, streaming_tier_bytes_and_alignment_head)
{
    const lh_usize_t stream_threshold = 2U * 1024U * 1024U; // LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD
    const lh_usize_t sizes[] = {stream_threshold - 1, stream_threshold, stream_threshold + 137};
    const lh_usize_t offsets[] = {0, 1, 17, 31};

    for (lh_usize_t size : sizes)
    {
        for (lh_usize_t dst_off : offsets)
        {
            for (lh_usize_t src_off : offsets)
            {
                std::vector<lh_uchar_t> src_buf(size + 64);
                std::vector<lh_uchar_t> dst_buf(size + 64, 0xEE);
                lh_uchar_t *src = src_buf.data() + src_off;
                lh_uchar_t *dst = dst_buf.data() + dst_off;
                for (lh_usize_t i = 0; i < size; ++i)
                {
                    src[i] = static_cast<lh_uchar_t>((i * 2654435761U) & 0xFFU);
                }

                lh_memory_std_copy(dst, src, size);

                for (lh_usize_t i = 0; i < size; ++i)
                {
                    ASSERT_EQ(dst[i], src[i]) << "size=" << size << " dst_off=" << dst_off
                                               << " src_off=" << src_off << " i=" << i;
                }
            }
        }
    }
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

/*
 * Same tail-remainder concern as memory_std_copy's sweep above, mirrored for
 * lh_memory_std_rcopy's own SIMD tier (LH_MEMORY_STD_SIMD_RCOPY_THRESHOLD, walking
 * from the end of the range down to its start in 32-/16-byte blocks, handing off the
 * remaining head to lh_algorithm_rcopy). Non-overlapping buffers here — the
 * overlap-specific semantics are already covered by the test above — just to isolate
 * "is every byte in range copied correctly" from "is the overlap direction correct".
 */
TEST(memory_std_rcopy, exact_bytes_across_every_tail_remainder)
{
    const lh_usize_t guard = 8;

    for (lh_usize_t n = 0; n <= 200; ++n)
    {
        // Guarded the same as dst below, not just sized n: an empty (n=0) std::vector's
        // data() is permitted to be null, which lh_memory_std_rcopy's own null-pointer
        // assertion (checked regardless of n — see the memory_std_rcopy_death tests
        // below) would reject even though a zero-byte rcopy is otherwise a no-op.
        std::vector<lh_uchar_t> src(n + guard);
        std::vector<lh_uchar_t> dst(n + guard, 0xEE);
        for (lh_usize_t i = 0; i < n; ++i)
        {
            src[i] = static_cast<lh_uchar_t>((i * 37U + 11U) & 0xFFU);
        }

        lh_memory_std_rcopy(dst.data(), src.data(), n);

        for (lh_usize_t i = 0; i < n; ++i)
        {
            ASSERT_EQ(dst[i], src[i]) << "n=" << n << " i=" << i;
        }
        for (lh_usize_t i = n; i < dst.size(); ++i)
        {
            ASSERT_EQ(dst[i], 0xEE) << "n=" << n << " i=" << i << " (past requested range)";
        }
    }
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

/*
 * Same tail-remainder concern as memory_std_copy's sweep above, mirrored for
 * lh_memory_std_set's own SIMD tier (LH_MEMORY_STD_SIMD_SET_THRESHOLD, filling in
 * 128-/32-/16-byte broadcast-store blocks and handing off the remainder to
 * lh_algorithm_set).
 */
TEST(memory_std_set, fills_every_byte_across_every_tail_remainder)
{
    const lh_usize_t guard = 8;

    for (lh_usize_t n = 0; n <= 200; ++n)
    {
        std::vector<lh_uchar_t> dst(n + guard, 0xEE);

        lh_ptr end = lh_memory_std_set(dst.data(), 0x77, n);

        ASSERT_EQ(end, static_cast<lh_ptr>(dst.data() + n)) << "n=" << n;
        for (lh_usize_t i = 0; i < n; ++i)
        {
            ASSERT_EQ(dst[i], 0x77) << "n=" << n << " i=" << i;
        }
        for (lh_usize_t i = n; i < dst.size(); ++i)
        {
            ASSERT_EQ(dst[i], 0xEE) << "n=" << n << " i=" << i << " (past requested range)";
        }
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
