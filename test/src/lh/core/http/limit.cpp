#include <gtest/gtest.h>

#include <lh/core/http/limit/bucket.h>
#include <lh/core/http/limit/peer.h>

#include <cstring>

TEST(lh_http_limit_bucket, refills_at_the_rate_up_to_the_burst)
{
    EXPECT_DOUBLE_EQ(lh_http_limit_bucket_get_refilled(0.0, 500, 1000.0, 1e9), 500.0);
    EXPECT_DOUBLE_EQ(lh_http_limit_bucket_get_refilled(-100.0, 100, 1000.0, 1e9), 0.0);
    EXPECT_DOUBLE_EQ(lh_http_limit_bucket_get_refilled(90.0, 1000, 1000.0, 100.0), 100.0);
    EXPECT_DOUBLE_EQ(lh_http_limit_bucket_get_refilled(5.0, -10, 1000.0, 100.0), 5.0);
}

TEST(lh_http_limit_peer, an_address_is_held_to_its_limits)
{
    lh_usize_t slot = 99U;
    lh_usize_t again = 99U;

    lh_http_limit_peer_t peers[4];
    std::memset(peers, 0, sizeof(peers));
    ASSERT_TRUE(lh_http_limit_peer_try_acquire(peers, 4U, 7U, 0, 2U, 100U, &slot));
    ASSERT_TRUE(lh_http_limit_peer_try_acquire(peers, 4U, 7U, 1, 2U, 100U, &again));
    EXPECT_EQ(slot, again);
    /* Two open: the third waits for one to close. */
    EXPECT_FALSE(lh_http_limit_peer_try_acquire(peers, 4U, 7U, 2, 2U, 100U, &again));
    lh_http_limit_peer_release(&peers[slot]);
    EXPECT_TRUE(lh_http_limit_peer_try_acquire(peers, 4U, 7U, 3, 2U, 100U, &again));
}

TEST(lh_http_limit_peer, the_rate_window_starts_over)
{
    lh_usize_t slot = 0U;

    lh_http_limit_peer_t peers[1];
    std::memset(peers, 0, sizeof(peers));
    ASSERT_TRUE(lh_http_limit_peer_try_acquire(peers, 1U, 7U, 0, 10U, 2U, &slot));
    lh_http_limit_peer_release(&peers[slot]);
    ASSERT_TRUE(lh_http_limit_peer_try_acquire(peers, 1U, 7U, 10, 10U, 2U, &slot));
    lh_http_limit_peer_release(&peers[slot]);
    EXPECT_FALSE(lh_http_limit_peer_try_acquire(peers, 1U, 7U, 20, 10U, 2U, &slot));
    EXPECT_TRUE(lh_http_limit_peer_try_acquire(peers, 1U, 7U, LH_HTTP_LIMIT_PEER_WINDOW_MS, 10U, 2U,
                                               &slot));
}

TEST(lh_http_limit_peer, a_full_table_reuses_the_oldest_idle_peer)
{
    lh_usize_t first = 0U;
    lh_usize_t second = 0U;
    lh_usize_t third = 0U;

    lh_http_limit_peer_t peers[2];
    std::memset(peers, 0, sizeof(peers));
    ASSERT_TRUE(lh_http_limit_peer_try_acquire(peers, 2U, 1U, 0, 5U, 5U, &first));
    ASSERT_TRUE(lh_http_limit_peer_try_acquire(peers, 2U, 2U, 10, 5U, 5U, &second));
    /* Both busy: a third address finds no slot. */
    EXPECT_FALSE(lh_http_limit_peer_try_acquire(peers, 2U, 3U, 20, 5U, 5U, &third));
    lh_http_limit_peer_release(&peers[first]);
    lh_http_limit_peer_release(&peers[second]);
    ASSERT_TRUE(lh_http_limit_peer_try_acquire(peers, 2U, 3U, 30, 5U, 5U, &third));
    EXPECT_EQ(third, first);
    EXPECT_EQ(lh_http_limit_peer_get_key(&peers[third]), 3U);
}
