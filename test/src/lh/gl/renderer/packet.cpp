#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/expect/death.h>
#include <lh/gl/canvas.h>
#include <lh/gl/layer.h>
#include <lh/gl/renderer/packet.h>
#include <lh/gl/surface.h>
#include <lh/rail/inline.h>
#include <lh/rail/mailbox.h>

namespace
{

const lh_gl_entity_t *drawn[4];
lh_gl_rect_t clips[4];
lh_usize_t drawn_count;

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t w, lh_gl_scalar_t h)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, w, h);
    return rect;
}

lh_void
record_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas, const lh_gl_place_t *place)
{
    const lh_usize_t index = drawn_count++;

    (void)canvas;
    drawn[index] = entity;
    clips[index] = lh_ptr_deref(lh_gl_place_get_clip(place));
}

TEST(gl_renderer_packet, post_draws_the_copy_and_clears_the_list)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, record_draw);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 100, 100);
    lh_gl_surface_t surface;

    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_layer_t root;

    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 100, 100));
    lh_gl_entity_t child;

    lh_gl_entity_init(lh_addr_of(child), rect_of(0, 50, 100, 10), lh_addr_of(cls));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(child));
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(root));
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;

    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_renderer_t renderer;

    lh_gl_renderer_init(lh_addr_of(renderer), lh_addr_of(surface), lh_addr_of(canvas), 0);
    lh_gl_renderer_packet_t packet;

    lh_gl_renderer_packet_init(lh_addr_of(packet), lh_addr_of(renderer));
    lh_rail_mailbox_t mailbox;

    lh_rail_mailbox_init(lh_addr_of(mailbox), 1U);
    drawn_count = 0;
    EXPECT_TRUE(lh_gl_renderer_packet_try_post(lh_addr_of(packet), lh_rail_inline_get_executor(),
                                               lh_addr_of(mailbox)));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    ASSERT_EQ(drawn_count, 1U);
    EXPECT_EQ(drawn[0], lh_addr_of(child));
    EXPECT_EQ(lh_gl_renderer_packet_get_count(lh_addr_of(packet)), 1U);
    lh_gl_surface_invalidate(lh_addr_of(surface),
                             lh_gl_entity_get_rect(lh_addr_of(child)));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_EQ(lh_gl_renderer_packet_get_count(lh_addr_of(packet)), 1U);
    EXPECT_EQ(lh_rail_mailbox_dispatch(lh_addr_of(mailbox)), 1U);
    EXPECT_EQ(lh_rail_mailbox_get_used_count(lh_addr_of(mailbox)), 0U);
    lh_gl_layer_deinit(lh_addr_of(root));
}

TEST(gl_renderer_packet, a_full_mailbox_leaves_the_dirty_list)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, record_draw);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 20, 20);
    lh_gl_surface_t surface;

    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_layer_t root;

    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 20, 20));
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(root));
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;

    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_renderer_t renderer;

    lh_gl_renderer_init(lh_addr_of(renderer), lh_addr_of(surface), lh_addr_of(canvas), 0);
    lh_gl_renderer_packet_t packet;

    lh_gl_renderer_packet_init(lh_addr_of(packet), lh_addr_of(renderer));
    lh_rail_mailbox_t mailbox;

    lh_rail_mailbox_init(lh_addr_of(mailbox), 0U);
    drawn_count = 0;
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_FALSE(lh_gl_renderer_packet_try_post(lh_addr_of(packet), lh_rail_inline_get_executor(),
                                                 lh_addr_of(mailbox)));
    EXPECT_EQ(drawn_count, 0U);
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    LH_EXPECT_CHECK_DEATH(lh_gl_renderer_packet_post(lh_addr_of(packet),
                                                      lh_rail_inline_get_executor(),
                                                      lh_addr_of(mailbox)));
    lh_gl_layer_deinit(lh_addr_of(root));
}

} /* namespace */
