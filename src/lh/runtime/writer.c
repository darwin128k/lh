#include <lh/runtime/writer.h>
#include <lh/attribute/thread_local.h>
#include <lh/cast/static.h>
#include <lh/config.h>
#include <lh/library/option.h>
#include <lh/null.h>
#include <lh/self.h>
#include <lh/util/addr.h>

#if (LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include <stdio.h>

static lh_ssize_t
lh_runtime_writer_stdout(lh_self_ptr self, const lh_ptr buf, lh_usize_t size)
{
    size_t n;

    (void)self;
    if (size == 0u)
    {
        return 0;
    }
    n = fwrite(buf, 1u, lh_cast_static(size_t, size), stdout);
    if (n == 0u && ferror(stdout))
    {
        return LH_IO_ERROR;
    }
    return lh_cast_static(lh_ssize_t, n);
}

LH_ATTRIBUTE_THREAD_LOCAL
lh_io_writer_t m_runtime_writer = {lh_runtime_writer_stdout, lh_null};
#else
LH_ATTRIBUTE_THREAD_LOCAL
lh_io_writer_t m_runtime_writer = {lh_null, lh_null};
#endif

lh_io_writer_t *
lh_runtime_writer(void)
{
    return lh_addr_of(m_runtime_writer);
}

lh_void
lh_runtime_writer_init(lh_io_writer_write_cb write_cb, lh_ptr context)
{
    lh_io_writer_init(lh_runtime_writer(), write_cb, context);
}

lh_ssize_t
lh_runtime_writer_write(const lh_ptr buf, lh_usize_t size)
{
    return lh_io_writer_write(lh_runtime_writer(), buf, size);
}

lh_bool_t
lh_runtime_writer_write_all(const lh_ptr buf, lh_usize_t size)
{
    return lh_io_writer_write_all(lh_runtime_writer(), buf, size);
}

lh_ssize_t
lh_runtime_writer_print_v(const lh_str_ptr fmt, va_list args)
{
    return lh_io_writer_print_v(lh_runtime_writer(), fmt, args);
}

lh_ssize_t
lh_runtime_writer_print(const lh_str_ptr fmt, ...)
{
    va_list args;
    lh_ssize_t n;

    va_start(args, fmt);
    n = lh_runtime_writer_print_v(fmt, args);
    va_end(args);
    return n;
}
