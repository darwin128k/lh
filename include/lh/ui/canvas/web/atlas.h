/**
 * @file atlas.h
 * @brief The glyph covers a web canvas has sent, so each crosses the wire once:
 *        ::lh_ui_canvas_web_atlas_t.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_CANVAS_WEB_ATLAS_H
#define LH_UI_CANVAS_WEB_ATLAS_H

#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/ui/canvas/web/atlas/fields.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/canvas/web/atlas.h requires LH_LIBRARY_OPTION_UI"
#endif

/** How many distinct glyph covers the process-wide intern table holds. */
#define LH_UI_CANVAS_WEB_GLYPHS_MAX 1024u

/** Largest cover, in pixels, that is interned; bigger ones go inline every time. */
#define LH_UI_CANVAS_WEB_ATLAS_PIX 4096

/** Hash slots: twice the entries, a power of two. */
#define LH_UI_CANVAS_WEB_ATLAS_SLOTS (2u * LH_UI_CANVAS_WEB_GLYPHS_MAX)

/**
 * @struct lh_ui_canvas_web_atlas
 * @brief Every distinct glyph cover sent so far, by id, with a hash index over them.
 */
struct lh_ui_canvas_web_atlas
{
    /** Members: ::lh_ui_canvas_web_atlas_fields. */
    lh_ui_canvas_web_atlas_fields(lh_uint_t, lh_s32_t, lh_u32_t, lh_byte_ptr, lh_sint_t,
                                  LH_UI_CANVAS_WEB_GLYPHS_MAX, LH_UI_CANVAS_WEB_ATLAS_SLOTS);
};
/** @copybrief lh_ui_canvas_web_atlas */
typedef struct lh_ui_canvas_web_atlas lh_ui_canvas_web_atlas_t;

LH_COMPILER_EXTERN_C_BEGIN

/** The id of the @p n pixels @p px of a @p w x @p h cover hashed to @p hash, or -1 with
   @p slot the empty place they would go. */
lh_sint_t
lh_ui_canvas_web_atlas_find(const lh_ui_canvas_web_atlas_t *self, const lh_byte_ptr px, lh_uint_t n,
                            lh_s32_t w, lh_s32_t h, lh_u32_t hash, lh_uint_t *slot);

/** Copy a new cover into the table at hash @p slot; its id, or -1 when the table is full or
   the copy could not be allocated. */
lh_sint_t
lh_ui_canvas_web_atlas_add(lh_ui_canvas_web_atlas_t *self, const lh_byte_ptr px, lh_uint_t n,
                           lh_s32_t w, lh_s32_t h, lh_u32_t hash, lh_uint_t slot);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_WEB_ATLAS_H */
