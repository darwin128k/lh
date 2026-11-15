/**
 * @file status.h
 * @brief Where a streaming request stands after one step of its pipe.
 */

#ifndef LH_GL_WEB_HOST_PIPE_STATUS_H
#define LH_GL_WEB_HOST_PIPE_STATUS_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_gl_web_host_pipe_status_t
 * @brief Result of ::lh_gl_web_host_pipe_step.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_gl_web_host_pipe_status_*`
 * constants below.
 */
typedef lh_u8_t lh_gl_web_host_pipe_status_t;

/** @def lh_gl_web_host_pipe_status_pending
 *  @brief More of the body is needed: step again when the socket is readable. */
#define lh_gl_web_host_pipe_status_pending 0U

/** @def lh_gl_web_host_pipe_status_committed
 *  @brief The body validated to its end and the route's commit ran. */
#define lh_gl_web_host_pipe_status_committed 1U

/** @def lh_gl_web_host_pipe_status_aborted
 *  @brief The handler refused, the body failed or the time ran out; abort ran. */
#define lh_gl_web_host_pipe_status_aborted 2U

#endif /* LH_GL_WEB_HOST_PIPE_STATUS_H */
