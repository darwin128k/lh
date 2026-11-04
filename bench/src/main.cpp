#include <benchmark/benchmark.h>

#include <lh/core/memory/allocator/initializer.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/runtime/terminate.h>
#include <lh/core/runtime/terminate/handler/initializer.h>

#include <cstdlib>

/* The benchmark runner is a host: the core starts with every runtime slot
   empty, so it binds the C library before any benchmark allocates. */

namespace
{

lh_ptr
bench_alloc(lh_self_ptr self, lh_usize_t size)
{
    (void)self;
    return std::malloc(size);
}

lh_void
bench_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    (void)self;
    std::free(ptr);
}

lh_ptr
bench_realloc(lh_self_ptr self, lh_ptr ptr, lh_usize_t old_size, lh_usize_t new_size)
{
    (void)self;
    (void)old_size;
    return std::realloc(ptr, new_size);
}

lh_void
bench_abort(lh_self_ptr self)
{
    (void)self;
    std::abort();
}

/* Copied into the runtime slots before any benchmark runs. */
const lh_memory_allocator_t bench_allocator =
    lh_memory_allocator_initializer_with_realloc(bench_alloc, bench_dealloc, bench_realloc);

const lh_runtime_terminate_handler_t bench_terminate =
    lh_runtime_terminate_handler_initializer(bench_abort, lh_null);

} // namespace

int
main(int argc, char **argv)
{
    lh_memory_allocator_assign(lh_runtime_allocator_get_mut(), &bench_allocator);
    lh_runtime_terminate_handler_assign(lh_runtime_terminate_get_mut(), &bench_terminate);
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    {
        return 1;
    }
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}
