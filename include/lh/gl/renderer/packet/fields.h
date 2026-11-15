/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_renderer_packet_t.
 */

#ifndef LH_GL_RENDERER_PACKET_FIELDS_H
#define LH_GL_RENDERER_PACKET_FIELDS_H

#include <lh/config.h>

/**
 * @def lh_gl_renderer_packet_fields(task_type, renderer_type, rect_type, count_type)
 * @brief A draw request: the rail task, the renderer it draws, and the dirty
 *        rectangles copied at the post.
 *
 * @param task_type     ::lh_rail_task_t. `task` is the request rail carries.
 *                      It must stay where it is from a successful post until
 *                      its destroy runs.
 * @param renderer_type ::lh_gl_renderer_t. `renderer` is borrowed.
 * @param rect_type     ::lh_gl_rect_t. `area` holds the dirty rectangles taken
 *                      from the surface. The surface list is free for new
 *                      invalidation after a successful post.
 * @param count_type    ::lh_usize_t. `count` is how many of `area` are occupied.
 */
#define lh_gl_renderer_packet_fields(task_type, renderer_type, rect_type, count_type)              \
    task_type task;                                                                                \
    renderer_type *renderer;                                                                       \
    count_type count;                                                                              \
    rect_type area[LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX]

#endif /* LH_GL_RENDERER_PACKET_FIELDS_H */
