#include <gtest/gtest.h>

#include <lh/entity/2d.h>
#include <lh/entity/3d.h>
#include <lh/entity/rect.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/null.h>
#include <lh/self.h>

#include <cstdlib>

namespace
{

const lh_float_t k_pi = 3.14159265f;
const lh_float_t k_eps = 1e-4f;

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
spatial_test_alloc(lh_self_ptr, lh_usize_t size)
{
    return std::malloc(static_cast<std::size_t>(size));
}

lh_void
spatial_test_dealloc(lh_self_ptr, lh_ptr ptr)
{
    std::free(ptr);
}

LH_COMPILER_EXTERN_C_END

void
expect_vec3_near(lh_math_vec3_t v, lh_float_t x, lh_float_t y, lh_float_t z)
{
    EXPECT_NEAR(v.x, x, k_eps);
    EXPECT_NEAR(v.y, y, k_eps);
    EXPECT_NEAR(v.z, z, k_eps);
}

class Spatial : public ::testing::Test
{
  protected:
    void
    SetUp() override
    {
        sized = lh_memory_allocator_initializer_with_context(
            spatial_test_alloc, spatial_test_dealloc, lh_null, lh_null);
        root = lh_entity_create_root(&lh_entity_base_class, &sized);
    }

    void
    TearDown() override
    {
        lh_entity_delete(root);
    }

    lh_entity_2d_t *
    make_2d(lh_entity_t *parent)
    {
        return reinterpret_cast<lh_entity_2d_t *>(lh_entity_create(&lh_entity_2d_class, parent));
    }

    lh_entity_rect_t *
    make_rect(lh_entity_t *parent, lh_float_t x, lh_float_t y, lh_float_t w, lh_float_t h)
    {
        lh_entity_rect_t *r =
            reinterpret_cast<lh_entity_rect_t *>(lh_entity_create(&lh_entity_rect_class, parent));
        lh_entity_2d_set_position(reinterpret_cast<lh_entity_2d_t *>(r), lh_math_vec2_make(x, y));
        lh_entity_rect_set_size(r, lh_math_vec2_make(w, h));
        return r;
    }

    static lh_entity_t *
    as_entity(void *p)
    {
        return static_cast<lh_entity_t *>(p);
    }

    lh_memory_sized_allocator_t sized;
    lh_entity_t *root;
};

TEST_F(Spatial, classes_derive_2d_then_rect_and_3d)
{
    lh_entity_t *r = as_entity(make_rect(root, 0, 0, 1, 1));
    lh_entity_t *d = lh_entity_create(&lh_entity_3d_class, root);
    EXPECT_TRUE(lh_entity_is_instance_of(r, &lh_entity_2d_class));
    EXPECT_TRUE(lh_entity_is_instance_of(d, &lh_entity_2d_class));
    EXPECT_TRUE(lh_entity_is_instance_of(d, &lh_entity_base_class));
    EXPECT_FALSE(lh_entity_is_instance_of(d, &lh_entity_rect_class));
    EXPECT_FALSE(lh_entity_is_instance_of(root, &lh_entity_2d_class));
}

TEST_F(Spatial, new_2d_entity_is_at_the_parent_origin)
{
    lh_entity_2d_t *e = make_2d(root);
    EXPECT_FLOAT_EQ(lh_entity_2d_get_position(e).x, 0.0f);
    EXPECT_FLOAT_EQ(lh_entity_2d_get_angle(e), 0.0f);
    EXPECT_FLOAT_EQ(lh_entity_2d_get_scale(e).y, 1.0f);
    EXPECT_TRUE(lh_math_mat4_near(lh_entity_2d_get_world_matrix(e), lh_math_mat4_identity(), 0.0f));
}

TEST_F(Spatial, children_move_rotate_and_scale_with_the_parent)
{
    lh_entity_2d_t *parent = make_2d(root);
    lh_entity_2d_set_position(parent, lh_math_vec2_make(100, 50));
    lh_entity_2d_set_angle(parent, k_pi / 2); // quarter turn: +x goes to +y
    lh_entity_2d_set_scale(parent, lh_math_vec2_make(2, 2));

    lh_entity_2d_t *child = make_2d(as_entity(parent));
    lh_entity_2d_set_position(child, lh_math_vec2_make(10, 0));

    // Child origin: (10,0) -> scaled (20,0) -> rotated (0,20) -> moved (100,70).
    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(child);
    expect_vec3_near(lh_math_mat4_transform_point(world, lh_math_vec3_make(0, 0, 0)), 100, 70, 0);
}

TEST_F(Spatial, plain_containers_in_between_count_as_no_transform)
{
    lh_entity_2d_t *outer = make_2d(root);
    lh_entity_2d_set_position(outer, lh_math_vec2_make(5, 5));
    lh_entity_t *group = lh_entity_create(&lh_entity_base_class, as_entity(outer));
    lh_entity_2d_t *inner = make_2d(group);
    lh_entity_2d_set_position(inner, lh_math_vec2_make(1, 2));

    expect_vec3_near(
        lh_math_mat4_transform_point(lh_entity_2d_get_world_matrix(inner), lh_math_vec3_make(0, 0, 0)), 6, 7,
        0);
}

TEST_F(Spatial, entity_3d_keeps_xy_in_its_2d_part)
{
    lh_entity_3d_t *e =
        reinterpret_cast<lh_entity_3d_t *>(lh_entity_create(&lh_entity_3d_class, root));
    expect_vec3_near(lh_entity_3d_get_scale(e), 1, 1, 1);
    lh_entity_3d_set_position(e, lh_math_vec3_make(1, 2, 3));
    expect_vec3_near(lh_entity_3d_get_position(e), 1, 2, 3);
    const lh_math_vec2_t xy = lh_entity_2d_get_position(reinterpret_cast<lh_entity_2d_t *>(e));
    EXPECT_FLOAT_EQ(xy.x, 1.0f);
    EXPECT_FLOAT_EQ(xy.y, 2.0f);
}

TEST_F(Spatial, entity_3d_answers_with_its_full_matrix)
{
    lh_entity_3d_t *e =
        reinterpret_cast<lh_entity_3d_t *>(lh_entity_create(&lh_entity_3d_class, root));
    lh_entity_3d_set_position(e, lh_math_vec3_make(0, 0, 10));
    lh_entity_3d_set_rotation(e, lh_math_quat_from_axis_angle(lh_math_vec3_make(1, 0, 0), k_pi / 2));
    lh_entity_3d_set_scale(e, lh_math_vec3_make(1, 1, 3));

    // Asked through the 2D function, the 3D class answers: z, rotation, z scale.
    const lh_math_mat4_t local = lh_entity_2d_get_local_matrix(reinterpret_cast<lh_entity_2d_t *>(e));
    // (0,1,0) about +x by 90 degrees -> (0,0,1); then +10 on z.
    expect_vec3_near(lh_math_mat4_transform_point(local, lh_math_vec3_make(0, 1, 0)), 0, 0, 11);
    // z is scaled by 3 first: (0,0,1) -> (0,0,3) -> rotated to (0,-3,0).
    expect_vec3_near(lh_math_mat4_transform_point(local, lh_math_vec3_make(0, 0, 1)), 0, -3, 10);
}

TEST_F(Spatial, rect_inside_a_3d_parent_is_placed_in_the_scene)
{
    lh_entity_3d_t *stand =
        reinterpret_cast<lh_entity_3d_t *>(lh_entity_create(&lh_entity_3d_class, root));
    lh_entity_3d_set_position(stand, lh_math_vec3_make(0, 0, -5));
    lh_entity_rect_t *sign = make_rect(as_entity(stand), 1, 1, 4, 2);

    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(reinterpret_cast<lh_entity_2d_t *>(sign));
    expect_vec3_near(lh_math_mat4_transform_point(world, lh_math_vec3_make(0, 0, 0)), 1, 1, -5);
}

TEST_F(Spatial, rect_contains_is_half_open)
{
    lh_entity_rect_t *r = make_rect(root, 10, 20, 30, 40);
    EXPECT_TRUE(lh_entity_rect_contains(r, lh_math_vec2_make(10, 20)));
    EXPECT_TRUE(lh_entity_rect_contains(r, lh_math_vec2_make(39.9f, 59.9f)));
    EXPECT_FALSE(lh_entity_rect_contains(r, lh_math_vec2_make(40, 30)));
    EXPECT_FALSE(lh_entity_rect_contains(r, lh_math_vec2_make(9.9f, 30)));
}

TEST_F(Spatial, rect_contains_follows_rotation)
{
    // 10x2 bar turned a quarter: now it runs down from (0,0) along +y.
    lh_entity_rect_t *bar = make_rect(root, 0, 0, 10, 2);
    lh_entity_2d_set_angle(reinterpret_cast<lh_entity_2d_t *>(bar), k_pi / 2);
    EXPECT_TRUE(lh_entity_rect_contains(bar, lh_math_vec2_make(-1, 8)));
    EXPECT_FALSE(lh_entity_rect_contains(bar, lh_math_vec2_make(8, 1)));
}

TEST_F(Spatial, rect_scaled_to_nothing_contains_no_point)
{
    lh_entity_rect_t *r = make_rect(root, 0, 0, 10, 10);
    lh_entity_2d_set_scale(reinterpret_cast<lh_entity_2d_t *>(r), lh_math_vec2_make(0, 1));
    EXPECT_FALSE(lh_entity_rect_contains(r, lh_math_vec2_make(0, 0)));
}

TEST_F(Spatial, find_at_returns_what_is_on_top)
{
    lh_entity_rect_t *window = make_rect(root, 0, 0, 100, 100);
    lh_entity_rect_t *panel = make_rect(as_entity(window), 10, 10, 50, 50);
    lh_entity_rect_t *button = make_rect(as_entity(panel), 5, 5, 10, 10); // at (15,15) on screen
    lh_entity_rect_t *popup = make_rect(as_entity(window), 0, 0, 20, 20); // younger: on top

    EXPECT_EQ(lh_entity_rect_find_at(root, lh_math_vec2_make(90, 90)), as_entity(window));
    EXPECT_EQ(lh_entity_rect_find_at(root, lh_math_vec2_make(40, 40)), as_entity(panel));
    EXPECT_EQ(lh_entity_rect_find_at(root, lh_math_vec2_make(22, 22)), as_entity(button));
    EXPECT_EQ(lh_entity_rect_find_at(root, lh_math_vec2_make(16, 16)), as_entity(popup));
    EXPECT_EQ(lh_entity_rect_find_at(root, lh_math_vec2_make(200, 5)), nullptr);
}

} // namespace
