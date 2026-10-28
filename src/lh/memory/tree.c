#include <lh/memory/tree.h>
#include <lh/assert.h>
#include <lh/memory/tree/header.h>
#include <lh/math.h>
#include <lh/util/return.h>
#include <lh/util/interval.h>

lh_usize_t
lh_memory_tree_get_block_size(lh_usize_t size)
{
    lh_runtime_check_if(
        lh_interval_closed_is_add_overflow(size, LH_MEMORY_TREE_HEADER_SIZE, 0, LH_USIZE_T_MAX),
        lh_runtime_error_code_overflow);
    return lh_math_add(size, LH_MEMORY_TREE_HEADER_SIZE);
}

lh_ptr
lh_memory_tree_alloc(lh_memory_sized_allocator_t *allocator, lh_usize_t size)
{
    return lh_memory_tree_header_get_data(lh_memory_tree_header_new(allocator, lh_null, size));
}

lh_ptr
lh_memory_tree_alloc_child(lh_ptr parent, lh_usize_t size)
{
    lh_assert_runtime_ref(parent);
    lh_memory_tree_header_t *const parent_header = lh_memory_tree_header_of(parent);
    return lh_memory_tree_header_get_data(lh_memory_tree_header_new(
        lh_memory_tree_header_get_allocator(parent_header), parent_header, size));
}

lh_void
lh_memory_tree_free(lh_ptr ptr)
{
    lh_return_ifn(ptr);

    lh_memory_tree_header_t *const top = lh_memory_tree_header_of(ptr);
    lh_list_node_unlink(lh_memory_tree_header_get_sibling(top));

    /* Depth-first without recursion, so a deep tree cannot exhaust the stack.
     * Entering a block runs its destructor (once: it is cleared), then the
     * walk descends to its first child; a block with no children left is
     * freed and the walk goes back up to its parent. */
    lh_memory_tree_header_t *header = top;
    for (;;)
    {
        lh_memory_tree_destructor_cb destructor = lh_memory_tree_header_get_destructor(header);
        if (lh_ptr_is_set(destructor))
        {
            lh_memory_tree_header_set_destructor(header, lh_null);
            destructor(lh_memory_tree_header_get_data(header));
        }

        lh_memory_tree_header_t *child = lh_memory_tree_header_get_first_child(header);
        if (lh_ptr_is_set(child))
        {
            header = child;
            continue;
        }

        lh_memory_tree_header_t *const parent =
            header == top ? lh_null : lh_memory_tree_header_get_parent(header);
        lh_list_node_unlink(lh_memory_tree_header_get_sibling(header));
        lh_memory_sized_allocator_dealloc(lh_memory_tree_header_get_allocator(header), header);
        lh_return_ifn(parent);
        header = parent;
    }
}

lh_usize_t
lh_memory_tree_get_size(const lh_ptr ptr)
{
    lh_assert_runtime_ref(ptr);
    return lh_math_sub(lh_memory_sized_allocator_get_size(lh_memory_tree_header_of(ptr)),
                       LH_MEMORY_TREE_HEADER_SIZE);
}

lh_void
lh_memory_tree_set_parent(lh_ptr ptr, lh_ptr parent)
{
    lh_assert_runtime_ref(ptr);
    lh_memory_tree_header_t *const header = lh_memory_tree_header_of(ptr);
    lh_memory_tree_header_t *const parent_header =
        lh_ptr_is_set(parent) ? lh_memory_tree_header_of(parent) : lh_null;

    for (const lh_memory_tree_header_t *up = parent_header; lh_ptr_is_set(up);
         up = lh_memory_tree_header_get_parent(up))
    {
        lh_assert_runtime_if(up == header, lh_runtime_error_code_invalid_argument);
    }

    lh_list_node_unlink(lh_memory_tree_header_get_sibling(header));
    lh_memory_tree_header_link(header, parent_header);
}

lh_ptr
lh_memory_tree_get_parent(const lh_ptr ptr)
{
    lh_assert_runtime_ref(ptr);
    const lh_memory_tree_header_t *const parent =
        lh_memory_tree_header_get_parent(lh_memory_tree_header_of(ptr));
    return lh_ptr_is_set(parent) ? lh_memory_tree_header_get_data(parent) : lh_null;
}

lh_ptr
lh_memory_tree_get_first_child(const lh_ptr ptr)
{
    lh_assert_runtime_ref(ptr);
    const lh_memory_tree_header_t *const child =
        lh_memory_tree_header_get_first_child(lh_memory_tree_header_of(ptr));
    return lh_ptr_is_set(child) ? lh_memory_tree_header_get_data(child) : lh_null;
}

lh_ptr
lh_memory_tree_get_next_sibling(const lh_ptr ptr)
{
    lh_assert_runtime_ref(ptr);
    lh_memory_tree_header_t *const header = lh_memory_tree_header_of(ptr);
    lh_memory_tree_header_t *const parent = lh_memory_tree_header_get_parent(header);
    lh_return_ifn(parent, lh_null);
    const lh_memory_tree_header_t *const next = lh_memory_tree_header_of_sibling(
        lh_list_get_next(lh_memory_tree_header_get_children(parent),
                         lh_memory_tree_header_get_sibling(header)));
    return lh_ptr_is_set(next) ? lh_memory_tree_header_get_data(next) : lh_null;
}

lh_void
lh_memory_tree_set_destructor(lh_ptr ptr, lh_memory_tree_destructor_cb destructor)
{
    lh_assert_runtime_ref(ptr);
    lh_memory_tree_header_set_destructor(lh_memory_tree_header_of(ptr), destructor);
}

lh_ptr
lh_memory_tree_realloc(lh_ptr ptr, lh_usize_t new_size)
{
    lh_assert_runtime_ref(ptr);
    const lh_usize_t block_size = lh_memory_tree_get_block_size(new_size);

    /* The links point at the header's current address, from both sides: take
     * the block out of its parent's list and its children out of its own list
     * before it may move, and put both back into the moved header after. */
    lh_memory_tree_header_t *header = lh_memory_tree_header_of(ptr);
    lh_list_node_t *const sibling = lh_memory_tree_header_get_sibling(header);
    lh_list_node_t *const prev =
        lh_list_node_is_linked(sibling) ? lh_list_node_get_prev(sibling) : lh_null;
    lh_list_t children;
    lh_list_init(lh_addr_of(children));
    lh_list_splice_back(lh_addr_of(children), lh_memory_tree_header_get_children(header));
    lh_list_node_unlink(sibling);

    header = lh_ptr_cast(lh_memory_tree_header_t,
                         lh_memory_sized_allocator_realloc(
                             lh_memory_tree_header_get_allocator(header), header, block_size));

    lh_list_node_init(lh_memory_tree_header_get_sibling(header));
    if (lh_ptr_is_set(prev))
    {
        lh_list_node_insert_after(lh_memory_tree_header_get_sibling(header), prev);
    }
    lh_list_t *const own = lh_memory_tree_header_get_children(header);
    lh_list_init(own);
    lh_list_splice_back(own, lh_addr_of(children));
    for (lh_list_node_t *node = lh_list_get_first(own); lh_ptr_is_set(node);
         node = lh_list_get_next(own, node))
    {
        lh_memory_tree_header_set_parent(lh_memory_tree_header_of_sibling(node), header);
    }
    return lh_memory_tree_header_get_data(header);
}

lh_ptr
lh_memory_tree_allocator_alloc(lh_self_ptr self, lh_usize_t size)
{
    return lh_memory_tree_alloc_child(self, size);
}

lh_void
lh_memory_tree_allocator_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    (void)self;
    lh_memory_tree_free(ptr);
}

lh_ptr
lh_memory_tree_allocator_realloc(lh_self_ptr self, lh_ptr ptr, lh_usize_t old_size,
                                 lh_usize_t new_size)
{
    (void)self;
    (void)old_size;
    return lh_memory_tree_realloc(ptr, new_size);
}

lh_void
lh_memory_tree_allocator_init(lh_memory_allocator_t *self, lh_ptr root)
{
    lh_assert_runtime_ref(root);
    lh_memory_allocator_init(self, lh_memory_tree_allocator_alloc,
                             lh_memory_tree_allocator_dealloc);
    lh_memory_allocator_set_realloc_cb(self, lh_memory_tree_allocator_realloc);
    lh_memory_allocator_set_context(self, root);
}
