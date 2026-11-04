/**
 * @file capacity.h
 * @brief The web host's fixed sizes and request time limit.
 *
 * One family of constants: what one HTTP connection holds (its request buffer,
 * its chunk line, its parsed fields), how many connections and how long a
 * request may take. The host's records are sized by them at build time.
 */

#ifndef LH_UI_WEB_HOST_CAPACITY_H
#define LH_UI_WEB_HOST_CAPACITY_H

/**
 * Request head and buffered-request wire capacity, in bytes.
 * Streaming routes reuse this capacity as scratch and may receive larger bodies.
 *
 * Define this before building LH to change the size of every connection.
 * ::lh_ui_web_host_try_read_request instead uses its caller's buffer size.
 */
#ifndef LH_UI_WEB_HOST_REQUEST_CAPACITY
#    define LH_UI_WEB_HOST_REQUEST_CAPACITY 1024u
#endif

/** Maximum chunk size line or trailer line held by a streaming request. */
#ifndef LH_UI_WEB_HOST_CHUNK_LINE_CAPACITY
#    define LH_UI_WEB_HOST_CHUNK_LINE_CAPACITY 1024u
#endif

/** Header fields one request may carry. */
#define LH_UI_WEB_HOST_HEADER_MAX 32u

/** HTTP connections served at once (requests still arriving); one more waits in
 *  the listen queue. */
#ifndef LH_UI_WEB_HOST_CONN_MAX
#    define LH_UI_WEB_HOST_CONN_MAX 8u
#endif

/** How long an accepted connection may take to send its complete request. */
#define LH_UI_WEB_HOST_REQUEST_MS 2000u

#endif /* LH_UI_WEB_HOST_CAPACITY_H */
