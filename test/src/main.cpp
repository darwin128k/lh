#include <gtest/gtest.h>

#include <lh/core/memory/allocator/initializer.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/runtime/logger.h>
#include <lh/core/runtime/terminate.h>
#include <lh/core/runtime/terminate/handler/initializer.h>
#include <lh/core/runtime/writer.h>
#include <lh/core/str/format/text.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* The runner is a host like any other: the core starts with every runtime slot
   empty, and the runner fills them over the C library before any test runs,
   so every thread a test starts sees them too. */

namespace
{

lh_ptr
runner_alloc(lh_self_ptr self, lh_usize_t size)
{
    (void)self;
    return std::malloc(size);
}

lh_void
runner_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    (void)self;
    std::free(ptr);
}

lh_ptr
runner_realloc(lh_self_ptr self, lh_ptr ptr, lh_usize_t old_size, lh_usize_t new_size)
{
    (void)self;
    (void)old_size;
    return std::realloc(ptr, new_size);
}

lh_ssize_t
runner_write(lh_self_ptr self, const lh_ptr buf, lh_usize_t size)
{
    (void)self;
    return static_cast<lh_ssize_t>(std::fwrite(buf, 1u, size, stdout));
}

/* One record, one line on stderr; the death tests match the failed-check
   report there. */
lh_ssize_t
runner_log(lh_self_ptr self, lh_logger_level_t level, lh_str_cptr fmt, va_list args)
{
    (void)self;
    (void)level;
    char line[1024];
    const lh_usize_t n = lh_str_ptr_format_text_v(line, sizeof(line) - 1u, fmt, args);
    std::fwrite(n != 0u ? line : fmt, 1u, n != 0u ? n : std::strlen(fmt), stderr);
    std::fputc('\n', stderr);
    std::fflush(stderr);
    return static_cast<lh_ssize_t>(n + 1u);
}

lh_void
runner_abort(lh_self_ptr self)
{
    (void)self;
    std::abort();
}

/* Copied into the runtime slots before any test runs. */
const lh_memory_allocator_t runner_allocator =
    lh_memory_allocator_initializer_with_realloc(runner_alloc, runner_dealloc, runner_realloc);

const lh_io_writer_t runner_writer = {runner_write, lh_null};

const lh_logger_t runner_logger = {lh_logger_level_flags_all,
                                   runner_log,
                                   runner_log,
                                   runner_log,
                                   runner_log,
                                   runner_log,
                                   runner_log,
                                   runner_log,
                                   runner_log,
                                   lh_null};

const lh_runtime_terminate_handler_t runner_terminate =
    lh_runtime_terminate_handler_initializer(runner_abort, lh_null);

} // namespace

int
main(int argc, char **argv)
{
    lh_memory_allocator_assign(lh_runtime_allocator_get_mut(), &runner_allocator);
    lh_io_writer_assign(lh_runtime_writer_get_mut(), &runner_writer);
    lh_logger_assign(lh_runtime_logger_get_mut(), &runner_logger);
    lh_runtime_terminate_handler_assign(lh_runtime_terminate_get_mut(), &runner_terminate);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
