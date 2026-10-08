/**
 * @file decl.h
 * @brief A tree written as a table: ::lh_ui_decl_t.
 *
 * The same tree an app builds by hand, written as rows instead of as calls. There
 * is no parser and no markup language: a declaration is a `const` array of
 * ::lh_ui_decl_t that the compiler puts in flash, and ::lh_ui_decl_build places
 * one widget per row into memory the app owns and wires parent to child.
 *
 * **A declaration describes the tree, not the materials.** Styles, texts, masks,
 * layouts and click callbacks are ordinary C that the app already has, and a row
 * points at them — that is deliberate. A markup language would have to invent a
 * spelling for a fill, a shadow and a radius, and then a parser for it; here a
 * style stays what it already is, a call to ::lh_ui_style_set_fill, and the thing
 * being declared is the one part that is painful to write by hand: who is a child
 * of whom, where it sits, which flow places it.
 *
 * **The app owns the memory.** ::lh_ui_decl_build takes a byte pool and a stride,
 * because `lh` does not own an allocator: a table of rows is data, and the widgets
 * it makes are the app's. Nothing here allocates, parses or frees.
 *
 * The contract is that a tree from a declaration is **indistinguishable** from the
 * same tree written by hand: same rects, same classes, same damage, same picture.
 * That is what the tests compare, and it is the only claim this file makes.
 *
 * A row's parent must be a **lower index**: a child cannot exist before the thing
 * it is a child of. ::lh_ui_decl_build refuses anything else and says which row.
 */

#ifndef LH_UI_DECL_H
#define LH_UI_DECL_H

#include <lh/byte.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ptr.h>
#include <lh/ui/button.h>
#include <lh/ui/container.h>
#include <lh/ui/entity.h>
#include <lh/ui/image.h>
#include <lh/ui/label.h>
#include <lh/ui/layout.h>
#include <lh/ui/layout/place.h>
#include <lh/ui/scrollbar.h>
#include <lh/ui/style.h>
#include <lh/ui/toggle.h>
#include <lh/util/addr.h>
#include <lh/void.h>

/**
 * @brief @p parent of a row that is a root: no parent, and exactly one row of a
 *        declaration may say it.
 */
#define LH_UI_DECL_ROOT ((lh_s32_t)(-1))

/**
 * @enum lh_ui_decl_kind
 * @brief What a row builds. Each is the component of the same name, and each is
 *        the thing an app would have declared by hand.
 */
typedef enum lh_ui_decl_kind
{
    lh_ui_decl_kind_entity = 0, /**< A plain ::lh_ui_entity_t: a fill and nothing else. */
    lh_ui_decl_kind_container,  /**< ::lh_ui_container_t, with the flow that places its children. */
    lh_ui_decl_kind_label,      /**< ::lh_ui_label_t and its text. */
    lh_ui_decl_kind_button,     /**< ::lh_ui_button_t, its two looks and its click. */
    lh_ui_decl_kind_toggle,     /**< ::lh_ui_toggle_t, both pairs of looks and its state. */
    lh_ui_decl_kind_image,      /**< ::lh_ui_image_t, its mask and its tint. */
    lh_ui_decl_kind_scrollbar   /**< ::lh_ui_scrollbar_t over the container of another row. */
} lh_ui_decl_kind_t;

/**
 * @struct lh_ui_decl
 * @typedef lh_ui_decl_t
 * @brief One row: one widget, where it sits, who holds it, and what it is made
 *        of.
 *
 * The fields above `as` are the same for every kind and mean the same thing. What
 * is under `as` depends on the kind, so a button row never carries a mask and a
 * label row never carries a callback. Nothing here is owned except the node the
 * build places: styles, layouts, texts, masks, tints and callbacks all outlive it
 * by the app's own rules, exactly as they do when the tree is written by hand.
 */
typedef struct lh_ui_decl
{
    lh_ui_decl_kind_t kind; /**< What this row builds. */
    lh_ui_scalar_t x;       /**< Left of the rect, in the parent's space (absolute, as everywhere). */
    lh_ui_scalar_t y;       /**< Top of the rect. */
    lh_ui_scalar_t w;       /**< Width of the rect. */
    lh_ui_scalar_t h;       /**< Height of the rect. */
    lh_s32_t parent;        /**< Row that holds this one, or ::LH_UI_DECL_ROOT. Must be lower. */
    const lh_ui_style_t *style; /**< Look of @p self (not owned, may be ::lh_null). */
    const lh_ui_place_t *place; /**< What this row wants a flow to give it (not owned, may be ::lh_null). */
    /**
     * @brief The flow that places this row's children (not owned, may be
     *        ::lh_null: no flow, and its children stay where they were put).
     *
     * One field for every kind, because a flow belongs to a **container** and a
     * button, a toggle and a caption all are one — that is what makes a captioned
     * button three entities and still places them. A row whose widget is not a
     * container ignores it.
     */
    const lh_ui_layout_t *layout;

    /** What this row carries, by kind: only the block that matches @p kind is read. */
    union
    {
        /** lh_ui_decl_kind_label */
        struct
        {
            const lh_char_t *text; /**< Caption (not owned, may be ::lh_null for none). */
        } label;

        /** lh_ui_decl_kind_button */
        struct
        {
            const lh_ui_style_t *hot_style;    /**< Look while the pointer is on it. */
            lh_ui_button_on_click_cb on_click; /**< What a click means (may be ::lh_null). */
            lh_ptr click_context;              /**< Handed to @p on_click untouched. */
        } button;

        /** lh_ui_decl_kind_toggle */
        struct
        {
            const lh_ui_style_t *off_style;     /**< Resting look while off. */
            const lh_ui_style_t *off_hot_style; /**< Pointer look while off. */
            const lh_ui_style_t *on_style;      /**< Resting look while on. */
            const lh_ui_style_t *on_hot_style;  /**< Pointer look while on. */
            lh_bool_t checked;                  /**< The state it starts in. */
        } toggle;

        /** lh_ui_decl_kind_image */
        struct
        {
            const lh_ui_mask_t *mask;      /**< What it shows (may be ::lh_null for none). */
            const lh_ui_color_t *tint;     /**< The colour it shows it in (may be ::lh_null: white). */
        } image;

        /** lh_ui_decl_kind_scrollbar */
        struct
        {
            lh_s32_t box;                  /**< Row of the container it scrolls (lower than this one). */
            lh_ui_axis_t axis;             /**< Axis it scrolls on. */
            lh_ui_scrollbar_mode_t mode;   /**< When it is shown (default auto). */
        } scrollbar;
    } as;
} lh_ui_decl_t;

/**
 * @enum lh_ui_decl_error
 * @brief What a build refused, and the one question asked of every row.
 */
typedef enum lh_ui_decl_error
{
    lh_ui_decl_error_none = 0,     /**< It built. */
    lh_ui_decl_error_kind,         /**< @c kind is not a widget this build knows. */
    lh_ui_decl_error_parent,       /**< @c parent is out of range, or not lower than this row. */
    lh_ui_decl_error_roots,        /**< Not exactly one row is a root. */
    lh_ui_decl_error_box,          /**< A scrollbar names a row that is not a container, or is higher. */
    lh_ui_decl_error_storage      /**< The pool is too small for these rows. */
} lh_ui_decl_error_t;

/**
 * @struct lh_ui_decl_fault
 * @typedef lh_ui_decl_fault_t
 * @brief Which row, and what was wrong with it. A table is data, so a typo in one
 *        row has to say which row — that is the whole difference between a
 *        declaration and a call sequence.
 */
typedef struct lh_ui_decl_fault
{
    lh_ui_decl_error_t code; /**< ::lh_ui_decl_error_none when the build worked. */
    lh_u32_t index;          /**< Row that was refused; the last row for a whole-table fault. */
} lh_ui_decl_fault_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Build every row of @p decls into @p storage and wire parent to child.
 *
 * @param decls   Rows, in any kind. Not owned; may be a static table.
 * @param count   How many rows.
 * @param nodes   Receives the ::lh_ui_entity_t of every row, so a caller can reach
 *                a child later (a scrollbar's container, a row to press). Not owned.
 * @param storage Bytes the widgets are placed in. Not owned, not freed. Rows are
 *                laid **densely**: row N starts where row N-1 ended, sized by the
 *                widget that row builds. There is no stride to pass and therefore
 *                no way to pass one by mistake — the two numbers a caller could
 *                mix up are exactly the two that cost a silent overrun.
 * @param bytes   Size of @p storage. A pool too small for the rows is refused
 *                rather than written past.
 * @param root    Receives the row that is a root, the tree the view takes.
 * @param fault   Receives the row and the reason, when the build refuses. May be
 *                ::lh_null when the app does not want to know.
 *
 * @return ::lh_bool_true when every row was built. A refused build says which row,
 *         and nothing is wired until every row is true, so there is never a
 *         half-built tree that looks finished.
 */
lh_bool_t
lh_ui_decl_build(const lh_ui_decl_t *decls, lh_u32_t count, lh_ui_entity_t **nodes, lh_byte_t *storage,
                 lh_u32_t bytes, lh_ui_entity_t **root, lh_ui_decl_fault_t *fault);

/**
 * @brief Bytes one row takes when it is any widget at all: the widest node, so an
 *        app that wants a static pool of a known row count can size it without
 *        measuring anything. Dense packing usually needs less.
 */
#define LH_UI_DECL_NODE_MAX (sizeof(lh_ui_toggle_t))

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_DECL_H */
