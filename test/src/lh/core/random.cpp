#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/random.h>

TEST(random, init_stores_the_seed)
{
    lh_random_t self;

    lh_memory_std_set(&self, 0xFF, sizeof self);
    lh_random_init(&self, 42U);
    EXPECT_EQ(lh_random_get_state(&self), 42U);
}

TEST(random, init_replaces_the_zero_seed)
{
    lh_random_t self;

    lh_random_init(&self, 0U);
    EXPECT_EQ(lh_random_get_state(&self), LH_RANDOM_ZERO_SEED_STATE);
    EXPECT_NE(lh_random_next(&self), 0U);
}

/* xorshift32 (13, 17, 5) from the state 1, computed by hand. */
TEST(random, next_known_values)
{
    lh_random_t self;

    lh_random_init(&self, 1U);
    EXPECT_EQ(lh_random_next(&self), 270369U);
    EXPECT_EQ(lh_random_next(&self), 67634689U);
    EXPECT_EQ(lh_random_next(&self), 2647435461U);
    EXPECT_EQ(lh_random_get_state(&self), 2647435461U);
}

TEST(random, same_seed_same_sequence)
{
    lh_random_t a;
    lh_random_t b;

    lh_random_init(&a, 2026U);
    lh_random_init(&b, 2026U);
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(lh_random_next(&a), lh_random_next(&b));
    }
}

TEST(random, set_state_replays)
{
    lh_random_t self;

    lh_random_init(&self, 7U);
    const lh_u32_t saved = lh_random_get_state(&self);
    const lh_u32_t first = lh_random_next(&self);

    lh_random_set_state(&self, saved);
    EXPECT_EQ(lh_random_next(&self), first);
}

TEST(random, is_hit_bounds_and_one_step)
{
    lh_random_t self;
    lh_random_t twin;

    lh_random_init(&self, 7U);
    lh_random_init(&twin, 7U);
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(lh_random_is_hit(&self, 0U), lh_bool_false);
        EXPECT_EQ(lh_random_is_hit(&self, 100U), lh_bool_true);
        lh_random_next(&twin);
        lh_random_next(&twin);
    }
    EXPECT_EQ(lh_random_get_state(&self), lh_random_get_state(&twin));
}

TEST(random, is_hit_rate_is_close)
{
    lh_random_t self;
    int hits = 0;

    lh_random_init(&self, 1U);
    for (int i = 0; i < 10000; ++i)
    {
        hits += lh_random_is_hit(&self, 30U) ? 1 : 0;
    }
    EXPECT_GT(hits, 2800);
    EXPECT_LT(hits, 3200);
}
