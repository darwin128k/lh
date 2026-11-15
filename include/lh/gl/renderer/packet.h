/**
 * @file packet.h
 * @brief A draw request carried by rail: ::lh_gl_renderer_packet_t.
 *
 * The renderer draws on the thread that runs the task. Posting copies the
 * surface's dirty rectangles into the packet and clears that list, so the
 * thread that invalidates can keep going while another loop draws this frame.
 * The copy is what the task draws. Nothing is allocated.
 *
 * The same loop draws by posting to the inline executor: the work runs before
 * post returns, and complete still waits for the mailbox dispatch. Another
 * loop draws by posting to the executor that loop runs. A full mailbox posts
 * nothing and leaves the dirty list as it was.
 */

#ifndef LH_GL_RENDERER_PACKET_H
#define LH_GL_RENDERER_PACKET_H

#include <lh/gl/rect.h>
#include <lh/gl/renderer.h>
#include <lh/gl/renderer/packet/fields.h>
#include <lh/rail/executor.h>
#include <lh/rail/task.h>

/**
 * @struct lh_gl_renderer_packet
 * @brief One draw request. Fields via ::lh_gl_renderer_packet_fields.
 */
struct lh_gl_renderer_packet
{
    /** Members: ::lh_gl_renderer_packet_fields. */
    lh_gl_renderer_packet_fields(lh_rail_task_t, lh_gl_renderer_t, lh_gl_rect_t, lh_usize_t);
};
/** @copybrief lh_gl_renderer_packet */
typedef struct lh_gl_renderer_packet lh_gl_renderer_packet_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The rail task this request is. */
lh_rail_task_t *
lh_gl_renderer_packet_get_task_mut(lh_gl_renderer_packet_t *self);

/** @brief The rail task this request is. */
const lh_rail_task_t *
lh_gl_renderer_packet_get_task(const lh_gl_renderer_packet_t *self);

/** @brief Borrowed renderer, or ::lh_null. */
lh_gl_renderer_t *
lh_gl_renderer_packet_get_renderer(const lh_gl_renderer_packet_t *self);

/** @brief Store the borrowed renderer. */
lh_void
lh_gl_renderer_packet_set_renderer(lh_gl_renderer_packet_t *self, lh_gl_renderer_t *renderer);

/** @brief How many dirty rectangles this request holds. */
lh_usize_t
lh_gl_renderer_packet_get_count(const lh_gl_renderer_packet_t *self);

/** @brief Store how many dirty rectangles this request holds. */
lh_void
lh_gl_renderer_packet_set_count(lh_gl_renderer_packet_t *self, lh_usize_t count);

/** @brief Dirty rectangle @p index. @p index must be below the surface capacity. */
lh_gl_rect_t *
lh_gl_renderer_packet_get_area_mut(lh_gl_renderer_packet_t *self, lh_uindex_t index);

/** @brief Dirty rectangle @p index. @p index must be below the surface capacity. */
const lh_gl_rect_t *
lh_gl_renderer_packet_get_area(const lh_gl_renderer_packet_t *self, lh_uindex_t index);

/**
 * @brief A request that draws @p renderer. The task's user pointer is @p self.
 *
 * Complete and destroy are null. Do not move @p self after a successful post
 * until the task's destroy would have run.
 */
lh_void
lh_gl_renderer_packet_init(lh_gl_renderer_packet_t *self, lh_gl_renderer_t *renderer);

/**
 * @brief The task's work: draw the rectangles copied into @p user.
 *
 * @p local is unused. @p user is the ::lh_gl_renderer_packet_t.
 */
lh_void
lh_gl_renderer_packet_work(lh_ptr user, lh_ptr local);

/**
 * @brief Copy the renderer's dirty rectangles, clear them, and submit the task.
 *
 * On refusal the dirty list is put back and the task stays the caller's.
 * A null renderer or a surface with no root posts nothing.
 *
 * @param self     Request; must not be null.
 * @param executor Executor that runs the draw; must not be null.
 * @param mailbox  Owner mailbox; must not be null.
 * @return ::lh_bool_true when the executor accepted the task.
 */
lh_bool_t
lh_gl_renderer_packet_try_post(lh_gl_renderer_packet_t *self, const lh_rail_executor_t *executor,
                               lh_rail_mailbox_t *mailbox);

/**
 * @brief ::lh_gl_renderer_packet_try_post, and a refusal is a failed check.
 *
 * @fails ::lh_runtime_error_code_not_accepted when the post is refused.
 */
lh_void
lh_gl_renderer_packet_post(lh_gl_renderer_packet_t *self, const lh_rail_executor_t *executor,
                           lh_rail_mailbox_t *mailbox);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_RENDERER_PACKET_H */
