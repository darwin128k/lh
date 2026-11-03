/**
 * @file cb.h
 * @brief Callback pointer for finishing a streaming request transaction.
 *
 * The pointed-to callback commits or discards application-owned staged data.
 */

#ifndef LH_UI_WEB_APP_STREAM_ROUTE_FINISH_CB_H
#define LH_UI_WEB_APP_STREAM_ROUTE_FINISH_CB_H

#include <lh/ui/web/app/stream/route/finish/fn.h>

/** @brief Pointer to a streaming transaction finish function. */
typedef lh_ui_web_stream_route_finish_fn *lh_ui_web_stream_route_finish_cb;

#endif /* LH_UI_WEB_APP_STREAM_ROUTE_FINISH_CB_H */
