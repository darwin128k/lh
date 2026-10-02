#include <gtest/gtest.h>

#include <lh/array.h>
#include <lh/expect/death.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/memory/tree.h>
#include <lh/null.h>
#include <lh/runtime/allocator.h>
#include <lh/self.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

namespace
{

/* The allocator's state: how many blocks are live, so a test can tell
 * everything was given back. */
struct counting_heap
{
    int live;
};

/* What destructors ran, in order. */
std::string g_destroyed;

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
tree_test_alloc(lh_self_ptr self, lh_usize_t size)
{
    ++static_cast<counting_heap *>(self)->live;
    return std::malloc(static_cast<std::size_t>(size));
}

lh_void
tree_test_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    --static_cast<counting_heap *>(self)->live;
    std::free(ptr);
}

/* Each block used with it holds the name it records. */
lh_void
tree_test_record_destroy(lh_ptr ptr)
{
    g_destroyed += static_cast<const char *>(ptr);
}

LH_COMPILER_EXTERN_C_END

class MemoryTree : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        heap = counting_heap();
        sized = lh_memory_allocator_initializer_with_context(tree_test_alloc, tree_test_dealloc,
                                                             lh_null, &heap);
        g_destroyed.clear();
    }

    /* A block holding @p name that records it when destroyed. */
    lh_ptr
    named(lh_ptr parent, const char *name)
    {
        const lh_usize_t size = std::strlen(name) + 1;
        lh_ptr p = parent ? lh_memory_tree_alloc_child(parent, size)
                          : lh_memory_tree_alloc(&sized, size);
        std::memcpy(p, name, size);
        lh_memory_tree_set_destructor(p, tree_test_record_destroy);
        return p;
    }

    counting_heap heap;
    lh_memory_sized_allocator_t sized;
};

TEST_F(MemoryTree, freeing_a_block_frees_all_it_owns)
{
    lh_ptr root = lh_memory_tree_alloc(&sized, 0);
    lh_ptr a = lh_memory_tree_alloc_child(root, 10);
    lh_memory_tree_alloc_child(a, 20);
    lh_memory_tree_alloc_child(a, 30);
    lh_memory_tree_alloc_child(root, 40);
    EXPECT_EQ(heap.live, 5);

    lh_memory_tree_free(root);
    EXPECT_EQ(heap.live, 0);
}

TEST_F(MemoryTree, destructors_run_parent_first_then_children_oldest_first)
{
    lh_ptr root = named(nullptr, "R");
    lh_ptr a = named(root, "a");
    named(a, "1");
    named(a, "2");
    named(root, "b");

    lh_memory_tree_free(root);
    EXPECT_EQ(g_destroyed, "Ra12b");
    EXPECT_EQ(heap.live, 0);
}

TEST_F(MemoryTree, freeing_a_child_detaches_it_from_its_parent)
{
    lh_ptr root = lh_memory_tree_alloc(&sized, 0);
    lh_ptr a = lh_memory_tree_alloc_child(root, 1);
    lh_ptr b = lh_memory_tree_alloc_child(root, 1);
    lh_memory_tree_alloc_child(a, 1);

    lh_memory_tree_free(a);
    EXPECT_EQ(heap.live, 2);
    EXPECT_EQ(lh_memory_tree_get_first_child(root), b);
    EXPECT_EQ(lh_memory_tree_get_next_sibling(b), nullptr);

    lh_memory_tree_free(root);
    EXPECT_EQ(heap.live, 0);
    lh_memory_tree_free(lh_null); // ignored
}

TEST_F(MemoryTree, size_and_alignment)
{
    lh_ptr root = lh_memory_tree_alloc(&sized, 0);
    lh_ptr p = lh_memory_tree_alloc_child(root, 100);
    EXPECT_EQ(lh_memory_tree_get_size(root), 0u);
    EXPECT_EQ(lh_memory_tree_get_size(p), 100u);
    // The headers are multiples of 16, so data keeps malloc's alignment.
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(p) % alignof(std::max_align_t), 0u);
    lh_memory_tree_free(root);
}

TEST_F(MemoryTree, realloc_keeps_place_children_and_destructor)
{
    lh_ptr root = named(nullptr, "R");
    lh_ptr first = named(root, "f");
    lh_ptr mid = named(root, "m");
    lh_ptr last = named(root, "l");
    lh_ptr c1 = named(mid, "1");
    lh_ptr c2 = named(mid, "2");

    // Big enough that the block has to move.
    lh_ptr moved = lh_memory_tree_realloc(mid, 1 << 16);
    EXPECT_EQ(lh_memory_tree_get_size(moved), static_cast<lh_usize_t>(1 << 16));
    EXPECT_STREQ(static_cast<const char *>(moved), "m");

    EXPECT_EQ(lh_memory_tree_get_parent(moved), root);
    EXPECT_EQ(lh_memory_tree_get_first_child(root), first);
    EXPECT_EQ(lh_memory_tree_get_next_sibling(first), moved);
    EXPECT_EQ(lh_memory_tree_get_next_sibling(moved), last);
    EXPECT_EQ(lh_memory_tree_get_first_child(moved), c1);
    EXPECT_EQ(lh_memory_tree_get_next_sibling(c1), c2);
    EXPECT_EQ(lh_memory_tree_get_parent(c1), moved);
    EXPECT_EQ(lh_memory_tree_get_parent(c2), moved);

    lh_memory_tree_free(root);
    EXPECT_EQ(g_destroyed, "Rfm12l");
    EXPECT_EQ(heap.live, 0);
}

TEST_F(MemoryTree, set_parent_moves_ownership)
{
    lh_ptr old_root = lh_memory_tree_alloc(&sized, 0);
    lh_ptr new_root = lh_memory_tree_alloc(&sized, 0);
    lh_ptr obj = lh_memory_tree_alloc_child(old_root, 8);
    lh_memory_tree_alloc_child(obj, 8);

    lh_memory_tree_set_parent(obj, new_root);
    EXPECT_EQ(lh_memory_tree_get_parent(obj), new_root);
    EXPECT_EQ(lh_memory_tree_get_first_child(old_root), nullptr);

    lh_memory_tree_free(old_root); // obj survives: it moved out
    EXPECT_EQ(heap.live, 3);

    lh_memory_tree_set_parent(obj, lh_null); // now a root of its own
    lh_memory_tree_free(new_root);
    EXPECT_EQ(heap.live, 2);
    lh_memory_tree_free(obj);
    EXPECT_EQ(heap.live, 0);
}

TEST_F(MemoryTree, set_parent_refuses_a_cycle)
{
    lh_ptr root = lh_memory_tree_alloc(&sized, 0);
    lh_ptr child = lh_memory_tree_alloc_child(root, 0);
    LH_EXPECT_DEATH(lh_memory_tree_set_parent(root, child));
    LH_EXPECT_DEATH(lh_memory_tree_set_parent(root, root));
    lh_memory_tree_free(root);
}

TEST_F(MemoryTree, as_runtime_allocator_lh_containers_land_in_the_sandbox)
{
    lh_ptr sandbox = lh_memory_tree_alloc(&sized, 0);
    lh_memory_allocator_t tree_allocator;
    lh_memory_tree_allocator_init(&tree_allocator, sandbox);

    lh_memory_allocator_t saved;
    lh_memory_allocator_assign(&saved, lh_runtime_allocator());
    lh_memory_allocator_assign(lh_runtime_allocator(), &tree_allocator);

    // A "module" that allocates and forgets to free.
    lh_array_t leaked;
    lh_array_init(&leaked, sizeof(int));
    for (int i = 0; i < 1000; ++i)
    {
        lh_array_push_back(&leaked, &i); // grows: realloc inside the tree
    }

    lh_memory_allocator_assign(lh_runtime_allocator(), &saved);

    EXPECT_NE(lh_memory_tree_get_first_child(sandbox), nullptr);
    EXPECT_EQ(lh_memory_tree_get_parent(lh_array_get_data(&leaked)), sandbox);
    EXPECT_EQ(*static_cast<int *>(lh_array_get_ptr(&leaked, 999)), 999);

    lh_memory_tree_free(sandbox); // the module unloads
    EXPECT_EQ(heap.live, 0);
}

} // namespace
