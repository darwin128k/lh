#include <lh/memory/tree/header.h>
#include <lh/memory/tree.h>
#include <lh/assert.h>

lh_memory_tree_header_t *
lh_memory_tree_header_of(const lh_ptr ptr)
{
    return lh_ptr_sub_by_offset_unsafe(lh_memory_tree_header_t, ptr, LH_MEMORY_TREE_HEADER_SIZE);
}

lh_ptr
lh_memory_tree_header_get_data(const lh_memory_tree_header_t *header)
{
    return lh_ptr_add_by_offset_unsafe(lh_void, header, LH_MEMORY_TREE_HEADER_SIZE);
}

lh_memory_tree_header_t *
lh_memory_tree_header_of_sibling(const lh_list_node_t *node)
{
    return lh_list_entry(lh_memory_tree_header_t, sibling, node);
}

lh_memory_sized_allocator_t *
lh_memory_tree_header_get_allocator(const lh_memory_tree_header_t *header)
{
    lh_assert_runtime_ref(header);
    return header->allocator;
}

lh_void
lh_memory_tree_header_set_allocator(lh_memory_tree_header_t *header,
                                    lh_memory_sized_allocator_t *allocator)
{
    lh_assert_runtime_ref(header);
    header->allocator = allocator;
}

lh_memory_tree_header_t *
lh_memory_tree_header_get_parent(const lh_memory_tree_header_t *header)
{
    lh_assert_runtime_ref(header);
    return header->parent;
}

lh_void
lh_memory_tree_header_set_parent(lh_memory_tree_header_t *header, lh_memory_tree_header_t *parent)
{
    lh_assert_runtime_ref(header);
    header->parent = parent;
}

lh_list_node_t *
lh_memory_tree_header_get_sibling(lh_memory_tree_header_t *header)
{
    lh_assert_runtime_ref(header);
    return lh_addr_of(header->sibling);
}

lh_list_t *
lh_memory_tree_header_get_children(lh_memory_tree_header_t *header)
{
    lh_assert_runtime_ref(header);
    return lh_addr_of(header->children);
}

lh_memory_tree_destructor_cb
lh_memory_tree_header_get_destructor(const lh_memory_tree_header_t *header)
{
    lh_assert_runtime_ref(header);
    return header->destructor;
}

lh_void
lh_memory_tree_header_set_destructor(lh_memory_tree_header_t *header,
                                     lh_memory_tree_destructor_cb destructor)
{
    lh_assert_runtime_ref(header);
    header->destructor = destructor;
}

lh_memory_tree_header_t *
lh_memory_tree_header_get_first_child(lh_memory_tree_header_t *header)
{
    return lh_memory_tree_header_of_sibling(
        lh_list_get_first(lh_memory_tree_header_get_children(header)));
}

lh_void
lh_memory_tree_header_link(lh_memory_tree_header_t *header, lh_memory_tree_header_t *parent)
{
    lh_memory_tree_header_set_parent(header, parent);
    if (lh_ptr_is_set(parent))
    {
        lh_list_push_back(lh_memory_tree_header_get_children(parent),
                          lh_memory_tree_header_get_sibling(header));
    }
}

lh_void
lh_memory_tree_header_init(lh_memory_tree_header_t *header, lh_memory_sized_allocator_t *allocator,
                           lh_memory_tree_header_t *parent)
{
    lh_memory_tree_header_set_allocator(header, allocator);
    lh_memory_tree_header_set_destructor(header, lh_null);
    lh_list_node_init(lh_memory_tree_header_get_sibling(header));
    lh_list_init(lh_memory_tree_header_get_children(header));
    lh_memory_tree_header_link(header, parent);
}

lh_memory_tree_header_t *
lh_memory_tree_header_new(lh_memory_sized_allocator_t *allocator, lh_memory_tree_header_t *parent,
                          lh_usize_t size)
{
    lh_memory_tree_header_t *header = lh_ptr_cast(
        lh_memory_tree_header_t,
        lh_memory_sized_allocator_alloc(allocator, lh_memory_tree_get_block_size(size)));
    lh_memory_tree_header_init(header, allocator, parent);
    return header;
}
