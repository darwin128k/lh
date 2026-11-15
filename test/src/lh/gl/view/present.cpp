#include <gtest/gtest.h>

#include <lh/gl/view.h>
#include <lh/core/memory/std.h>

static lh_gl_rects_t view_presented;

void
view_present_begin(lh_ptr)
{
}

void
view_present_fill(lh_ptr, const lh_gl_rect_t *, const lh_gl_color_t *)
{
}

void
view_present_clip(lh_ptr, const lh_gl_canvas_clip_t *)
{
}

void
view_present_end(lh_ptr, const lh_gl_rects_t *drawn)
{
    lh_gl_rects_init(&view_presented);
    for (lh_u32_t i = 0; i < lh_gl_rects_get_count(drawn); ++i)
    {
        lh_gl_rects_add(&view_presented, lh_gl_rects_get(drawn, i));
    }
}

TEST(ui_view_presentation, full_frame_keeps_unclipped_header_after_child_clips)
{
    lh_gl_canvas_backend_t backend;
    lh_memory_std_set(&backend, 0, sizeof(backend));
    lh_gl_canvas_backend_set_begin_fn(&backend, view_present_begin);
    lh_gl_canvas_backend_set_fill_rect_fn(&backend, view_present_fill);
    lh_gl_canvas_backend_set_set_clip_fn(&backend, view_present_clip);
    lh_gl_canvas_backend_set_end_fn(&backend, view_present_end);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(&canvas, &backend, lh_null);
    lh_gl_rect_t whole;
    lh_gl_rect_init(&whole, 0, 0, 200, 100);
    lh_gl_entity_t root;
    lh_gl_entity_init(&root, whole);
    lh_gl_rect_t rect;
    lh_gl_rect_init(&rect, 0, 20, 200, 60);
    lh_gl_container_t viewport;
    lh_gl_container_init(&viewport, rect);
    lh_gl_entity_add_child(&root, lh_gl_container_as_entity(&viewport));
    lh_gl_entity_t child;
    lh_gl_entity_init(&child, rect);
    lh_gl_entity_add_child(lh_gl_container_as_entity(&viewport), &child);
    lh_gl_view_t view;
    lh_gl_view_init(&view);
    lh_gl_view_set_canvas(&view, &canvas);
    lh_gl_view_set_root_v(&view, &root);
    lh_gl_view_draw(&view, lh_null);
    ASSERT_EQ(lh_gl_rects_get_count(&view_presented), 1U);
    EXPECT_TRUE(lh_gl_rect_eq(lh_gl_rects_get(&view_presented, 0), &whole));
    lh_gl_rect_t damage;
    lh_gl_rect_init(&damage, 10, 30, 40, 20);
    lh_gl_view_draw(&view, &damage);
    ASSERT_EQ(lh_gl_rects_get_count(&view_presented), 1U);
    EXPECT_TRUE(lh_gl_rect_eq(lh_gl_rects_get(&view_presented, 0), &damage));
    lh_gl_view_set_root_v(&view, lh_null);
    lh_gl_view_deinit(&view);
    lh_gl_canvas_deinit(&canvas);
}
