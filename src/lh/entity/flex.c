#include <lh/entity/flex.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/entity/event.h>
#include <lh/null.h>
#include <lh/runtime/allocator.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

struct lh_entity_flex_node
{
    struct lh_entity_flex_node *next;
    lh_entity_t *entity;
    lh_int_t direction;
    lh_int_t wrap;
    lh_int_t justify;
    lh_int_t align_items;
    lh_int_t align_content;
    lh_int_t row_gap;
    lh_int_t column_gap;
    lh_int_t pad_top;
    lh_int_t pad_right;
    lh_int_t pad_bottom;
    lh_int_t pad_left;
    lh_bool_t on;
    lh_bool_t hug_width;
    lh_bool_t hug_height;
    lh_int_t grow;
    lh_int_t shrink;
    lh_int_t basis;
    lh_int_t align_self;
    lh_int_t order;
};

struct lh_entity_flex_item
{
    lh_entity_t *entity;
    lh_int_t base;
    lh_int_t main;
    lh_int_t cross;
    lh_bool_t cross_auto;
    lh_int_t grow;
    lh_int_t shrink;
    lh_int_t align;
    lh_int_t order;
    lh_int_t index;
    lh_int_t line;
    lh_int_t main_pos;
    lh_int_t cross_pos;
};

struct lh_entity_flex_line
{
    lh_int_t first;
    lh_int_t count;
    lh_int_t cross;
    lh_int_t pos;
};

static struct lh_entity_flex_node *lh_entity_flex_nodes;
static lh_int_t lh_entity_flex_placing;

lh_int_t
lh_entity_flex_px(lh_float_t value)
{
    if (value <= 0.0f)
    {
        return 0;
    }
    return (lh_int_t)(value + 0.5f);
}

lh_int_t
lh_entity_flex_span(const lh_entity_2d_t *box, lh_bool_t horizontal)
{
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    const lh_float_t value =
        horizontal != lh_bool_false ? lh_math_vec2_get_x(lh_addr_of(size)) : lh_math_vec2_get_y(lh_addr_of(size));
    return lh_entity_flex_px(value);
}

lh_bool_t
lh_entity_flex_horizontal(lh_int_t direction)
{
    return direction == LH_ENTITY_FLEX_ROW || direction == LH_ENTITY_FLEX_ROW_REVERSE ? lh_bool_true
                                                                                      : lh_bool_false;
}

struct lh_entity_flex_node *
lh_entity_flex_find(const lh_entity_t *entity)
{
    struct lh_entity_flex_node *node;
    for (node = lh_entity_flex_nodes; lh_ptr_is_set(node); node = node->next)
    {
        if (node->entity == entity)
        {
            return node;
        }
    }
    return lh_null;
}

lh_void
lh_entity_flex_on_delete(lh_entity_event_t *event, lh_ptr user)
{
    struct lh_entity_flex_node *const node = lh_ptr_rcast(struct lh_entity_flex_node, user);
    struct lh_entity_flex_node **link;
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_DELETE || lh_ptr_is_null(node))
    {
        return;
    }
    link = lh_addr_of(lh_entity_flex_nodes);
    while (lh_ptr_is_set(*link))
    {
        if (*link == node)
        {
            *link = node->next;
            break;
        }
        link = lh_addr_of((*link)->next);
    }
    lh_runtime_allocator_free(node);
}

struct lh_entity_flex_node *
lh_entity_flex_node(lh_entity_t *entity)
{
    struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(entity);
    node = lh_entity_flex_find(entity);
    if (lh_ptr_is_set(node))
    {
        return node;
    }
    node = lh_ptr_rcast(struct lh_entity_flex_node,
                        lh_runtime_allocator_alloc(sizeof(struct lh_entity_flex_node)));
    if (lh_ptr_is_null(node))
    {
        return lh_null;
    }
    node->next = lh_entity_flex_nodes;
    node->entity = entity;
    node->direction = LH_ENTITY_FLEX_ROW;
    node->wrap = LH_ENTITY_FLEX_NOWRAP;
    node->justify = LH_ENTITY_FLEX_START;
    node->align_items = LH_ENTITY_FLEX_STRETCH;
    node->align_content = LH_ENTITY_FLEX_STRETCH;
    node->row_gap = 0;
    node->column_gap = 0;
    node->pad_top = 0;
    node->pad_right = 0;
    node->pad_bottom = 0;
    node->pad_left = 0;
    node->on = lh_bool_false;
    node->hug_width = lh_bool_true;
    node->hug_height = lh_bool_true;
    node->grow = 0;
    node->shrink = 1;
    node->basis = -1;
    node->align_self = LH_ENTITY_FLEX_AUTO;
    node->order = 0;
    lh_entity_flex_nodes = node;
    lh_entity_add_handler(entity, lh_entity_flex_on_delete, node);
    return node;
}

lh_int_t
lh_entity_flex_clamp_gap(lh_int_t gap)
{
    return gap < 0 ? 0 : gap;
}

lh_void
lh_entity_flex_note_size(const lh_entity_2d_t *self)
{
    struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    if (lh_entity_flex_placing > 0)
    {
        return;
    }
    node = lh_entity_flex_find(lh_ptr_rcast(const lh_entity_t, self));
    if (lh_ptr_is_null(node))
    {
        return;
    }
    node->hug_width = lh_entity_flex_span(self, lh_bool_true) <= 0 ? lh_bool_true : lh_bool_false;
    node->hug_height = lh_entity_flex_span(self, lh_bool_false) <= 0 ? lh_bool_true : lh_bool_false;
}

lh_void
lh_entity_flex_sort(struct lh_entity_flex_item *items, lh_int_t count)
{
    lh_int_t i;
    for (i = 1; i < count; ++i)
    {
        const struct lh_entity_flex_item slot = items[i];
        lh_int_t j = i;
        while (j > 0 && (items[j - 1].order > slot.order ||
                         (items[j - 1].order == slot.order && items[j - 1].index > slot.index)))
        {
            items[j] = items[j - 1];
            j -= 1;
        }
        items[j] = slot;
    }
}

lh_int_t
lh_entity_flex_child_span(lh_entity_t *child, lh_bool_t horizontal, lh_int_t basis);

lh_int_t
lh_entity_flex_content(const struct lh_entity_flex_node *node, lh_bool_t horizontal)
{
    const lh_bool_t row = lh_entity_flex_horizontal(node->direction);
    const lh_bool_t main = horizontal == row ? lh_bool_true : lh_bool_false;
    const lh_int_t gap = row != lh_bool_false ? node->column_gap : node->row_gap;
    lh_int_t sum = 0;
    lh_int_t max_cross = 0;
    lh_int_t count = 0;
    lh_entity_foreach_child(child, node->entity)
    {
        lh_entity_2d_t *box;
        struct lh_entity_flex_node *item;
        lh_int_t span;
        if (lh_entity_has_flags(child, lh_entity_flags_hidden))
        {
            continue;
        }
        box = lh_entity_cast(child, lh_addr_of(lh_entity_2d_class));
        if (lh_ptr_is_null(box))
        {
            continue;
        }
        item = lh_entity_flex_find(child);
        if (main != lh_bool_false)
        {
            const lh_int_t basis = lh_ptr_is_set(item) ? item->basis : -1;
            span = lh_entity_flex_child_span(child, horizontal, basis);
            if (count > 0)
            {
                sum += gap;
            }
            sum += span;
            count += 1;
        }
        else
        {
            span = lh_entity_flex_child_span(child, horizontal, -1);
            if (span > max_cross)
            {
                max_cross = span;
            }
        }
    }
    if (horizontal != lh_bool_false)
    {
        return node->pad_left + node->pad_right + (main != lh_bool_false ? sum : max_cross);
    }
    return node->pad_top + node->pad_bottom + (main != lh_bool_false ? sum : max_cross);
}

lh_int_t
lh_entity_flex_child_span(lh_entity_t *child, lh_bool_t horizontal, lh_int_t basis)
{
    const lh_entity_2d_t *const box = lh_entity_cast(child, lh_addr_of(lh_entity_2d_class));
    struct lh_entity_flex_node *node;
    lh_int_t span;
    if (basis >= 0)
    {
        return basis;
    }
    if (lh_ptr_is_null(box))
    {
        return 0;
    }
    node = lh_entity_flex_find(child);
    span = lh_entity_flex_span(box, horizontal);
    if (lh_ptr_is_set(node) && node->on)
    {
        const lh_bool_t hug = horizontal != lh_bool_false ? node->hug_width : node->hug_height;
        if (hug != lh_bool_false || span <= 0)
        {
            return lh_entity_flex_content(node, horizontal);
        }
    }
    return span;
}

lh_int_t
lh_entity_flex_gather(struct lh_entity_flex_node *node, struct lh_entity_flex_item *items)
{
    const lh_bool_t row = lh_entity_flex_horizontal(node->direction);
    lh_int_t count = 0;
    lh_int_t index = 0;
    lh_entity_foreach_child(child, node->entity)
    {
        struct lh_entity_flex_item *item;
        struct lh_entity_flex_node *props;
        const lh_entity_2d_t *box;
        if (count >= LH_ENTITY_FLEX_LIMIT)
        {
            break;
        }
        if (lh_entity_has_flags(child, lh_entity_flags_hidden))
        {
            continue;
        }
        box = lh_entity_cast(child, lh_addr_of(lh_entity_2d_class));
        if (lh_ptr_is_null(box))
        {
            continue;
        }
        props = lh_entity_flex_find(child);
        item = lh_addr_of(items[count]);
        item->entity = child;
        item->grow = lh_ptr_is_set(props) ? props->grow : 0;
        item->shrink = lh_ptr_is_set(props) ? props->shrink : 1;
        item->order = lh_ptr_is_set(props) ? props->order : 0;
        item->align = lh_ptr_is_set(props) ? props->align_self : LH_ENTITY_FLEX_AUTO;
        item->index = index;
        item->line = 0;
        item->main_pos = 0;
        item->cross_pos = 0;
        item->base = lh_entity_flex_child_span(child, row, lh_ptr_is_set(props) ? props->basis : -1);
        item->cross = lh_entity_flex_child_span(child, row != lh_bool_false ? lh_bool_false : lh_bool_true, -1);
        item->cross_auto = lh_bool_false;
        if (lh_ptr_is_set(props) && props->on)
        {
            const lh_bool_t hug = row != lh_bool_false ? props->hug_height : props->hug_width;
            if (hug != lh_bool_false)
            {
                item->cross_auto = lh_bool_true;
            }
        }
        else if (item->cross <= 0)
        {
            item->cross_auto = lh_bool_true;
        }
        item->main = item->base;
        index += 1;
        count += 1;
    }
    lh_entity_flex_sort(items, count);
    return count;
}

lh_void
lh_entity_flex_distribute(const lh_int_t *weight, lh_int_t count, lh_int_t space, lh_int_t *share)
{
    lh_int_t total = 0;
    lh_int_t used = 0;
    lh_int_t i;
    for (i = 0; i < count; ++i)
    {
        share[i] = 0;
        if (weight[i] > 0)
        {
            total += weight[i];
        }
    }
    if (total <= 0 || space == 0)
    {
        return;
    }
    for (i = 0; i < count; ++i)
    {
        if (weight[i] > 0)
        {
            share[i] = (lh_int_t)((lh_sllong_t)space * (lh_sllong_t)weight[i] / (lh_sllong_t)total);
            used += share[i];
        }
    }
    space -= used;
    i = 0;
    while (space != 0 && i < count)
    {
        if (weight[i] > 0)
        {
            if (space > 0)
            {
                share[i] += 1;
                space -= 1;
            }
            else
            {
                share[i] -= 1;
                space += 1;
            }
        }
        i += 1;
    }
}

lh_int_t
lh_entity_flex_break(struct lh_entity_flex_item *items, lh_int_t count, lh_int_t inner, lh_int_t gap,
                     lh_int_t wrap, struct lh_entity_flex_line *lines)
{
    lh_int_t line = 0;
    lh_int_t used = 0;
    lh_int_t on = 0;
    lh_int_t i;
    if (count <= 0)
    {
        return 0;
    }
    lines[0].first = 0;
    lines[0].count = 0;
    lines[0].cross = 0;
    lines[0].pos = 0;
    for (i = 0; i < count; ++i)
    {
        const lh_int_t gap_before = on > 0 ? gap : 0;
        if (wrap != LH_ENTITY_FLEX_NOWRAP && on > 0 && used + gap_before + items[i].base > inner &&
            line + 1 < LH_ENTITY_FLEX_LIMIT)
        {
            line += 1;
            lines[line].first = i;
            lines[line].count = 0;
            lines[line].cross = 0;
            lines[line].pos = 0;
            used = 0;
            on = 0;
        }
        items[i].line = line;
        lines[line].count += 1;
        used += (on > 0 ? gap : 0) + items[i].base;
        on += 1;
    }
    return line + 1;
}

lh_void
lh_entity_flex_resolve(struct lh_entity_flex_item *items, const struct lh_entity_flex_line *line,
                       lh_int_t inner, lh_int_t gap)
{
    lh_int_t weight[LH_ENTITY_FLEX_LIMIT];
    lh_int_t share[LH_ENTITY_FLEX_LIMIT];
    lh_int_t sum = 0;
    lh_int_t i;
    lh_int_t space;
    for (i = 0; i < line->count; ++i)
    {
        sum += items[line->first + i].base;
    }
    if (line->count > 1)
    {
        sum += (line->count - 1) * gap;
    }
    space = inner - sum;
    if (space > 0)
    {
        for (i = 0; i < line->count; ++i)
        {
            weight[i] = items[line->first + i].grow;
        }
        lh_entity_flex_distribute(weight, line->count, space, share);
    }
    else if (space < 0)
    {
        for (i = 0; i < line->count; ++i)
        {
            const struct lh_entity_flex_item *const item = lh_addr_of(items[line->first + i]);
            weight[i] = item->shrink > 0 && item->base > 0 ? item->shrink * item->base : 0;
        }
        lh_entity_flex_distribute(weight, line->count, space, share);
    }
    else
    {
        for (i = 0; i < line->count; ++i)
        {
            share[i] = 0;
        }
    }
    for (i = 0; i < line->count; ++i)
    {
        struct lh_entity_flex_item *const item = lh_addr_of(items[line->first + i]);
        item->main = item->base + share[i];
        if (item->main < 0)
        {
            item->main = 0;
        }
    }
}

lh_int_t
lh_entity_flex_lead(lh_int_t free, lh_int_t count, lh_int_t gap, lh_int_t mode, lh_int_t *between)
{
    *between = gap;
    if (count <= 0 || free <= 0)
    {
        return 0;
    }
    if (mode == LH_ENTITY_FLEX_END)
    {
        return free;
    }
    if (mode == LH_ENTITY_FLEX_CENTER)
    {
        return free / 2;
    }
    if (mode == LH_ENTITY_FLEX_SPACE_BETWEEN)
    {
        if (count > 1)
        {
            *between = gap + free / (count - 1);
        }
        return 0;
    }
    if (mode == LH_ENTITY_FLEX_SPACE_AROUND)
    {
        *between = gap + free / count;
        return free / (2 * count);
    }
    if (mode == LH_ENTITY_FLEX_SPACE_EVENLY)
    {
        const lh_int_t slot = free / (count + 1);
        *between = gap + slot;
        return slot;
    }
    return 0;
}

lh_void
lh_entity_flex_write(lh_entity_t *entity, lh_int_t x, lh_int_t y, lh_int_t width, lh_int_t height)
{
    lh_entity_2d_t *const box = lh_entity_cast(entity, lh_addr_of(lh_entity_2d_class));
    lh_math_vec2_t position;
    lh_math_vec2_t size;
    if (lh_ptr_is_null(box))
    {
        return;
    }
    position = lh_entity_2d_get_position(box);
    size = lh_entity_2d_get_size(box);
    if (lh_math_vec2_get_x(lh_addr_of(position)) != (lh_float_t)x ||
        lh_math_vec2_get_y(lh_addr_of(position)) != (lh_float_t)y)
    {
        lh_entity_2d_set_position(box, lh_math_vec2_make((lh_float_t)x, (lh_float_t)y));
    }
    if (lh_math_vec2_get_x(lh_addr_of(size)) != (lh_float_t)width ||
        lh_math_vec2_get_y(lh_addr_of(size)) != (lh_float_t)height)
    {
        lh_entity_2d_set_size(box, lh_math_vec2_make((lh_float_t)width, (lh_float_t)height));
    }
}

lh_void
lh_entity_flex_set_on(lh_entity_t *self, lh_bool_t on)
{
    struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_node(self);
    if (lh_ptr_is_null(node))
    {
        return;
    }
    node->on = on != lh_bool_false ? lh_bool_true : lh_bool_false;
    if (lh_ptr_is_set(lh_entity_cast(self, lh_addr_of(lh_entity_2d_class))))
    {
        lh_entity_flex_note_size(lh_entity_cast(self, lh_addr_of(lh_entity_2d_class)));
    }
}

lh_bool_t
lh_entity_flex_get_on(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) && node->on != lh_bool_false ? lh_bool_true : lh_bool_false;
}

lh_void
lh_entity_flex_set_direction(lh_entity_t *self, lh_int_t direction)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(node))
    {
        return;
    }
    if (direction < LH_ENTITY_FLEX_ROW || direction > LH_ENTITY_FLEX_COLUMN_REVERSE)
    {
        direction = LH_ENTITY_FLEX_ROW;
    }
    node->direction = direction;
}

lh_int_t
lh_entity_flex_get_direction(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->direction : LH_ENTITY_FLEX_ROW;
}

lh_void
lh_entity_flex_set_wrap(lh_entity_t *self, lh_int_t wrap)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(node))
    {
        return;
    }
    if (wrap < LH_ENTITY_FLEX_NOWRAP || wrap > LH_ENTITY_FLEX_WRAP_REVERSE)
    {
        wrap = LH_ENTITY_FLEX_NOWRAP;
    }
    node->wrap = wrap;
}

lh_int_t
lh_entity_flex_get_wrap(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->wrap : LH_ENTITY_FLEX_NOWRAP;
}

lh_void
lh_entity_flex_set_justify(lh_entity_t *self, lh_int_t justify)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(node))
    {
        return;
    }
    if (justify < LH_ENTITY_FLEX_START || justify > LH_ENTITY_FLEX_SPACE_EVENLY)
    {
        justify = LH_ENTITY_FLEX_START;
    }
    node->justify = justify;
}

lh_int_t
lh_entity_flex_get_justify(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->justify : LH_ENTITY_FLEX_START;
}

lh_void
lh_entity_flex_set_align(lh_entity_t *self, lh_int_t align)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(node))
    {
        return;
    }
    if (align != LH_ENTITY_FLEX_START && align != LH_ENTITY_FLEX_END && align != LH_ENTITY_FLEX_CENTER &&
        align != LH_ENTITY_FLEX_STRETCH)
    {
        align = LH_ENTITY_FLEX_STRETCH;
    }
    node->align_items = align;
}

lh_int_t
lh_entity_flex_get_align(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->align_items : LH_ENTITY_FLEX_STRETCH;
}

lh_void
lh_entity_flex_set_content(lh_entity_t *self, lh_int_t align)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(node))
    {
        return;
    }
    if (align < LH_ENTITY_FLEX_START || align > LH_ENTITY_FLEX_STRETCH || align == LH_ENTITY_FLEX_AUTO)
    {
        align = LH_ENTITY_FLEX_STRETCH;
    }
    node->align_content = align;
}

lh_int_t
lh_entity_flex_get_content(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->align_content : LH_ENTITY_FLEX_STRETCH;
}

lh_void
lh_entity_flex_set_gap(lh_entity_t *self, lh_int_t gap)
{
    lh_entity_flex_set_gaps(self, gap, gap);
}

lh_void
lh_entity_flex_set_gaps(lh_entity_t *self, lh_int_t row_gap, lh_int_t column_gap)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(node))
    {
        return;
    }
    node->row_gap = lh_entity_flex_clamp_gap(row_gap);
    node->column_gap = lh_entity_flex_clamp_gap(column_gap);
}

lh_int_t
lh_entity_flex_get_row_gap(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->row_gap : 0;
}

lh_int_t
lh_entity_flex_get_column_gap(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->column_gap : 0;
}

lh_void
lh_entity_flex_set_pad(lh_entity_t *self, lh_int_t pad)
{
    lh_entity_flex_set_padding(self, pad, pad, pad, pad);
}

lh_void
lh_entity_flex_set_padding(lh_entity_t *self, lh_int_t top, lh_int_t right, lh_int_t bottom,
                           lh_int_t left)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(node))
    {
        return;
    }
    node->pad_top = lh_entity_flex_clamp_gap(top);
    node->pad_right = lh_entity_flex_clamp_gap(right);
    node->pad_bottom = lh_entity_flex_clamp_gap(bottom);
    node->pad_left = lh_entity_flex_clamp_gap(left);
}

lh_void
lh_entity_flex_item_set_grow(lh_entity_t *self, lh_int_t grow)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_set(node))
    {
        node->grow = grow < 0 ? 0 : grow;
    }
}

lh_int_t
lh_entity_flex_item_get_grow(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->grow : 0;
}

lh_void
lh_entity_flex_item_set_shrink(lh_entity_t *self, lh_int_t shrink)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_set(node))
    {
        node->shrink = shrink < 0 ? 0 : shrink;
    }
}

lh_int_t
lh_entity_flex_item_get_shrink(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->shrink : 1;
}

lh_void
lh_entity_flex_item_set_basis(lh_entity_t *self, lh_int_t basis)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_set(node))
    {
        node->basis = basis < 0 ? -1 : basis;
    }
}

lh_int_t
lh_entity_flex_item_get_basis(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->basis : -1;
}

lh_void
lh_entity_flex_item_set_align(lh_entity_t *self, lh_int_t align)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(node))
    {
        return;
    }
    if (align != LH_ENTITY_FLEX_AUTO && align != LH_ENTITY_FLEX_START && align != LH_ENTITY_FLEX_END &&
        align != LH_ENTITY_FLEX_CENTER && align != LH_ENTITY_FLEX_STRETCH)
    {
        align = LH_ENTITY_FLEX_AUTO;
    }
    node->align_self = align;
}

lh_int_t
lh_entity_flex_item_get_align(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->align_self : LH_ENTITY_FLEX_AUTO;
}

lh_void
lh_entity_flex_item_set_order(lh_entity_t *self, lh_int_t order)
{
    struct lh_entity_flex_node *const node = lh_entity_flex_node(self);
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_set(node))
    {
        node->order = order;
    }
}

lh_int_t
lh_entity_flex_item_get_order(const lh_entity_t *self)
{
    const struct lh_entity_flex_node *node;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    return lh_ptr_is_set(node) ? node->order : 0;
}

lh_void
lh_entity_flex_layout(lh_entity_t *self)
{
    struct lh_entity_flex_node *node;
    struct lh_entity_flex_item items[LH_ENTITY_FLEX_LIMIT];
    struct lh_entity_flex_line lines[LH_ENTITY_FLEX_LIMIT];
    lh_int_t weight[LH_ENTITY_FLEX_LIMIT];
    lh_int_t share[LH_ENTITY_FLEX_LIMIT];
    const lh_entity_2d_t *box;
    lh_entity_t *parent;
    struct lh_entity_flex_node *parent_node;
    lh_bool_t row;
    lh_bool_t parent_flex;
    lh_int_t count;
    lh_int_t line_count;
    lh_int_t width;
    lh_int_t height;
    lh_int_t inner_main;
    lh_int_t inner_cross;
    lh_int_t main_gap;
    lh_int_t cross_gap;
    lh_int_t i;
    lh_assert_runtime_ref(self);
    node = lh_entity_flex_find(self);
    if (lh_ptr_is_null(node) || node->on == lh_bool_false)
    {
        return;
    }
    box = lh_entity_cast(self, lh_addr_of(lh_entity_2d_class));
    if (lh_ptr_is_null(box))
    {
        return;
    }
    lh_entity_flex_placing += 1;
    row = lh_entity_flex_horizontal(node->direction);
    parent = lh_entity_get_parent(self);
    parent_node = lh_ptr_is_set(parent) ? lh_entity_flex_find(parent) : lh_null;
    parent_flex = lh_ptr_is_set(parent_node) && parent_node->on != lh_bool_false ? lh_bool_true
                                                                                 : lh_bool_false;
    width = lh_entity_flex_span(box, lh_bool_true);
    height = lh_entity_flex_span(box, lh_bool_false);
    if (node->hug_width != lh_bool_false && parent_flex == lh_bool_false)
    {
        width = lh_entity_flex_content(node, lh_bool_true);
    }
    if (node->hug_height != lh_bool_false && parent_flex == lh_bool_false)
    {
        height = lh_entity_flex_content(node, lh_bool_false);
    }
    if (width <= 0)
    {
        width = lh_entity_flex_content(node, lh_bool_true);
    }
    if (height <= 0)
    {
        height = lh_entity_flex_content(node, lh_bool_false);
    }
    {
        const lh_math_vec2_t place = lh_entity_2d_get_position(box);
        lh_entity_flex_write(self, lh_entity_flex_px(lh_math_vec2_get_x(lh_addr_of(place))),
                             lh_entity_flex_px(lh_math_vec2_get_y(lh_addr_of(place))), width, height);
    }
    count = lh_entity_flex_gather(node, items);
    main_gap = row != lh_bool_false ? node->column_gap : node->row_gap;
    cross_gap = row != lh_bool_false ? node->row_gap : node->column_gap;
    inner_main = (row != lh_bool_false ? width : height) - (row != lh_bool_false ? node->pad_left + node->pad_right
                                                                                 : node->pad_top + node->pad_bottom);
    inner_cross = (row != lh_bool_false ? height : width) -
                  (row != lh_bool_false ? node->pad_top + node->pad_bottom : node->pad_left + node->pad_right);
    if (inner_main < 0)
    {
        inner_main = 0;
    }
    if (inner_cross < 0)
    {
        inner_cross = 0;
    }
    line_count = lh_entity_flex_break(items, count, inner_main, main_gap, node->wrap, lines);
    for (i = 0; i < line_count; ++i)
    {
        lh_int_t c;
        lh_entity_flex_resolve(items, lh_addr_of(lines[i]), inner_main, main_gap);
        lines[i].cross = 0;
        for (c = 0; c < lines[i].count; ++c)
        {
            if (items[lines[i].first + c].cross > lines[i].cross)
            {
                lines[i].cross = items[lines[i].first + c].cross;
            }
        }
    }
    if (line_count == 1)
    {
        lines[0].cross = inner_cross > lines[0].cross ? inner_cross : lines[0].cross;
    }
    else if (line_count > 1)
    {
        lh_int_t sum = 0;
        lh_int_t free;
        lh_int_t between = 0;
        lh_int_t cursor;
        for (i = 0; i < line_count; ++i)
        {
            sum += lines[i].cross;
            weight[i] = 1;
        }
        if (line_count > 1)
        {
            sum += (line_count - 1) * cross_gap;
        }
        free = inner_cross - sum;
        if (node->align_content == LH_ENTITY_FLEX_STRETCH && free > 0)
        {
            lh_entity_flex_distribute(weight, line_count, free, share);
            for (i = 0; i < line_count; ++i)
            {
                lines[i].cross += share[i];
            }
            free = 0;
        }
        cursor = lh_entity_flex_lead(free, line_count, cross_gap, node->align_content, lh_addr_of(between));
        for (i = 0; i < line_count; ++i)
        {
            lines[i].pos = cursor;
            cursor += lines[i].cross + between;
        }
    }
    if (node->wrap == LH_ENTITY_FLEX_WRAP_REVERSE)
    {
        for (i = 0; i < line_count; ++i)
        {
            lines[i].pos = inner_cross - lines[i].pos - lines[i].cross;
        }
    }
    for (i = 0; i < line_count; ++i)
    {
        lh_int_t sum = 0;
        lh_int_t between = 0;
        lh_int_t cursor;
        lh_int_t c;
        const struct lh_entity_flex_line *const line = lh_addr_of(lines[i]);
        for (c = 0; c < line->count; ++c)
        {
            struct lh_entity_flex_item *const item = lh_addr_of(items[line->first + c]);
            lh_int_t align = item->align == LH_ENTITY_FLEX_AUTO ? node->align_items : item->align;
            lh_int_t extra;
            sum += item->main;
            if (align == LH_ENTITY_FLEX_STRETCH && item->cross_auto != lh_bool_false)
            {
                item->cross = line->cross;
                align = LH_ENTITY_FLEX_START;
            }
            else if (align == LH_ENTITY_FLEX_STRETCH)
            {
                align = LH_ENTITY_FLEX_START;
            }
            extra = line->cross - item->cross;
            if (extra < 0)
            {
                extra = 0;
            }
            if (align == LH_ENTITY_FLEX_END)
            {
                item->cross_pos = line->pos + extra;
            }
            else if (align == LH_ENTITY_FLEX_CENTER)
            {
                item->cross_pos = line->pos + extra / 2;
            }
            else
            {
                item->cross_pos = line->pos;
            }
        }
        if (line->count > 1)
        {
            sum += (line->count - 1) * main_gap;
        }
        cursor = lh_entity_flex_lead(inner_main - sum, line->count, main_gap, node->justify, lh_addr_of(between));
        for (c = 0; c < line->count; ++c)
        {
            items[line->first + c].main_pos = cursor;
            cursor += items[line->first + c].main + between;
        }
    }
    if (node->direction == LH_ENTITY_FLEX_ROW_REVERSE || node->direction == LH_ENTITY_FLEX_COLUMN_REVERSE)
    {
        for (i = 0; i < count; ++i)
        {
            items[i].main_pos = inner_main - items[i].main_pos - items[i].main;
        }
    }
    for (i = 0; i < count; ++i)
    {
        const struct lh_entity_flex_item *const item = lh_addr_of(items[i]);
        if (row != lh_bool_false)
        {
            lh_entity_flex_write(item->entity, node->pad_left + item->main_pos, node->pad_top + item->cross_pos,
                                 item->main, item->cross);
        }
        else
        {
            lh_entity_flex_write(item->entity, node->pad_left + item->cross_pos, node->pad_top + item->main_pos,
                                 item->cross, item->main);
        }
    }
    lh_entity_flex_placing -= 1;
}

lh_void
lh_entity_flex_layout_tree(lh_entity_t *root)
{
    if (lh_ptr_is_null(root))
    {
        return;
    }
    lh_entity_flex_layout(root);
    lh_entity_foreach_child(child, root)
    {
        lh_entity_flex_layout_tree(child);
    }
}
