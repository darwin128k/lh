/**
 * @file route.h
 * @brief ::lh_ui_web_route_t — one route of a web application.
 *
 * A route is what a REST surface is made of: the method and the path, and the
 * handler that answers them. The table belongs to the application; the host
 * only walks it.
 *
 * Requires ::LH_LIBRARY_OPTION_UI and ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_UI_WEB_APP_ROUTE_H
#define LH_UI_WEB_APP_ROUTE_H

#include <lh/config.h>
#include <lh/assert/runtime.h>
#include <lh/os/net/socket.h>
#include <lh/ui/web/app/route/fields.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/app/route.h requires LH_LIBRARY_OPTION_UI"
#endif
#if !LH_LIBRARY_OPTION_OS
#    error "lh/ui/web/app/route.h requires LH_LIBRARY_OPTION_OS"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What answers one route.
 *
 * The handler writes the answer on the socket — ::lh_ui_web_host_reply and
 * ::lh_ui_web_host_reply_text are there for it — and says whether it answered.
 *
 * @param ctx    The route's context, as the application has it.
 * @param req    The whole request, as it came in.
 * @param len    Its length.
 * @param client The socket to answer on.
 *
 * @return ::lh_bool_true when the route answered.
 */
typedef lh_bool_t (*lh_ui_web_route_fn)(lh_ptr ctx, const lh_str_ptr req, lh_usize_t len,
                                       lh_os_net_socket_t *client);

LH_COMPILER_EXTERN_C_END

/**
 * @struct lh_ui_web_route
 * @brief One route of a web application: the method and the path it answers,
 *        and the handler that answers them.
 */
struct lh_ui_web_route
{
    /** Members: ::lh_ui_web_route_fields. */
    lh_ui_web_route_fields(const lh_str_ptr, lh_usize_t, const lh_str_ptr, lh_ui_web_route_fn);
};
/** @copybrief lh_ui_web_route */
typedef struct lh_ui_web_route lh_ui_web_route_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The method the route answers (level 1).
 *
 * @param self The route; checked by ::lh_assert_runtime_ref.
 */
const lh_str_ptr
lh_ui_web_route_get_method(const lh_ui_web_route_t *self);

/**
 * @brief Store the method the route answers (level 1).
 *
 * A level-1 setter: the pointer is stored as given and is not validated. The
 * text is not owned and must outlive the route. Its length is stored apart,
 * with ::lh_ui_web_route_set_method_len.
 *
 * @param self   The route; checked by ::lh_assert_runtime_ref.
 * @param method The new method.
 */
lh_void
lh_ui_web_route_set_method(lh_ui_web_route_t *self, const lh_str_ptr method);

/**
 * @brief How long the method the route answers is (level 1).
 *
 * @param self The route; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_ui_web_route_get_method_len(const lh_ui_web_route_t *self);

/**
 * @brief Store how long the method the route answers is (level 1).
 *
 * A level-1 setter: the length is stored as given.
 *
 * @param self       The route; checked by ::lh_assert_runtime_ref.
 * @param method_len The new length.
 */
lh_void
lh_ui_web_route_set_method_len(lh_ui_web_route_t *self, lh_usize_t method_len);

/**
 * @brief The path the route answers (level 1).
 *
 * @param self The route; checked by ::lh_assert_runtime_ref.
 */
const lh_str_ptr
lh_ui_web_route_get_path(const lh_ui_web_route_t *self);

/**
 * @brief Store the path the route answers (level 1).
 *
 * A level-1 setter: the pointer is stored as given and is not validated. The
 * text is not owned and must outlive the route. Its length is stored apart,
 * with ::lh_ui_web_route_set_path_len.
 *
 * @param self The route; checked by ::lh_assert_runtime_ref.
 * @param path The new path.
 */
lh_void
lh_ui_web_route_set_path(lh_ui_web_route_t *self, const lh_str_ptr path);

/**
 * @brief How long the path the route answers is (level 1).
 *
 * @param self The route; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_ui_web_route_get_path_len(const lh_ui_web_route_t *self);

/**
 * @brief Store how long the path the route answers is (level 1).
 *
 * A level-1 setter: the length is stored as given.
 *
 * @param self     The route; checked by ::lh_assert_runtime_ref.
 * @param path_len The new length.
 */
lh_void
lh_ui_web_route_set_path_len(lh_ui_web_route_t *self, lh_usize_t path_len);

/**
 * @brief The handler that answers the route (level 1).
 *
 * @param self The route; checked by ::lh_assert_runtime_ref.
 */
lh_ui_web_route_fn
lh_ui_web_route_get_handler(const lh_ui_web_route_t *self);

/**
 * @brief Store the handler that answers the route (level 1).
 *
 * A level-1 setter: the function pointer is stored as given.
 *
 * @param self    The route; checked by ::lh_assert_runtime_ref.
 * @param handler The new handler.
 */
lh_void
lh_ui_web_route_set_handler(lh_ui_web_route_t *self, lh_ui_web_route_fn handler);

/**
 * @brief Whether @p req is the request this route answers.
 *
 * The request line is `<method> <path>`; the path ends at the space before the
 * HTTP version, at the `?` of the query, or at the end of the request. A route
 * with no method answers any method.
 *
 * @param self The route; checked by ::lh_assert_runtime_ref.
 * @param req  The request, as it came in.
 * @param len  Its length.
 *
 * @return ::lh_bool_true when this route answers the request.
 */
lh_bool_t
lh_ui_web_route_match(const lh_ui_web_route_t *self, const lh_str_ptr req, lh_usize_t len);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_APP_ROUTE_H */
