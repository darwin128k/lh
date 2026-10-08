/**
 * @file decl.c
 * @brief Implementation of `lh/ui/decl.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/decl.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

static lh_void
lh_ui_decl_fault(lh_ui_decl_fault_t *fault, lh_ui_decl_error_t code, lh_u32_t index)
{
    if (lh_null_ne(fault))
    {
        fault->code = code;
        fault->index = index;
    }
}

static lh_u32_t
lh_ui_decl_size_of(lh_ui_decl_kind_t kind)
{
    switch (kind)
    {
    case lh_ui_decl_kind_entity:
        return (lh_u32_t)sizeof(lh_ui_entity_t);
    case lh_ui_decl_kind_container:
        return (lh_u32_t)sizeof(lh_ui_container_t);
    case lh_ui_decl_kind_label:
        return (lh_u32_t)sizeof(lh_ui_label_t);
    case lh_ui_decl_kind_button:
        return (lh_u32_t)sizeof(lh_ui_button_t);
    case lh_ui_decl_kind_toggle:
        return (lh_u32_t)sizeof(lh_ui_toggle_t);
    case lh_ui_decl_kind_image:
        return (lh_u32_t)sizeof(lh_ui_image_t);
    case lh_ui_decl_kind_scrollbar:
        return (lh_u32_t)sizeof(lh_ui_scrollbar_t);
    default:
        return 0;
    }
}

/* Everything a row can be wrong on its own, before a single widget is placed: what
   it is, whether a slot can hold it, and what it points at. A build that stopped
   half way through would leave a tree that looks finished and is not. */
static lh_ui_decl_error_t
lh_ui_decl_check(const lh_ui_decl_t *decls, lh_u32_t index)
{
    const lh_ui_decl_t *row = lh_addr_of(decls[index]);

    if (lh_ui_decl_size_of(row->kind) == 0)
    {
        return lh_ui_decl_error_kind;
    }
    /* A child cannot exist before the thing that holds it, so a parent is a lower
       index or nothing at all. */
    if (row->parent != LH_UI_DECL_ROOT && (row->parent < 0 || (lh_u32_t)row->parent >= index))
    {
        return lh_ui_decl_error_parent;
    }
    /* A scrollbar names the container it scrolls. Whether that row really is one is
       asked once the nodes are placed, because the answer is a property of the
       component the row builds and not of the kind it wrote. */
    if (row->kind == lh_ui_decl_kind_scrollbar &&
        (row->as.scrollbar.box < 0 || (lh_u32_t)row->as.scrollbar.box >= index))
    {
        return lh_ui_decl_error_box;
    }
    return lh_ui_decl_error_none;
}

/* One row into one slot: the component its kind names and that kind's own fields,
   and nothing of any other kind. The style and the place are the caller's, because
   they are the same whatever the widget is. */
static lh_ui_entity_t *
lh_ui_decl_place_row(const lh_ui_decl_t *row, lh_byte_t *slot, lh_ui_entity_t **nodes)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), row->x, row->y, row->w, row->h);

    switch (row->kind)
    {
    case lh_ui_decl_kind_entity:
    {
        lh_ui_entity_t *entity = lh_ptr_rcast(lh_ui_entity_t, slot);

        lh_ui_entity_init(entity, rect);
        return entity;
    }
    case lh_ui_decl_kind_container:
    {
        lh_ui_container_t *box = lh_ptr_rcast(lh_ui_container_t, slot);

        lh_ui_container_init(box, rect);
        return lh_ui_container_as_entity(box);
    }
    case lh_ui_decl_kind_label:
    {
        lh_ui_label_t *label = lh_ptr_rcast(lh_ui_label_t, slot);

        lh_ui_label_init(label, rect, row->as.label.text);
        return lh_ui_label_as_entity(label);
    }
    case lh_ui_decl_kind_button:
    {
        lh_ui_button_t *button = lh_ptr_rcast(lh_ui_button_t, slot);

        lh_ui_button_init(button, rect);
        lh_ui_button_set_style(button, row->style);
        lh_ui_button_set_hot_style(button, row->as.button.hot_style);
        lh_ui_button_set_on_click(button, row->as.button.on_click, row->as.button.click_context);
        return lh_ui_button_as_entity(button);
    }
    case lh_ui_decl_kind_toggle:
    {
        lh_ui_toggle_t *toggle = lh_ptr_rcast(lh_ui_toggle_t, slot);

        lh_ui_toggle_init(toggle, rect);
        lh_ui_toggle_set_off_style(toggle, row->as.toggle.off_style, row->as.toggle.off_hot_style);
        lh_ui_toggle_set_on_style(toggle, row->as.toggle.on_style, row->as.toggle.on_hot_style);
        lh_ui_toggle_set_checked(toggle, row->as.toggle.checked);
        return lh_ui_toggle_as_entity(toggle);
    }
    case lh_ui_decl_kind_image:
    {
        lh_ui_image_t *image = lh_ptr_rcast(lh_ui_image_t, slot);

        lh_ui_image_init(image, rect, row->as.image.mask);
        if (lh_null_ne(row->as.image.tint))
        {
            lh_ui_image_set_tint(image, row->as.image.tint);
        }
        return lh_ui_image_as_entity(image);
    }
    case lh_ui_decl_kind_scrollbar:
    {
        lh_ui_scrollbar_t *bar = lh_ptr_rcast(lh_ui_scrollbar_t, slot);
        lh_ui_container_t *box = lh_ui_entity_as_container(nodes[row->as.scrollbar.box]);

        lh_ui_scrollbar_init(bar, rect, row->as.scrollbar.axis, box);
        /* An undeclared mode is the one the component starts with, so a row that
           says nothing gets what a hand-written bar gets. */
        if (row->as.scrollbar.mode != (lh_ui_scrollbar_mode_t)0)
        {
            lh_ui_scrollbar_set_mode(bar, row->as.scrollbar.mode);
        }
        return lh_ui_scrollbar_as_entity(bar);
    }
    default:
        return lh_null;
    }
}

lh_bool_t
lh_ui_decl_build(const lh_ui_decl_t *decls, lh_u32_t count, lh_ui_entity_t **nodes, lh_byte_t *storage,
                 lh_u32_t bytes, lh_ui_entity_t **root, lh_ui_decl_fault_t *fault)
{
    lh_ui_entity_t *tree = lh_null;
    lh_u32_t roots = 0;
    lh_u32_t needed = 0;
    lh_u32_t index;

    lh_ui_decl_fault(fault, lh_ui_decl_error_none, count);
    lh_return_if(lh_null_eq(decls) || lh_null_eq(nodes) || lh_null_eq(storage) || lh_null_eq(root) ||
                     count == 0,
                 lh_bool_false);

    for (index = 0; index < count; ++index)
    {
        lh_ui_decl_error_t code = lh_ui_decl_check(decls, index);

        if (code != lh_ui_decl_error_none)
        {
            lh_ui_decl_fault(fault, code, index);
            return lh_bool_false;
        }
        /* Every sizeof is a multiple of the alignment of what it holds, so packing
           rows end to end keeps every widget aligned. */
        needed += lh_ui_decl_size_of(decls[index].kind);
        if (decls[index].parent == LH_UI_DECL_ROOT)
        {
            ++roots;
        }
    }
    /* One tree, one root: a table with two is not a mistake a build can guess at. */
    if (roots != 1)
    {
        lh_ui_decl_fault(fault, lh_ui_decl_error_roots, count);
        return lh_bool_false;
    }
    /* Measured before anything is written, which is the whole point of asking: the
       alternative is a pool that is too small and a row that lands outside it. */
    if (bytes < needed)
    {
        lh_ui_decl_fault(fault, lh_ui_decl_error_storage, count);
        return lh_bool_false;
    }

    /* Place every node first, then ask what a scrollbar needs of a row that is now
       a real widget, then wire the tree: nothing is connected until all of it is
       true, so a refused build is not a tree that looks finished. */
    {
        lh_u32_t used = 0;

        for (index = 0; index < count; ++index)
        {
            const lh_ui_decl_t *row = lh_addr_of(decls[index]);
            lh_byte_t *slot = storage + used;
            lh_ui_entity_t *entity;

            /* A scrollbar is built around the container it scrolls, and
               ::lh_ui_scrollbar_init will not take a null one. The row it names is
               lower, so its node already exists: asked here, before the widget is
               touched at all, and not after something has been written into it. */
            if (row->kind == lh_ui_decl_kind_scrollbar &&
                lh_null_eq(lh_ui_entity_as_container(nodes[row->as.scrollbar.box])))
            {
                lh_ui_decl_fault(fault, lh_ui_decl_error_box, index);
                return lh_bool_false;
            }
            entity = lh_ui_decl_place_row(row, slot, nodes);

            if (lh_null_ne(row->style) && row->kind != lh_ui_decl_kind_button &&
                row->kind != lh_ui_decl_kind_toggle)
            {
                /* A button and a toggle aim the entity at their style themselves,
                   so a press composes on top of whichever of the two is showing. */
                lh_ui_entity_set_style(entity, row->style);
            }
            if (lh_null_ne(row->place))
            {
                lh_ui_entity_set_place(entity, row->place);
            }
            /* A flow belongs to a container, and the question "is this one?" is the
               container's own answer: a button, a toggle and a caption are
               containers, and a plain entity or a picture is not. One call, no table
               of kinds. */
            if (lh_null_ne(row->layout))
            {
                lh_ui_container_t *box = lh_ui_entity_as_container(entity);

                if (lh_null_ne(box))
                {
                    lh_ui_container_set_layout(box, row->layout);
                }
            }
            nodes[index] = entity;
            used += lh_ui_decl_size_of(row->kind);
        }
    }

    for (index = 0; index < count; ++index)
    {
        const lh_ui_decl_t *row = lh_addr_of(decls[index]);

        if (row->parent == LH_UI_DECL_ROOT)
        {
            tree = nodes[index];
        }
        else
        {
            lh_ui_entity_add_child(nodes[row->parent], nodes[index]);
        }
    }
    *root = tree;
    return lh_bool_true;
}
