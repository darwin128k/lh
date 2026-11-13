#include <gtest/gtest.h>

#include <climits>
#include <lh/test/alloc_counter.h>
#include <lh/expect/death.h>
#include <lh/os/system/posix/surface.h>
#include <lh/os/system/surface.h>

TEST(os_posix_surface_fit, samples_pixels_centres_bars_and_reuses_the_buffer)
{
    auto source = lh_os_system_surface_create(2, 2, 32);
    ASSERT_NE(source, nullptr);
    auto pixels = static_cast<lh_u32_t *>(lh_os_system_surface_get_pixels(source));
    pixels[0] = 0x00ff0000;
    pixels[1] = 0x0000ff00;
    pixels[2] = 0x000000ff;
    pixels[3] = 0x00ffffff;
    auto backend = static_cast<lh_os_system_posix_surface_t *>(source);
    auto scaled = lh_os_system_posix_surface_try_prepare_fit(backend, 6, 4, 0x00332211);
    ASSERT_NE(scaled, nullptr);
    auto out = static_cast<const lh_u32_t *>(lh_os_system_surface_get_pixels(scaled));
    for (int y = 0; y < 4; ++y)
    {
        EXPECT_EQ(out[y * 6], 0x00112233u);
        EXPECT_EQ(out[y * 6 + 5], 0x00112233u);
        for (int x = 1; x < 5; ++x)
        {
            EXPECT_EQ(out[y * 6 + x], pixels[(y / 2) * 2 + (x - 1) / 2]);
        }
    }
    EXPECT_EQ(lh_os_system_posix_surface_try_prepare_fit(backend, 6, 4, 0), scaled);
    auto smaller = lh_os_system_posix_surface_try_prepare_fit(backend, 1, 1, 0);
    ASSERT_NE(smaller, nullptr);
    EXPECT_EQ(*static_cast<lh_u32_t *>(lh_os_system_surface_get_pixels(smaller)), pixels[0]);
    EXPECT_EQ(lh_os_system_posix_surface_try_prepare_fit(backend, -1, 1, 0), nullptr);
    EXPECT_EQ(lh_os_system_posix_surface_try_prepare_fit(backend, INT_MAX, 1, 0), nullptr);
    EXPECT_EQ(lh_os_system_posix_surface_get_fit(backend), smaller);
    LH_EXPECT_CHECK_DEATH(lh_os_system_posix_surface_prepare_fit(backend, -1, 1, 0));
    lh_os_system_surface_destroy(source);
}

TEST(os_posix_surface_fit, widens_rgb565_and_handles_fractional_scaling)
{
    auto source = lh_os_system_surface_create(2, 1, 16);
    ASSERT_NE(source, nullptr);
    auto pixels = static_cast<lh_u16_t *>(lh_os_system_surface_get_pixels(source));
    pixels[0] = 0xf800;
    pixels[1] = 0x07e0;
    auto backend = static_cast<lh_os_system_posix_surface_t *>(source);
    auto scaled = lh_os_system_posix_surface_try_prepare_fit(backend, 3, 1, 0);
    ASSERT_NE(scaled, nullptr);
    auto out = static_cast<const lh_u32_t *>(lh_os_system_surface_get_pixels(scaled));
    // 2:1 in a 3x1 destination stays 2x1, integer fit rounds down.
    EXPECT_EQ(out[0], 0x00ff0000u);
    EXPECT_EQ(out[1], 0x0000ff00u);
    EXPECT_EQ(out[2], 0u);
    scaled = lh_os_system_posix_surface_try_prepare_fit(backend, 6, 3, 0);
    out = static_cast<const lh_u32_t *>(lh_os_system_surface_get_pixels(scaled));
    EXPECT_EQ(out[2], 0x00ff0000u);
    EXPECT_EQ(out[3], 0x0000ff00u);
    lh_os_system_surface_destroy(source);
}

TEST(os_posix_surface_fit, fractional_scale_uses_floor_nearest_samples)
{
    auto source = lh_os_system_surface_create(4, 2, 32);
    ASSERT_NE(source, nullptr);
    auto pixels = static_cast<lh_u32_t *>(lh_os_system_surface_get_pixels(source));
    for (int i = 0; i < 8; ++i)
    {
        pixels[i] = i + 1;
    }
    auto scaled = lh_os_system_posix_surface_try_prepare_fit(
        static_cast<lh_os_system_posix_surface_t *>(source), 6, 3, 0);
    ASSERT_NE(scaled, nullptr);
    auto out = static_cast<const lh_u32_t *>(lh_os_system_surface_get_pixels(scaled));
    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 6; ++x)
        {
            EXPECT_EQ(out[y * 6 + x], pixels[(y * 2 / 3) * 4 + x * 4 / 6]);
        }
    }
    lh_os_system_surface_destroy(source);
}

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
fit_deny_alloc(lh_self_ptr, lh_usize_t)
{
    return nullptr;
}

LH_COMPILER_EXTERN_C_END

TEST(os_posix_surface_fit, steady_frames_do_not_allocate_and_failed_growth_keeps_cache)
{
    lh_test::alloc_counter counter;
    auto source = lh_os_system_surface_create(2, 2, 32);
    ASSERT_NE(source, nullptr);
    auto backend = static_cast<lh_os_system_posix_surface_t *>(source);
    auto scaled = lh_os_system_posix_surface_try_prepare_fit(backend, 4, 4, 0);
    ASSERT_NE(scaled, nullptr);
    int allocations = counter.count();
    for (int frame = 0; frame < 20; ++frame)
    {
        EXPECT_EQ(lh_os_system_posix_surface_try_prepare_fit(backend, 4, 4, 0), scaled);
    }
    EXPECT_EQ(counter.count(), allocations);
    auto previous = lh_memory_allocator_get_alloc_cb(lh_runtime_allocator_get());
    lh_memory_allocator_set_alloc_cb(lh_runtime_allocator_get_mut(), fit_deny_alloc);
    auto failed = lh_os_system_posix_surface_try_prepare_fit(backend, 8, 8, 0);
    lh_memory_allocator_set_alloc_cb(lh_runtime_allocator_get_mut(), previous);
    EXPECT_EQ(failed, nullptr);
    EXPECT_EQ(lh_os_system_posix_surface_get_fit(backend), scaled);
    EXPECT_EQ(lh_os_system_posix_surface_try_prepare_fit(backend, 4, 4, 0), scaled);
    lh_os_system_surface_destroy(source);
}
