/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_image_t.
 */

#ifndef LH_UI_IMAGE_FIELDS_H
#define LH_UI_IMAGE_FIELDS_H

/**
 * @def lh_ui_image_fields(entity_type, mask_type, color_type)
 * @brief The entity this image is, the mask it shows, and the colour it shows
 *        it in.
 *
 * The mask is not owned: a baked icon is a constant, and the font's glyphs are
 * masks too, so the same bytes can be shown by anything. The colour is a field
 * and not the style's text paint the way a label's text is, because a mask is
 * the whole of what an image shows: an icon that changes with a toggle changes
 * its own colour (::lh_ui_image_set_tint), and swapping a style for that would
 * make the thing that is not being pressed the thing that carries the look.
 *
 * @param entity_type Type of the embedded entity.
 * @param mask_type   Type of the mask pointer.
 * @param color_type  Type of the tint.
 */
#define lh_ui_image_fields(entity_type, mask_type, color_type) \
    entity_type entity;                                       \
    const mask_type *mask;                                    \
    color_type tint

#endif /* LH_UI_IMAGE_FIELDS_H */