/**
 * @file fn.h
 * @brief Handler function type for a selected streaming web route.
 *
 * The handler is called after the request head and framing are accepted, and
 * again each time more of the body has arrived. Each call reads decoded body
 * bytes through the borrowed LH reader until it answers ::LH_IO_PENDING (return
 * true: the host calls again when more arrives), 0 (the body ended and
 * validated) or an error, and stages changes. A handler that returns before
 * either is not called again: the host validates the rest of the body itself.
 * No staged changes may be published before the separate commit callback; the
 * host calls either commit or abort, once.
 * Header fields are the original request fields and are not rewritten after
 * gzip decompression; the body reader yields decompressed content.
 * ::LH_IO_PENDING is not the end of input: return true and wait for the next call.
 */

#ifndef LH_UI_WEB_APP_STREAM_ROUTE_FN_H
#define LH_UI_WEB_APP_STREAM_ROUTE_FN_H

#include <lh/core/http/header.h>
#include <lh/core/http/request.h>
#include <lh/core/io/reader.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Handle one request selected by a streaming route.
 * @param ctx Borrowed application context.
 * @param request Parsed request borrowing the host's head buffer.
 * @param headers Parsed header fields, count stored in @p request.
 * @param body Decoded body reader, valid only during this call.
 * @return True if staging succeeded, false to abort the transaction.
 * @pre All pointers are valid and the handler does not retain borrowed data.
 */
typedef lh_bool_t(lh_ui_web_stream_route_fn)(lh_ptr ctx, const lh_http_request_t *request,
                                             const lh_http_header_t *headers, lh_io_reader_t *body);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_APP_STREAM_ROUTE_FN_H */
