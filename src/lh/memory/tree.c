#include <lh/memory/tree.h>
#include <lh/assert.h>
#include <lh/memory/tree/header.h>
#include <lh/null.h>
#include <lh/numeric/limits.h>
#include <lh/runtime/assert.h>
#include <lh/runtime/error/code.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* User data <-> its header. */
#define lh_memory_tree_header_of(ptr)                                                              \
    lh_ptr_sub_by_offset_unsafe(lh_memory_tree_header_t, (ptr), LH_MEMORY_TREE_HEADER_SIZE)
#define lh_memory_tree_data_of(header)                                                             \
    lh_ptr_add_by_offset_unsafe(lh_void, (header), LH_MEMORY_TREE_HEADER_SIZE)

/* The header of the child whose sibling node is @p node, or null for null. */
#define lh_memory_tree_header_of_sibling(node)                                                     \
    lh_list_entry(lh_memory_tree_header_t, sibling, (node))

/* Bytes the sized allocator is asked for: the tree header plus @p size, failing on overflow. */
static lh_usize_t
lh_memory_tree_block_size(lh_usize_t size)
{
    lh_runtime_check_if(lh_math_gt(size, lh_math_sub(LH_USIZE_T_MAX, LH_MEMORY_TREE_HEADER_SIZE)),
                        lh_runtime_error_code_overflow);
    return lh_math_add(size, LH_MEMORY_TREE_HEADER_SIZE);
}

/* A new block under @p parent (null for a root): shared by alloc and alloc_child. */
static lh_ptr
lh_memory_tree_new(lh_memory_sized_allocator_t *allocator, lh_memory_tree_header_t *parent,
                   lh_usize_t size)
{
    lh_memory_tree_header_t *header =
        lh_ptr_rcast(lh_memory_tree_header_t,
                     lh_memory_sized_allocator_alloc(allocator, lh_memory_tree_block_size(size)));
    header->allocator = allocator;
    header->parent = parent;
    header->destructor = lh_null;
    lh_list_node_init(lh_addr_of(header->sibling));
    lh_list_init(lh_addr_of(header->children));
    if (lh_ptr_is_set(parent))
    {
        lh_list_push_back(lh_addr_of(parent->children), lh_addr_of(header->sibling));
    }
    return lh_memory_tree_data_of(header);
}

lh_ptr
lh_memory_tree_alloc(lh_memory_sized_allocator_t *allocator, lh_usize_t size)
{
    lh_assert_runtime_ref(allocator);
    return lh_memory_tree_new(allocator, lh_null, size);
}

lh_ptr
lh_memory_tree_alloc_child(lh_ptr parent, lh_usize_t size)
{
    lh_assert_runtime_ref(parent);
    lh_memory_tree_header_t *parent_header = lh_memory_tree_header_of(parent);
    return lh_memory_tree_new(parent_header->allocator, parent_header, size);
}

lh_void
lh_memory_tree_free(lh_ptr ptr)
{
    lh_return_ifn(ptr);

    lh_memory_tree_header_t *const top = lh_memory_tree_header_of(ptr);
    lh_list_node_unlink(lh_addr_of(top->sibling));

    /* Depth-first without recursion, so a deep tree cannot exhaust the stack.
     * Entering a block runs its destructor (once: it is cleared), then the
     * walk descends to its first child; a block with no children left is
     * freed and the walk goes back up to its parent. */
    lh_memory_tree_header_t *header = top;
    for (;;)
    {
        lh_memory_tree_destructor_fn *destructor = header->destructor;
        if (lh_ptr_is_set(destructor))
        {
            header->destructor = lh_null;
            destructor(lh_memory_tree_data_of(header));
        }

        lh_memory_tree_header_t *child =
            lh_memory_tree_header_of_sibling(lh_list_get_first(lh_addr_of(header->children)));
        if (lh_ptr_is_set(child))
        {
            header = child;
            continue;
        }

        lh_memory_tree_header_t *const parent = header == top ? lh_null : header->parent;
        lh_list_node_unlink(lh_addr_of(header->sibling));
        lh_memory_sized_allocator_dealloc(header->allocator, header);
        lh_return_ifn(parent);
        header = parent;
    }
}

lh_ptr
lh_memory_tree_realloc(lh_ptr ptr, lh_usize_t new_size)
{
    lh_assert_runtime_ref(ptr);
    const lh_usize_t block_size = lh_memory_tree_block_size(new_size);

    /* The links point at the header's current address, from both sides: take
     * the block out of its parent's list and its children out of its own list
     * before it may move, and put both back into the moved header after. */
    lh_memory_tree_header_t *header = lh_memory_tree_header_of(ptr);
    lh_list_node_t *const prev = lh_list_node_is_linked(lh_addr_of(header->sibling))
                                     ? lh_list_node_get_prev(lh_addr_of(header->sibling))
                                     : lh_null;
    lh_list_t children;
    lh_list_init(lh_addr_of(children));
    lh_list_splice_back(lh_addr_of(children), lh_addr_of(header->children));
    lh_list_node_unlink(lh_addr_of(header->sibling));

    header = lh_ptr_rcast(lh_memory_tree_header_t,
                          lh_memory_sized_allocator_realloc(header->allocator, header, block_size));

    lh_list_node_init(lh_addr_of(header->sibling));
    if (lh_ptr_is_set(prev))
    {
        lh_list_node_insert_after(lh_addr_of(header->sibling), prev);
    }
    lh_list_init(lh_addr_of(header->children));
    lh_list_splice_back(lh_addr_of(header->children), lh_addr_of(children));
    for (lh_list_node_t *node = lh_list_get_first(lh_addr_of(header->children));
         lh_ptr_is_set(node); node = lh_list_get_next(lh_addr_of(header->children), node))
    {
        lh_memory_tree_header_of_sibling(node)->parent = header;
    }
    return lh_memory_tree_data_of(header);
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

    for (const lh_memory_tree_header_t *up = parent_header; lh_ptr_is_set(up); up = up->parent)
    {
        lh_assert_runtime_if(up == header, lh_runtime_error_code_invalid_argument);
    }

    lh_list_node_unlink(lh_addr_of(header->sibling));
    header->parent = parent_header;
    if (lh_ptr_is_set(parent_header))
    {
        lh_list_push_back(lh_addr_of(parent_header->children), lh_addr_of(header->sibling));
    }
}

lh_ptr
lh_memory_tree_get_parent(const lh_ptr ptr)
{
    lh_assert_runtime_ref(ptr);
    const lh_memory_tree_header_t *const parent = lh_memory_tree_header_of(ptr)->parent;
    return lh_ptr_is_set(parent) ? lh_memory_tree_data_of(parent) : lh_null;
}

lh_ptr
lh_memory_tree_get_first_child(const lh_ptr ptr)
{
    lh_assert_runtime_ref(ptr);
    const lh_memory_tree_header_t *const child = lh_memory_tree_header_of_sibling(
        lh_list_get_first(lh_addr_of(lh_memory_tree_header_of(ptr)->children)));
    return lh_ptr_is_set(child) ? lh_memory_tree_data_of(child) : lh_null;
}

lh_ptr
lh_memory_tree_get_next_sibling(const lh_ptr ptr)
{
    lh_assert_runtime_ref(ptr);
    lh_memory_tree_header_t *const header = lh_memory_tree_header_of(ptr);
    lh_return_ifn(header->parent, lh_null);
    const lh_memory_tree_header_t *const next = lh_memory_tree_header_of_sibling(
        lh_list_get_next(lh_addr_of(header->parent->children), lh_addr_of(header->sibling)));
    return lh_ptr_is_set(next) ? lh_memory_tree_data_of(next) : lh_null;
}

lh_void
lh_memory_tree_set_destructor(lh_ptr ptr, lh_memory_tree_destructor_cb destructor)
{
    lh_assert_runtime_ref(ptr);
    lh_memory_tree_header_of(ptr)->destructor = destructor;
}

/* lh_memory_allocator_t callbacks: self is the root the blocks go under. */

static lh_ptr
lh_memory_tree_allocator_alloc(lh_self_ptr self, lh_usize_t size)
{
    return lh_memory_tree_alloc_child(self, size);
}

static lh_void
lh_memory_tree_allocator_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    (void)self;
    lh_memory_tree_free(ptr);
}

static lh_ptr
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
