/**
 * @file page.h
 * @brief Build the Canvas2D player HTML into a caller-owned byte buffer.
 *
 * Markup is ::lh_ui_web_html; script is ::lh_ui_web_js / ::lh_ui_web_x.
 * Both are constructed first, then serialized into a ::lh_ui_web_buf_t.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_PAGE_H
#define LH_UI_WEB_PAGE_H

#include <lh/ui/web/buf.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/page.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/** `var <name>={'<e.key>':<lh_os_key_t>,...};` from ::page_keys, only the modifiers when
   @p modifiers_only. Fails @p j when the table would not fit ::PAGE_KEY_TABLE_CAP. */
lh_void
lh_ui_web_page_emit_key_table(lh_ui_web_buf_t *j, const lh_str_ptr name, lh_bool_t modifiers_only);

/**
 * @brief Write the player document into @p mem.
 *
 * @param mem     Buffer the document is written into.
 * @param cap     Size of @p mem in bytes.
 * @param out_len Receives the byte length (no trailing NUL counted). May be ::lh_null.
 *
 * @return ::lh_bool_false when the buffer is too small or @p mem is null.
 */
lh_bool_t
lh_ui_web_page_build(lh_byte_ptr mem, lh_usize_t cap, lh_usize_t *out_len);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_PAGE_H */
