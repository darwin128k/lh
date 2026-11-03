#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/terminate.h>

#include <cstdlib>

namespace
{

/* A build with USE_STDLIB=OFF leaves the runtime allocator and the terminate
   handler empty, for the host to install. The runner is that host: it binds
   the C library to the process slots here, before any test runs, so every
   thread a test starts sees them too and every option set runs the same tests.
   A build with USE_STDLIB=ON already starts with them. */
#if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB == LH_LIBRARY_OPTION_OFF)
lh_ptr
test_runner_alloc(lh_self_ptr self, lh_usize_t size)
{
    (void)self;
    return std::malloc(size);
}

lh_void
test_runner_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    (void)self;
    std::free(ptr);
}

/* Published by address: it lives, unchanged, until the program ends. */
const lh_memory_allocator_t test_runner_allocator =
    lh_memory_allocator_initializer(test_runner_alloc, test_runner_dealloc);
#endif

void
test_runner_install_host(void)
{
#if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB == LH_LIBRARY_OPTION_OFF)
    lh_runtime_allocator_set_process(&test_runner_allocator);
#endif
#if (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_OFF)
    lh_runtime_terminate_set_process(std::abort);
#endif
}

} // namespace

int
main(int argc, char **argv)
{
    test_runner_install_host();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
