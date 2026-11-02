/**
 * @file x_fields.cpp
 * @brief JavaScript expression field accessors.
 */

#include <gtest/gtest.h>

#include <lh/ui/web/x.h>

TEST(ui_web_x_get_kind, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_kind(lh_addr_of(node), lh_ui_web_x_kind_id);
    EXPECT_EQ(lh_ui_web_x_get_kind(lh_addr_of(node)), lh_ui_web_x_kind_id);
}

TEST(ui_web_x_set_kind, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_kind(lh_addr_of(node), lh_ui_web_x_kind_id);
    EXPECT_EQ(lh_ui_web_x_get_kind(lh_addr_of(node)), lh_ui_web_x_kind_id);
}

TEST(ui_web_x_get_id, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_id(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_id(lh_addr_of(node)), "value");
}

TEST(ui_web_x_set_id, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_id(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_id(lh_addr_of(node)), "value");
}

TEST(ui_web_x_get_num, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_num(lh_addr_of(node), 7);
    EXPECT_EQ(lh_ui_web_x_get_num(lh_addr_of(node)), 7);
}

TEST(ui_web_x_set_num, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_num(lh_addr_of(node), 7);
    EXPECT_EQ(lh_ui_web_x_get_num(lh_addr_of(node)), 7);
}

TEST(ui_web_x_get_str, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_str(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_str(lh_addr_of(node)), "value");
}

TEST(ui_web_x_set_str, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_str(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_str(lh_addr_of(node)), "value");
}

TEST(ui_web_x_get_dot_obj, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_dot_obj(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_dot_obj(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_dot_obj, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_dot_obj(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_dot_obj(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_dot_name, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_dot_name(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_dot_name(lh_addr_of(node)), "value");
}

TEST(ui_web_x_set_dot_name, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_dot_name(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_dot_name(lh_addr_of(node)), "value");
}

TEST(ui_web_x_get_idx_obj, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_idx_obj(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_idx_obj(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_idx_obj, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_idx_obj(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_idx_obj(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_idx_ix, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_idx_ix(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_idx_ix(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_idx_ix, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_idx_ix(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_idx_ix(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_call_fn, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_call_fn(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_call_fn(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_call_fn, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_call_fn(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_call_fn(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_call_args, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    const lh_ui_web_x_t *nodes[1] = {lh_addr_of(node)};
    lh_ui_web_x_set_call_args(lh_addr_of(node), nodes);
    EXPECT_EQ(lh_ui_web_x_get_call_args(lh_addr_of(node)), nodes);
}

TEST(ui_web_x_set_call_args, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    const lh_ui_web_x_t *nodes[1] = {lh_addr_of(node)};
    lh_ui_web_x_set_call_args(lh_addr_of(node), nodes);
    EXPECT_EQ(lh_ui_web_x_get_call_args(lh_addr_of(node)), nodes);
}

TEST(ui_web_x_get_call_n, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_call_n(lh_addr_of(node), 7);
    EXPECT_EQ(lh_ui_web_x_get_call_n(lh_addr_of(node)), 7);
}

TEST(ui_web_x_set_call_n, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_call_n(lh_addr_of(node), 7);
    EXPECT_EQ(lh_ui_web_x_get_call_n(lh_addr_of(node)), 7);
}

TEST(ui_web_x_get_call_id_name, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_call_id_name(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_call_id_name(lh_addr_of(node)), "value");
}

TEST(ui_web_x_set_call_id_name, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_call_id_name(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_call_id_name(lh_addr_of(node)), "value");
}

TEST(ui_web_x_get_call_id_args, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    const lh_ui_web_x_t *nodes[1] = {lh_addr_of(node)};
    lh_ui_web_x_set_call_id_args(lh_addr_of(node), nodes);
    EXPECT_EQ(lh_ui_web_x_get_call_id_args(lh_addr_of(node)), nodes);
}

TEST(ui_web_x_set_call_id_args, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    const lh_ui_web_x_t *nodes[1] = {lh_addr_of(node)};
    lh_ui_web_x_set_call_id_args(lh_addr_of(node), nodes);
    EXPECT_EQ(lh_ui_web_x_get_call_id_args(lh_addr_of(node)), nodes);
}

TEST(ui_web_x_get_call_id_n, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_call_id_n(lh_addr_of(node), 7);
    EXPECT_EQ(lh_ui_web_x_get_call_id_n(lh_addr_of(node)), 7);
}

TEST(ui_web_x_set_call_id_n, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_call_id_n(lh_addr_of(node), 7);
    EXPECT_EQ(lh_ui_web_x_get_call_id_n(lh_addr_of(node)), 7);
}

TEST(ui_web_x_get_bin_a, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_bin_a(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_bin_a(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_bin_a, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_bin_a(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_bin_a(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_bin_op, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_bin_op(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_bin_op(lh_addr_of(node)), "value");
}

TEST(ui_web_x_set_bin_op, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_bin_op(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_bin_op(lh_addr_of(node)), "value");
}

TEST(ui_web_x_get_bin_b, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_bin_b(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_bin_b(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_bin_b, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_bin_b(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_bin_b(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_un_op, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_un_op(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_un_op(lh_addr_of(node)), "value");
}

TEST(ui_web_x_set_un_op, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_un_op(lh_addr_of(node), "value");
    EXPECT_STREQ(lh_ui_web_x_get_un_op(lh_addr_of(node)), "value");
}

TEST(ui_web_x_get_un_a, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_un_a(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_un_a(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_un_a, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_un_a(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_un_a(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_tern_c, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_tern_c(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_tern_c(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_tern_c, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_tern_c(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_tern_c(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_tern_t, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_tern_t(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_tern_t(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_tern_t, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_tern_t(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_tern_t(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_tern_f, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_tern_f(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_tern_f(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_tern_f, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_tern_f(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_tern_f(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_typeof_a, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_typeof_a(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_typeof_a(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_set_typeof_a, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_t other;
    lh_ui_web_x_set_typeof_a(lh_addr_of(node), lh_addr_of(other));
    EXPECT_EQ(lh_ui_web_x_get_typeof_a(lh_addr_of(node)), lh_addr_of(other));
}

TEST(ui_web_x_get_obj_keys, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    const lh_str_ptr keys[1] = {"key"};
    lh_ui_web_x_set_obj_keys(lh_addr_of(node), keys);
    EXPECT_EQ(lh_ui_web_x_get_obj_keys(lh_addr_of(node)), keys);
}

TEST(ui_web_x_set_obj_keys, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    const lh_str_ptr keys[1] = {"key"};
    lh_ui_web_x_set_obj_keys(lh_addr_of(node), keys);
    EXPECT_EQ(lh_ui_web_x_get_obj_keys(lh_addr_of(node)), keys);
}

TEST(ui_web_x_get_obj_vals, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    const lh_ui_web_x_t *nodes[1] = {lh_addr_of(node)};
    lh_ui_web_x_set_obj_vals(lh_addr_of(node), nodes);
    EXPECT_EQ(lh_ui_web_x_get_obj_vals(lh_addr_of(node)), nodes);
}

TEST(ui_web_x_set_obj_vals, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    const lh_ui_web_x_t *nodes[1] = {lh_addr_of(node)};
    lh_ui_web_x_set_obj_vals(lh_addr_of(node), nodes);
    EXPECT_EQ(lh_ui_web_x_get_obj_vals(lh_addr_of(node)), nodes);
}

TEST(ui_web_x_get_obj_n, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_obj_n(lh_addr_of(node), 7);
    EXPECT_EQ(lh_ui_web_x_get_obj_n(lh_addr_of(node)), 7);
}

TEST(ui_web_x_set_obj_n, stores_or_reads_the_value)
{
    lh_ui_web_x_t node;
    lh_ui_web_x_set_obj_n(lh_addr_of(node), 7);
    EXPECT_EQ(lh_ui_web_x_get_obj_n(lh_addr_of(node)), 7);
}
