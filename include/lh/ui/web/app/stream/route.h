/**
 * @file route.h
 * @brief One web route whose decoded request body is read incrementally.
 *
 * Reuses an ordinary route as a method/path matcher; its buffered handler is
 * not used. The matcher and callback remain caller-owned. Limits apply to the
 * decoded body and aggregate chunk metadata independently. The host invokes
 * the staging handler synchronously, validates the remaining content, then calls
 * exactly one commit or abort callback. Staging must not publish changes or send
 * a response; abort discards staged changes. Callbacks own application state.
 */

#ifndef LH_UI_WEB_APP_STREAM_ROUTE_H
#define LH_UI_WEB_APP_STREAM_ROUTE_H

#include <lh/ui/web/app/route.h>
#include <lh/ui/web/app/stream/route/cb.h>
#include <lh/ui/web/app/stream/route/finish/cb.h>
#include <lh/ui/web/app/stream/route/fields.h>

/** @brief Borrowed route matcher and handler with separate body limits. */
struct lh_ui_web_stream_route
{
    /** Members: ::lh_ui_web_stream_route_fields. */
    lh_ui_web_stream_route_fields(const lh_ui_web_route_t *, lh_ui_web_stream_route_cb,
                                  lh_ui_web_stream_route_finish_cb, lh_usize_t);
};
/** @copybrief lh_ui_web_stream_route */
typedef struct lh_ui_web_stream_route lh_ui_web_stream_route_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Return the stored match; @p self must be valid. */
const lh_ui_web_route_t *
lh_ui_web_stream_route_get_match(const lh_ui_web_stream_route_t *self);

/** Store match without validation; @p self must be valid. */
lh_void
lh_ui_web_stream_route_set_match(lh_ui_web_stream_route_t *self, const lh_ui_web_route_t *value);

/** Return the stored handler; @p self must be valid. */
lh_ui_web_stream_route_cb
lh_ui_web_stream_route_get_handler(const lh_ui_web_stream_route_t *self);

/** Store handler without validation; @p self must be valid. */
lh_void
lh_ui_web_stream_route_set_handler(lh_ui_web_stream_route_t *self, lh_ui_web_stream_route_cb value);

/** Return the callback that publishes staged changes after validation. */
lh_ui_web_stream_route_finish_cb
lh_ui_web_stream_route_get_commit(const lh_ui_web_stream_route_t *self);

/** Store the callback used to publish staged changes. */
lh_void
lh_ui_web_stream_route_set_commit(lh_ui_web_stream_route_t *self,
                                  lh_ui_web_stream_route_finish_cb value);

/** Return the callback that discards staged changes on failure. */
lh_ui_web_stream_route_finish_cb
lh_ui_web_stream_route_get_abort(const lh_ui_web_stream_route_t *self);

/** Store the callback used to discard staged changes. */
lh_void
lh_ui_web_stream_route_set_abort(lh_ui_web_stream_route_t *self,
                                 lh_ui_web_stream_route_finish_cb value);

/** Return the stored body_limit; @p self must be valid. */
lh_usize_t
lh_ui_web_stream_route_get_body_limit(const lh_ui_web_stream_route_t *self);

/** Store body_limit without validation; @p self must be valid. */
lh_void
lh_ui_web_stream_route_set_body_limit(lh_ui_web_stream_route_t *self, lh_usize_t value);

/** Return the stored metadata_limit; @p self must be valid. */
lh_usize_t
lh_ui_web_stream_route_get_metadata_limit(const lh_ui_web_stream_route_t *self);

/** Store metadata_limit without validation; @p self must be valid. */
lh_void
lh_ui_web_stream_route_set_metadata_limit(lh_ui_web_stream_route_t *self, lh_usize_t value);

/** Return the stored total request timeout in milliseconds. */
lh_u32_t
lh_ui_web_stream_route_get_timeout_ms(const lh_ui_web_stream_route_t *self);

/** Store the total request timeout without validation. */
lh_void
lh_ui_web_stream_route_set_timeout_ms(lh_ui_web_stream_route_t *self, lh_u32_t value);

/**
 * @brief Initialize a route using a borrowed matcher, handler and limits.
 * @param self Route to initialize.
 * @param match Caller-owned ordinary route used only for method/path matching.
 * @param handler Callback to invoke for a match; it may not be null.
 * @param commit Callback to publish staged changes on success.
 * @param abort Callback to discard staged changes on failure.
 * @param body_limit Maximum decoded content bytes.
 * @param metadata_limit Maximum chunk metadata bytes, including trailers.
 * @param timeout_ms Total request timeout in milliseconds, including the head.
 * @pre All pointers are valid; matcher and all callbacks outlive @p self.
 */
lh_void
lh_ui_web_stream_route_init(lh_ui_web_stream_route_t *self, const lh_ui_web_route_t *match,
                            lh_ui_web_stream_route_cb handler,
                            lh_ui_web_stream_route_finish_cb commit,
                            lh_ui_web_stream_route_finish_cb abort, lh_usize_t body_limit,
                            lh_usize_t metadata_limit, lh_u32_t timeout_ms);

/**
 * @brief Test a request against the borrowed method/path matcher.
 * @param self Valid streaming route.
 * @param request Parsed request, borrowed.
 * @return True if the route matches and has all three callbacks, false otherwise.
 * @pre The route, matcher and request are valid.
 */
lh_bool_t
lh_ui_web_stream_route_is_match(const lh_ui_web_stream_route_t *self,
                                const lh_http_request_t *request);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_APP_STREAM_ROUTE_H */
