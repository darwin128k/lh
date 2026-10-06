/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_style_t.
 */

#ifndef LH_UI_STYLE_FIELDS_H
#define LH_UI_STYLE_FIELDS_H

#include <lh/byte.h>

/**
 * @def lh_ui_style_fields()
 * @brief Paint recipe for one entity. Contents are not settled yet.
 *
 * `_reserved` keeps the struct non-empty in C; it is not part of the public
 * API and will go away when real fields arrive.
 */
#define lh_ui_style_fields()                                                                        \
    lh_byte_t _reserved

#endif /* LH_UI_STYLE_FIELDS_H */
