/**
 * @file kind.h
 * @brief The class of an entity: ::lh_gx_entity_kind_t.
 */

#ifndef LH_GX_ENTITY_KIND_H
#define LH_GX_ENTITY_KIND_H

/**
 * @enum lh_gx_entity_kind
 * @brief What an entity is.
 *
 * The class says what a node of the tree is, and the tree reads it: a layer
 * holds entities, a label shows text, a button shows text and can be pressed.
 * There is one entity type and one kind per class — the kind is what tells
 * them apart.
 *
 * @see lh_gx_entity_t
 */
typedef enum lh_gx_entity_kind
{
    /** An empty slot: nothing is drawn and nothing is hit. */
    lh_gx_entity_kind_none = 0,
    /** Shows a text. */
    lh_gx_entity_kind_label,
    /** Shows a text and can be pressed; reports the reactions of a press. */
    lh_gx_entity_kind_button,
    /** Holds other entities. */
    lh_gx_entity_kind_layer,
    /** Shows a picture. */
    lh_gx_entity_kind_image,
    /** Holds other entities and scrolls them. */
    lh_gx_entity_kind_scroll
} lh_gx_entity_kind_t;

#endif /* LH_GX_ENTITY_KIND_H */
