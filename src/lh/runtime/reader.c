#include <lh/runtime/reader.h>
#include <lh/attribute/thread_local.h>
#include <lh/config.h>

#if (LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include <stdio.h>

lh_ssize_t
lh_runtime_reader_stdin(lh_self_ptr self, lh_ptr buf, lh_usize_t size)
{
    size_t n;

    (void)self;
    if (size == 0u)
    {
        return 0;
    }
    n = fread(buf, 1u, lh_cast_static(size_t, size), stdin);
    if (n == 0u && ferror(stdin))
    {
        return -1;
    }
    return lh_cast_static(lh_ssize_t, n);
}

LH_ATTRIBUTE_THREAD_LOCAL
lh_io_reader_t m_runtime_reader = {lh_runtime_reader_stdin, lh_null};
#else
LH_ATTRIBUTE_THREAD_LOCAL
lh_io_reader_t m_runtime_reader = {lh_null, lh_null};
#endif

lh_io_reader_t *
lh_runtime_reader(void)
{
    return lh_addr_of(m_runtime_reader);
}

lh_void
lh_runtime_reader_init(lh_io_reader_read_cb read_cb, lh_ptr context)
{
    lh_io_reader_init(lh_runtime_reader(), read_cb, context);
}

lh_ssize_t
lh_runtime_reader_read(lh_ptr buf, lh_usize_t size)
{
    return lh_io_reader_read(lh_runtime_reader(), buf, size);
}
