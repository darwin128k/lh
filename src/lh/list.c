#include <lh/list.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/list/node/link.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/math.h>

LH_ATTRIBUTE_STATIC
lh_list_node_t *
lh_list_get_head(lh_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->head);
}

LH_ATTRIBUTE_STATIC
const lh_list_node_t *
lh_list_get_head_as_const(const lh_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->head);
}

/* A raw link as an element: the head is where walking stops. */
LH_ATTRIBUTE_STATIC
lh_list_node_t *
lh_list_as_element(const lh_list_t *self, lh_list_node_t *node)
{
    return lh_ptr_eq(node, lh_list_get_head_as_const(self)) ? lh_null : node;
}

void
lh_list_init(lh_list_t *self)
{
    lh_list_node_init(lh_list_get_head(self));
}

lh_bool_t
lh_list_is_empty(const lh_list_t *self)
{
    return lh_cast_static(lh_bool_t, !lh_list_node_is_linked(lh_list_get_head_as_const(self)));
}

lh_usize_t
lh_list_get_size(const lh_list_t *self)
{
    lh_usize_t size = 0U;

    for (const lh_list_node_t *node = lh_list_get_first(self); lh_ptr_is_set(node);
         node = lh_list_get_next(self, node))
    {
        size = lh_math_add_one(size);
    }
    return size;
}

lh_list_node_t *
lh_list_get_first(const lh_list_t *self)
{
    return lh_list_as_element(self, lh_list_node_get_next(lh_list_get_head_as_const(self)));
}

lh_list_node_t *
lh_list_get_last(const lh_list_t *self)
{
    return lh_list_as_element(self, lh_list_node_get_prev(lh_list_get_head_as_const(self)));
}

lh_list_node_t *
lh_list_get_at(const lh_list_t *self, lh_uindex_t index)
{
    lh_list_node_t *node = lh_list_get_first(self);

    while (lh_ptr_is_set(node) && !lh_math_is_zero(index))
    {
        node = lh_list_get_next(self, node);
        index = lh_math_sub_one(index);
    }
    return node;
}

lh_list_node_t *
lh_list_get_next(const lh_list_t *self, const lh_list_node_t *node)
{
    return lh_list_as_element(self, lh_list_node_get_next(node));
}

lh_list_node_t *
lh_list_get_prev(const lh_list_t *self, const lh_list_node_t *node)
{
    return lh_list_as_element(self, lh_list_node_get_prev(node));
}

lh_uindex_t
lh_list_index_of(const lh_list_t *self, const lh_list_node_t *node)
{
    lh_uindex_t index = 0U;

    for (const lh_list_node_t *at = lh_list_get_first(self); lh_ptr_is_set(at);
         at = lh_list_get_next(self, at))
    {
        if (lh_ptr_eq(at, node))
        {
            return index;
        }
        index = lh_math_add_one(index);
    }
    return LH_LIST_INVALID;
}

lh_bool_t
lh_list_contains(const lh_list_t *self, const lh_list_node_t *node)
{
    return lh_cast_static(lh_bool_t, lh_math_ne(lh_list_index_of(self, node), LH_LIST_INVALID));
}

void
lh_list_push_front(lh_list_t *self, lh_list_node_t *node)
{
    lh_list_node_insert_after(node, lh_list_get_head(self));
}

void
lh_list_push_back(lh_list_t *self, lh_list_node_t *node)
{
    lh_list_node_insert_before(node, lh_list_get_head(self));
}

void
lh_list_insert_sorted(lh_list_t *self, lh_list_node_t *node, lh_list_cmp_cb cmp, lh_ptr context)
{
    lh_list_node_t *pos;

    lh_assert_runtime_ref(cmp);
    for (pos = lh_list_get_first(self); lh_ptr_is_set(pos); pos = lh_list_get_next(self, pos))
    {
        if (lh_math_lt(cmp(node, pos, context), 0))
        {
            lh_list_node_insert_before(node, pos);
            return;
        }
    }
    lh_list_push_back(self, node);
}

lh_list_node_t *
lh_list_pop_front(lh_list_t *self)
{
    lh_list_node_t *const node = lh_list_get_first(self);

    if (lh_ptr_is_set(node))
    {
        lh_list_node_unlink(node);
    }
    return node;
}

lh_list_node_t *
lh_list_pop_back(lh_list_t *self)
{
    lh_list_node_t *const node = lh_list_get_last(self);

    if (lh_ptr_is_set(node))
    {
        lh_list_node_unlink(node);
    }
    return node;
}

/* Join every element of `other` between `prev` and `next`, which are
   neighbours in another list, then leave `other` empty. */
LH_ATTRIBUTE_STATIC
void
lh_list_splice_between(lh_list_t *other, lh_list_node_t *prev, lh_list_node_t *next)
{
    if (lh_list_is_empty(other))
    {
        return;
    }
    lh_list_node_t *const first = lh_list_get_first(other);
    lh_list_node_t *const last = lh_list_get_last(other);

    lh_list_node_set_prev(first, prev);
    lh_list_node_set_next(prev, first);
    lh_list_node_set_next(last, next);
    lh_list_node_set_prev(next, last);
    lh_list_init(other);
}

void
lh_list_splice_back(lh_list_t *self, lh_list_t *other)
{
    lh_assert_runtime_if(lh_ptr_eq(self, other), lh_runtime_error_code_invalid_argument);
    lh_list_node_t *const head = lh_list_get_head(self);

    lh_list_splice_between(other, lh_list_node_get_prev(head), head);
}

void
lh_list_splice_front(lh_list_t *self, lh_list_t *other)
{
    lh_assert_runtime_if(lh_ptr_eq(self, other), lh_runtime_error_code_invalid_argument);
    lh_list_node_t *const head = lh_list_get_head(self);

    lh_list_splice_between(other, head, lh_list_node_get_next(head));
}

void
lh_list_clear(lh_list_t *self)
{
    while (lh_ptr_is_set(lh_list_pop_front(self)))
    {
    }
}

/*
 * Bottom-up merge sort (Simon Tatham's, the one the Linux kernel's list_sort
 * is built on). The ring is opened into a null-terminated chain through
 * `next`; runs of 1, 2, 4, … elements are merged pairwise until one run is
 * left; then the chain is closed back into a ring and every `prev` rebuilt.
 * Taking from the left run on a tie keeps equal elements in order.
 */
void
lh_list_sort(lh_list_t *self, lh_list_cmp_cb cmp, lh_ptr context)
{
    lh_assert_runtime_ref(cmp);
    lh_list_node_t *const head = lh_list_get_head(self);
    lh_list_node_t *chain = lh_list_get_first(self);

    if (lh_ptr_is_null(chain))
    {
        return;
    }
    lh_list_node_set_next(lh_list_node_get_prev(head), lh_null);

    for (lh_usize_t run = 1U;; run = lh_math_mul(run, 2U))
    {
        lh_list_node_t *left = chain;
        lh_list_node_t *tail = lh_null;
        lh_usize_t merges = 0U;

        chain = lh_null;
        while (lh_ptr_is_set(left))
        {
            lh_list_node_t *right = left;
            lh_usize_t left_size = 0U;
            lh_usize_t right_size = run;

            merges = lh_math_add_one(merges);
            while (lh_math_lt(left_size, run) && lh_ptr_is_set(right))
            {
                left_size = lh_math_add_one(left_size);
                right = lh_list_node_get_next(right);
            }
            while (lh_math_gt(left_size, 0U) ||
                   (lh_math_gt(right_size, 0U) && lh_ptr_is_set(right)))
            {
                lh_list_node_t *take;

                if (lh_math_is_zero(left_size) ||
                    (lh_math_gt(right_size, 0U) && lh_ptr_is_set(right) &&
                     lh_math_gt(cmp(left, right, context), 0)))
                {
                    take = right;
                    right = lh_list_node_get_next(right);
                    right_size = lh_math_sub_one(right_size);
                }
                else
                {
                    take = left;
                    left = lh_list_node_get_next(left);
                    left_size = lh_math_sub_one(left_size);
                }
                if (lh_ptr_is_set(tail))
                {
                    lh_list_node_set_next(tail, take);
                }
                else
                {
                    chain = take;
                }
                tail = take;
            }
            left = right;
        }
        lh_list_node_set_next(tail, lh_null);
        if (lh_math_le(merges, 1U))
        {
            break;
        }
    }

    lh_list_node_t *prev = head;
    for (lh_list_node_t *node = chain; lh_ptr_is_set(node); node = lh_list_node_get_next(node))
    {
        lh_list_node_set_prev(node, prev);
        prev = node;
    }
    lh_list_node_set_next(head, chain);
    lh_list_node_set_next(prev, head);
    lh_list_node_set_prev(head, prev);
}
