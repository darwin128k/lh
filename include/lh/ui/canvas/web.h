/**
 * @file web.h
 * @brief Canvas backend that records primitives as JSON: ::lh_ui_canvas_backend_web.
 *
 * Same slot table as GDI and the software pixmap. A browser that replays the
 * bytes is painting this stream, not a DOM layout. XP never loads that player;
 * the desktop there stays GDI. Paladin, cfg and a visual editor all take this
 * backend from here, not a private copy in an application.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_CANVAS_WEB_H
#define LH_UI_CANVAS_WEB_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/size.h>
#include <lh/ui/canvas/backend.h>
#include <stdio.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/canvas/web.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief An open JSON array of canvas ops, to a file or to a buffer the caller owns.
 */
typedef struct lh_ui_canvas_web
{
    FILE *out;
    char *mem;
    lh_usize_t cap;
    lh_usize_t len;
    unsigned width;
    unsigned height;
    unsigned ops;
    int comma;
    lh_bool_t ok;
} lh_ui_canvas_web_t;

/** The table ::lh_ui_canvas_init wants. */
extern const lh_ui_canvas_backend_t lh_ui_canvas_backend_web;

/**
 * @brief Start a file of ops for a @p width × @p height target.
 */
lh_bool_t
lh_ui_canvas_web_open(lh_ui_canvas_web_t *self, const char *path, unsigned width, unsigned height);

/**
 * @brief Start a buffer of ops. @p buf is the caller's and is not freed here.
 */
lh_bool_t
lh_ui_canvas_web_open_mem(lh_ui_canvas_web_t *self, char *buf, lh_usize_t cap, unsigned width,
                          unsigned height);

/**
 * @brief Close the array. A file is closed; a buffer stays valid at ::lh_ui_canvas_web_bytes.
 */
lh_bool_t
lh_ui_canvas_web_close(lh_ui_canvas_web_t *self);

/** Bytes of JSON after a successful memory close, else "". */
const char *
lh_ui_canvas_web_bytes(const lh_ui_canvas_web_t *self);

/** How many of those bytes. A file stream answers 0. */
lh_usize_t
lh_ui_canvas_web_length(const lh_ui_canvas_web_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_WEB_H */
