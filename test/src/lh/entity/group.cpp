#include <gtest/gtest.h>

#include <lh/entity/group.h>
#include <lh/entity/option.h>
#include <lh/entity/screen.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/null.h>
#include <lh/self.h>

#include <cstdlib>

namespace
{

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
group_test_alloc(lh_self_ptr, lh_usize_t size)
{
    return std::malloc(static_cast<std::size_t>(size));
}

lh_void
group_test_dealloc(lh_self_ptr, lh_ptr ptr)
{
    std::free(ptr);
}

LH_COMPILER_EXTERN_C_END

class Group : public ::testing::Test
{
  protected:
    void
    SetUp() override
    {
        sized = lh_memory_allocator_initializer_with_context(group_test_alloc, group_test_dealloc,
                                                             lh_null, lh_null);
        screen = reinterpret_cast<lh_entity_screen_t *>(
            lh_entity_create_root(&lh_entity_screen_class, &sized));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(screen),
                              lh_math_vec2_make(200, 200));
    }

    void
    TearDown() override
    {
        lh_entity_delete(root());
    }

    lh_entity_t *
    root() const
    {
        return reinterpret_cast<lh_entity_t *>(screen);
    }

    lh_entity_group_t *
    make_group(lh_int_t mode)
    {
        lh_entity_group_t *group = reinterpret_cast<lh_entity_group_t *>(
            lh_entity_create(&lh_entity_group_class, root()));
        lh_entity_group_set_mode(group, mode);
        return group;
    }

    /* One of the three kinds, under @p parent. The kind is the only thing that
       changes, which is the point: a group asks its children a question that
       has one answer whichever kind it is. */
    lh_entity_option_t *
    make_option(lh_entity_t *parent, const lh_entity_class_t *kind)
    {
        return reinterpret_cast<lh_entity_option_t *>(lh_entity_create(kind, parent));
    }

    lh_memory_allocator_t sized;
    lh_entity_screen_t *screen = nullptr;
};

TEST_F(Group, a_new_group_is_single_mode)
{
    lh_entity_group_t *group = reinterpret_cast<lh_entity_group_t *>(
        lh_entity_create(&lh_entity_group_class, root()));
    EXPECT_EQ(lh_entity_group_get_mode(group), LH_ENTITY_GROUP_ONE);
}

TEST_F(Group, the_mode_folds_to_one_of_the_two)
{
    EXPECT_EQ(lh_entity_group_normalize_mode(LH_ENTITY_GROUP_ONE), LH_ENTITY_GROUP_ONE);
    EXPECT_EQ(lh_entity_group_normalize_mode(LH_ENTITY_GROUP_MANY), LH_ENTITY_GROUP_MANY);
    // Anything else is not "many", because only many says so.
    EXPECT_EQ(lh_entity_group_normalize_mode(7), LH_ENTITY_GROUP_ONE);
    EXPECT_EQ(lh_entity_group_normalize_mode(-1), LH_ENTITY_GROUP_ONE);

    lh_entity_group_t *group = make_group(LH_ENTITY_GROUP_MANY);
    lh_entity_group_set_mode(group, 99);
    EXPECT_EQ(lh_entity_group_get_mode(group), LH_ENTITY_GROUP_ONE);
}

TEST_F(Group, single_mode_turns_the_others_off)
{
    lh_entity_group_t *group = make_group(LH_ENTITY_GROUP_ONE);
    lh_entity_option_t *one = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);
    lh_entity_option_t *two = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_switch_class);

    lh_entity_option_set_on(one, lh_bool_true);
    lh_entity_option_set_on(two, lh_bool_true);

    EXPECT_FALSE(lh_entity_option_is_on(one)); // the second one took the slot
    EXPECT_TRUE(lh_entity_option_is_on(two));
}

TEST_F(Group, many_mode_leaves_everyone_alone)
{
    lh_entity_group_t *group = make_group(LH_ENTITY_GROUP_MANY);
    lh_entity_option_t *one = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);
    lh_entity_option_t *two = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_toggle_class);

    lh_entity_option_set_on(one, lh_bool_true);
    lh_entity_option_set_on(two, lh_bool_true);

    EXPECT_TRUE(lh_entity_option_is_on(one));
    EXPECT_TRUE(lh_entity_option_is_on(two));
}

TEST_F(Group, the_kind_does_not_matter_to_the_group)
{
    // All three under one single-mode group, turning on in turn. Whichever is
    // last is the only one left on, and the group never learns which is which:
    // that is the one cast in its walk instead of three.
    lh_entity_group_t *group = make_group(LH_ENTITY_GROUP_ONE);
    lh_entity_option_t *check = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);
    lh_entity_option_t *swtch = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_switch_class);
    lh_entity_option_t *toggl = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_toggle_class);

    lh_entity_option_set_on(check, lh_bool_true);
    EXPECT_TRUE(lh_entity_option_is_on(check));
    lh_entity_option_set_on(swtch, lh_bool_true);
    EXPECT_TRUE(lh_entity_option_is_on(swtch));
    lh_entity_option_set_on(toggl, lh_bool_true);

    EXPECT_FALSE(lh_entity_option_is_on(check));
    EXPECT_FALSE(lh_entity_option_is_on(swtch));
    EXPECT_TRUE(lh_entity_option_is_on(toggl));
}

TEST_F(Group, a_group_leaves_what_is_not_an_option_alone)
{
    lh_entity_group_t *group = make_group(LH_ENTITY_GROUP_ONE);
    lh_entity_option_t *one = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);
    lh_entity_2d_t *plain = reinterpret_cast<lh_entity_2d_t *>(
        lh_entity_create(&lh_entity_2d_class, lh_ptr_rcast(lh_entity_t, group)));

    lh_entity_option_set_on(one, lh_bool_true);
    // The walk passes over it without a complaint, and without a cast failing
    // anywhere the caller can see.
    EXPECT_TRUE(lh_entity_option_is_on(one));
    EXPECT_EQ(lh_ptr_rcast(lh_entity_t, plain), lh_ptr_rcast(lh_entity_t, plain));
}

TEST_F(Group, an_option_outside_any_group_still_works)
{
    lh_entity_option_t *alone =
        make_option(root(), &lh_entity_check_class);
    lh_entity_option_set_on(alone, lh_bool_true);
    EXPECT_TRUE(lh_entity_option_is_on(alone));
    lh_entity_option_set_on(alone, lh_bool_false);
    EXPECT_FALSE(lh_entity_option_is_on(alone));
}

TEST_F(Group, an_option_under_a_parent_that_is_not_a_group_still_works)
{
    lh_entity_2d_t *plain = reinterpret_cast<lh_entity_2d_t *>(
        lh_entity_create(&lh_entity_2d_class, root()));
    lh_entity_option_t *one =
        make_option(lh_ptr_rcast(lh_entity_t, plain), &lh_entity_switch_class);
    lh_entity_option_t *two =
        make_option(lh_ptr_rcast(lh_entity_t, plain), &lh_entity_check_class);

    lh_entity_option_set_on(one, lh_bool_true);
    lh_entity_option_set_on(two, lh_bool_true);
    EXPECT_TRUE(lh_entity_option_is_on(one)); // no group above: no clearing
    EXPECT_TRUE(lh_entity_option_is_on(two));
}

TEST_F(Group, turning_one_off_never_takes_the_others_with_it)
{
    // The trap a group falls into when it goes through set_on: each member it
    // turns off would ask the group in turn, and the first one would empty the
    // whole group.
    lh_entity_group_t *group = make_group(LH_ENTITY_GROUP_ONE);
    lh_entity_option_t *one = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);
    lh_entity_option_t *two = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);
    lh_entity_option_t *three = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);

    lh_entity_option_set_on(one, lh_bool_true);
    lh_entity_option_set_on(two, lh_bool_true); // one is off, two is on
    lh_entity_option_set_on(three, lh_bool_true); // two is off, three is on

    lh_entity_option_set_on(three, lh_bool_false);
    EXPECT_FALSE(lh_entity_option_is_on(three));
    // Nothing else was on, and nothing got turned on by the clearing.
    EXPECT_FALSE(lh_entity_option_is_on(one));
    EXPECT_FALSE(lh_entity_option_is_on(two));
}

TEST_F(Group, the_raw_setter_does_not_ask_the_group)
{
    lh_entity_group_t *group = make_group(LH_ENTITY_GROUP_ONE);
    lh_entity_option_t *one = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);
    lh_entity_option_t *two = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);

    lh_entity_option_set_on_raw(one, lh_bool_true);
    lh_entity_option_set_on_raw(two, lh_bool_true);
    EXPECT_TRUE(lh_entity_option_is_on(one)); // two of them on at once
    EXPECT_TRUE(lh_entity_option_is_on(two));
}

TEST_F(Group, a_group_can_be_told_which_member_won)
{
    // The operation is the parent's, so a caller that is not an option can use
    // it: choose the member, and the group clears the rest.
    lh_entity_group_t *group = make_group(LH_ENTITY_GROUP_ONE);
    lh_entity_option_t *one = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);
    lh_entity_option_t *two = make_option(lh_ptr_rcast(lh_entity_t, group), &lh_entity_check_class);

    lh_entity_option_set_on_raw(one, lh_bool_true);
    lh_entity_option_set_on_raw(two, lh_bool_true);
    lh_entity_group_select(group, lh_ptr_rcast(lh_entity_t, one));

    EXPECT_TRUE(lh_entity_option_is_on(one));  // named, so kept
    EXPECT_FALSE(lh_entity_option_is_on(two)); // the rest cleared
}

TEST_F(Group, the_three_kinds_answer_to_one_class)
{
    // What the single cast in the walk rests on: each of the three is an
    // option by the base class, and none of them needs the other two named.
    const lh_entity_class_t *kinds[3] = {&lh_entity_check_class, &lh_entity_switch_class,
                                         &lh_entity_toggle_class};
    for (int i = 0; i < 3; ++i)
    {
        lh_entity_option_t *option = make_option(root(), kinds[i]);
        EXPECT_NE(lh_entity_cast(lh_ptr_rcast(lh_entity_t, option),
                                 lh_addr_of(lh_entity_option_class)),
                  nullptr);
    }
    // And a plain box under the same parent is not.
    lh_entity_2d_t *plain = reinterpret_cast<lh_entity_2d_t *>(
        lh_entity_create(&lh_entity_2d_class, root()));
    EXPECT_EQ(lh_entity_cast(lh_ptr_rcast(lh_entity_t, plain), lh_addr_of(lh_entity_option_class)),
              nullptr);
}

} // namespace
