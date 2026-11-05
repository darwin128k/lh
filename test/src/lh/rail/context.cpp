#include <gtest/gtest.h>

#include <lh/rail/context.h>
#include <lh/rail/context/initializer.h>
#include <lh/expect/death.h>

namespace
{

struct state
{
    int opens = 0;
    int closes = 0;
    lh_ptr closed = nullptr;
};

lh_ptr
open_state(lh_ptr user)
{
    auto *s = static_cast<state *>(user);
    ++s->opens;
    return s;
}

void
close_state(lh_ptr user, lh_ptr local)
{
    auto *s = static_cast<state *>(user);
    ++s->closes;
    s->closed = local;
}

TEST(rail_context_initializer, holds_both_calls_and_the_pointer)
{
    state s;
    lh_rail_context_t context = lh_rail_context_initializer(open_state, close_state, &s);
    EXPECT_EQ(lh_rail_context_get_open_cb(&context), open_state);
    EXPECT_EQ(lh_rail_context_get_close_cb(&context), close_state);
    EXPECT_EQ(lh_rail_context_get_user(&context), &s);
}

TEST(rail_context_init, fills_and_assign_copies)
{
    state s;
    lh_rail_context_t context;
    lh_rail_context_init(&context, open_state, close_state, &s);
    lh_rail_context_t copy;
    lh_rail_context_assign(&copy, &context);
    EXPECT_EQ(lh_rail_context_get_open_cb(&copy), open_state);
    EXPECT_EQ(lh_rail_context_get_close_cb(&copy), close_state);
    EXPECT_EQ(lh_rail_context_get_user(&copy), &s);
    lh_rail_context_set_open_cb(&copy, lh_null);
    lh_rail_context_set_close_cb(&copy, lh_null);
    lh_rail_context_set_user(&copy, lh_null);
    EXPECT_EQ(lh_rail_context_get_open_cb(&copy), nullptr);
    EXPECT_EQ(lh_rail_context_get_close_cb(&copy), nullptr);
    EXPECT_EQ(lh_rail_context_get_user(&copy), nullptr);
}

TEST(rail_context_run, open_returns_local_and_close_receives_it)
{
    state s;
    lh_rail_context_t context = lh_rail_context_initializer(open_state, close_state, &s);
    lh_ptr local = lh_rail_context_run_open(&context);
    EXPECT_EQ(local, &s);
    lh_rail_context_run_close(&context, local);
    EXPECT_EQ(s.opens, 1);
    EXPECT_EQ(s.closes, 1);
    EXPECT_EQ(s.closed, &s);
}

TEST(rail_context_run, empty_context_opens_nothing)
{
    lh_rail_context_t context = lh_rail_context_initializer(lh_null, lh_null, lh_null);
    EXPECT_EQ(lh_rail_context_run_open(&context), nullptr);
    lh_rail_context_run_close(&context, lh_null);
}

TEST(rail_context_death, rejects_null_context)
{
    LH_EXPECT_DEATH(lh_rail_context_run_open(lh_null));
    LH_EXPECT_DEATH(lh_rail_context_set_user(lh_null, lh_null));
}

} // namespace
