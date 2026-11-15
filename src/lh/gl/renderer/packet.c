/**
 * @file packet.c
 * @brief A renderer draw request handed to rail.
 */

#include <lh/gl/renderer/packet.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/attribute/unused.h>
#include <lh/gl/surface.h>

lh_rail_task_t *
lh_gl_renderer_packet_get_task_mut(lh_gl_renderer_packet_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->task);
}

const lh_rail_task_t *
lh_gl_renderer_packet_get_task(const lh_gl_renderer_packet_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->task);
}

lh_gl_renderer_t *
lh_gl_renderer_packet_get_renderer(const lh_gl_renderer_packet_t *self)
{
    lh_assert_runtime_ref(self);
    return self->renderer;
}

lh_void
lh_gl_renderer_packet_set_renderer(lh_gl_renderer_packet_t *self, lh_gl_renderer_t *renderer)
{
    lh_assert_runtime_ref(self);
    self->renderer = renderer;
}

lh_usize_t
lh_gl_renderer_packet_get_count(const lh_gl_renderer_packet_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_gl_renderer_packet_set_count(lh_gl_renderer_packet_t *self, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_gl_rect_t *
lh_gl_renderer_packet_get_area_mut(lh_gl_renderer_packet_t *self, lh_uindex_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime(index < LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX,
                      lh_runtime_error_code_out_of_range);
    return lh_addr_of(self->area[index]);
}

const lh_gl_rect_t *
lh_gl_renderer_packet_get_area(const lh_gl_renderer_packet_t *self, lh_uindex_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime(index < LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX,
                      lh_runtime_error_code_out_of_range);
    return lh_addr_of(self->area[index]);
}

lh_void
lh_gl_renderer_packet_init(lh_gl_renderer_packet_t *self, lh_gl_renderer_t *renderer)
{
    lh_rail_task_init(lh_gl_renderer_packet_get_task_mut(self), lh_gl_renderer_packet_work, lh_null,
                      lh_null, self);
    lh_gl_renderer_packet_set_renderer(self, renderer);
    lh_gl_renderer_packet_set_count(self, 0U);
}

lh_void
lh_gl_renderer_packet_work(lh_ptr user, lh_ptr local)
{
    LH_ATTRIBUTE_UNUSED(local);

    lh_gl_renderer_packet_t *self = lh_ptr_cast(lh_gl_renderer_packet_t, user);
    lh_gl_renderer_t *renderer = lh_gl_renderer_packet_get_renderer(self);

    if (lh_ptr_is_null(renderer))
    {
        return;
    }
    lh_usize_t count = lh_gl_renderer_packet_get_count(self);
    const lh_gl_rect_t *area = count > 0U ? lh_gl_renderer_packet_get_area(self, 0U) : lh_null;

    lh_gl_renderer_draw(renderer, area, count);
}

lh_bool_t
lh_gl_renderer_packet_try_post(lh_gl_renderer_packet_t *self, const lh_rail_executor_t *executor,
                               lh_rail_mailbox_t *mailbox)
{
    lh_assert_runtime_ref(executor);
    lh_assert_runtime_ref(mailbox);

    lh_gl_renderer_t *renderer = lh_gl_renderer_packet_get_renderer(self);

    if (lh_ptr_is_null(renderer))
    {
        return lh_bool_false;
    }
    lh_gl_surface_t *surface = lh_gl_renderer_get_surface(renderer);

    if (lh_ptr_is_null(surface) || lh_ptr_is_null(lh_gl_surface_get_root(surface)))
    {
        return lh_bool_false;
    }
    lh_usize_t count = lh_gl_surface_get_dirty_count(surface);
    for (lh_uindex_t i = 0U; i < count; ++i)
    {
        lh_ptr_deref(lh_gl_renderer_packet_get_area_mut(self, i)) =
            lh_ptr_deref(lh_gl_surface_get_dirty(surface, i));
    }
    lh_gl_renderer_packet_set_count(self, count);
    lh_gl_surface_clear_dirty(surface);
    lh_bool_t posted =
        lh_rail_executor_try_submit(executor, lh_gl_renderer_packet_get_task_mut(self), mailbox);

    if (posted == lh_bool_false)
    {
        for (lh_uindex_t i = 0U; i < count; ++i)
        {
            lh_ptr_deref(lh_gl_surface_get_dirty_mut(surface, i)) =
                lh_ptr_deref(lh_gl_renderer_packet_get_area(self, i));
        }
        lh_gl_surface_set_dirty_count(surface, count);
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_void
lh_gl_renderer_packet_post(lh_gl_renderer_packet_t *self, const lh_rail_executor_t *executor,
                           lh_rail_mailbox_t *mailbox)
{
    lh_runtime_check_ifn(lh_gl_renderer_packet_try_post(self, executor, mailbox),
                         lh_runtime_error_code_not_accepted);
}
