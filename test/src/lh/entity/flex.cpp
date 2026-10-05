#include <gtest/gtest.h>

#include <lh/entity/2d.h>
#include <lh/entity/flex.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/null.h>
#include <lh/self.h>

#include <cstdlib>

namespace
{

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
flex_test_alloc(lh_self_ptr, lh_usize_t size)
{
    return std::malloc(static_cast<std::size_t>(size));
}

lh_void
flex_test_dealloc(lh_self_ptr, lh_ptr ptr)
{
    std::free(ptr);
}

LH_COMPILER_EXTERN_C_END

/* The measured content of a container is kept on the container for the rest of
   the pass it was measured in, so every test here is two questions: is the
   number right, and is it right again after something it was measured from
   has moved. */
class Flex : public ::testing::Test
{
  protected:
    void
    SetUp() override
    {
        sized = lh_memory_allocator_initializer_with_context(
            flex_test_alloc, flex_test_dealloc, lh_null, lh_null);
        root = lh_entity_create_root(&lh_entity_base_class, &sized);
    }

    void
    TearDown() override
    {
        lh_entity_delete(root);
    }

    lh_entity_flex_t *
    make_flex(lh_entity_t *parent, lh_float_t w, lh_float_t h)
    {
        lh_entity_flex_t *flex = reinterpret_cast<lh_entity_flex_t *>(
            lh_entity_create(&lh_entity_flex_class, parent));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(flex), lh_math_vec2_make(w, h));
        return flex;
    }

    lh_entity_2d_t *
    make_box(lh_entity_t *parent, lh_float_t w, lh_float_t h)
    {
        lh_entity_2d_t *box =
            reinterpret_cast<lh_entity_2d_t *>(lh_entity_create(&lh_entity_2d_class, parent));
        lh_entity_2d_set_size(box, lh_math_vec2_make(w, h));
        return box;
    }

    lh_entity_t *
    of(lh_entity_flex_t *flex)
    {
        return reinterpret_cast<lh_entity_t *>(flex);
    }

    void
    resize(lh_entity_2d_t *box, lh_float_t w, lh_float_t h)
    {
        lh_entity_2d_set_size(box, lh_math_vec2_make(w, h));
    }

    void
    expect_size(lh_entity_2d_t *box, lh_float_t w, lh_float_t h)
    {
        const lh_math_vec2_t size = lh_entity_2d_get_size(box);
        EXPECT_FLOAT_EQ(size.x, w);
        EXPECT_FLOAT_EQ(size.y, h);
    }

    lh_memory_sized_allocator_t sized;
    lh_entity_t *root;
};

TEST_F(Flex, content_sums_the_leaves_with_the_gap_and_its_own_padding)
{
    lh_entity_flex_t *outer = make_flex(root, 0.0f, 0.0f);
    lh_entity_flex_set_pad(outer, 5);
    lh_entity_flex_set_gap(outer, 10);
    make_box(of(outer), 30.0f, 20.0f);
    make_box(of(outer), 50.0f, 40.0f);

    EXPECT_EQ(lh_entity_flex_content(outer, lh_bool_true), 100);
    EXPECT_EQ(lh_entity_flex_content(outer, lh_bool_false), 50);
}

TEST_F(Flex, content_reads_a_hugging_child_through_its_own_children)
{
    lh_entity_flex_t *outer = make_flex(root, 0.0f, 0.0f);
    lh_entity_flex_t *inner = make_flex(of(outer), 0.0f, 0.0f);
    make_box(of(inner), 20.0f, 10.0f);
    make_box(of(inner), 30.0f, 15.0f);

    EXPECT_EQ(lh_entity_flex_content(outer, lh_bool_true), 50);
    EXPECT_EQ(lh_entity_flex_content(outer, lh_bool_false), 15);
}

TEST_F(Flex, a_chain_of_hugging_containers_answers_the_leaf_it_ends_on)
{
    lh_entity_t *chain[4];
    for (int i = 0; i < 4; ++i)
    {
        chain[i] = of(make_flex(i == 0 ? root : chain[i - 1], 0.0f, 0.0f));
    }
    make_box(chain[3], 12.0f, 7.0f);

    for (int i = 0; i < 4; ++i)
    {
        lh_entity_flex_t *flex = reinterpret_cast<lh_entity_flex_t *>(chain[i]);
        EXPECT_EQ(lh_entity_flex_content(flex, lh_bool_true), 12);
        EXPECT_EQ(lh_entity_flex_content(flex, lh_bool_false), 7);
    }
    lh_entity_flex_layout_tree(root);
    for (int i = 0; i < 4; ++i)
    {
        expect_size(reinterpret_cast<lh_entity_2d_t *>(chain[i]), 12.0f, 7.0f);
    }
}

TEST_F(Flex, layout_sizes_a_hugging_container_from_its_leaves)
{
    lh_entity_flex_t *outer = make_flex(root, 0.0f, 0.0f);
    lh_entity_flex_set_pad(outer, 5);
    lh_entity_flex_set_gap(outer, 10);
    make_box(of(outer), 30.0f, 20.0f);

    lh_entity_flex_layout_tree(root);
    expect_size(reinterpret_cast<lh_entity_2d_t *>(outer), 40.0f, 30.0f);
}

TEST_F(Flex, the_next_pass_remeasures_after_a_leaf_grows)
{
    lh_entity_flex_t *outer = make_flex(root, 0.0f, 0.0f);
    lh_entity_flex_set_pad(outer, 5);
    lh_entity_2d_t *leaf = make_box(of(outer), 30.0f, 20.0f);

    lh_entity_flex_layout_tree(root);
    expect_size(reinterpret_cast<lh_entity_2d_t *>(outer), 40.0f, 30.0f);

    resize(leaf, 60.0f, 40.0f);
    lh_entity_flex_layout_tree(root);
    expect_size(reinterpret_cast<lh_entity_2d_t *>(outer), 70.0f, 50.0f);
}

TEST_F(Flex, the_next_pass_remeasures_after_the_padding_changes)
{
    lh_entity_flex_t *outer = make_flex(root, 0.0f, 0.0f);
    lh_entity_flex_set_pad(outer, 5);
    make_box(of(outer), 30.0f, 20.0f);

    lh_entity_flex_layout_tree(root);
    expect_size(reinterpret_cast<lh_entity_2d_t *>(outer), 40.0f, 30.0f);

    lh_entity_flex_set_pad(outer, 20);
    lh_entity_flex_layout_tree(root);
    expect_size(reinterpret_cast<lh_entity_2d_t *>(outer), 70.0f, 60.0f);
}

TEST_F(Flex, the_next_pass_remeasures_after_the_gap_changes)
{
    lh_entity_flex_t *outer = make_flex(root, 0.0f, 0.0f);
    make_box(of(outer), 30.0f, 20.0f);
    make_box(of(outer), 50.0f, 40.0f);

    lh_entity_flex_layout_tree(root);
    expect_size(reinterpret_cast<lh_entity_2d_t *>(outer), 80.0f, 40.0f);

    lh_entity_flex_set_gap(outer, 7);
    lh_entity_flex_layout_tree(root);
    expect_size(reinterpret_cast<lh_entity_2d_t *>(outer), 87.0f, 40.0f);
}

TEST_F(Flex, a_leaf_that_only_grows_across_does_not_move_the_other_axis)
{
    lh_entity_flex_t *outer = make_flex(root, 0.0f, 0.0f);
    lh_entity_2d_t *a = make_box(of(outer), 30.0f, 20.0f);
    make_box(of(outer), 50.0f, 40.0f);

    lh_entity_flex_layout_tree(root);
    expect_size(reinterpret_cast<lh_entity_2d_t *>(outer), 80.0f, 40.0f);

    /* Only the width of the first leaf moves. A pass that kept one tag for
       both axes would hand the width back as the height. */
    resize(a, 60.0f, 20.0f);
    lh_entity_flex_layout_tree(root);
    expect_size(reinterpret_cast<lh_entity_2d_t *>(outer), 110.0f, 40.0f);
}

TEST_F(Flex, measuring_outside_layout_does_not_answer_with_a_layout_pass)
{
    lh_entity_flex_t *outer = make_flex(root, 0.0f, 0.0f);
    lh_entity_2d_t *leaf = make_box(of(outer), 30.0f, 20.0f);

    lh_entity_flex_layout_tree(root);
    resize(leaf, 60.0f, 20.0f);
    EXPECT_EQ(lh_entity_flex_content(outer, lh_bool_true), 60);
    EXPECT_EQ(lh_entity_flex_content(outer, lh_bool_false), 20);
}

TEST_F(Flex, a_leaf_that_is_hidden_contributes_nothing)
{
    lh_entity_flex_t *outer = make_flex(root, 0.0f, 0.0f);
    lh_entity_flex_set_gap(outer, 10);
    make_box(of(outer), 30.0f, 20.0f);
    lh_entity_t *hidden = reinterpret_cast<lh_entity_t *>(make_box(of(outer), 50.0f, 40.0f));

    EXPECT_EQ(lh_entity_flex_content(outer, lh_bool_true), 90);
    lh_entity_add_flags(hidden, lh_entity_flags_hidden);
    EXPECT_EQ(lh_entity_flex_content(outer, lh_bool_true), 30);
    EXPECT_EQ(lh_entity_flex_content(outer, lh_bool_false), 20);
}

} // namespace
