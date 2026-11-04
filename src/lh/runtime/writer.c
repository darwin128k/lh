#include <lh/runtime/writer.h>
#include <lh/attribute/unused.h>

/* One object for the whole program, empty until the host fills it: the core
   has no C library and no defaults. The host fills it before the first use
   and before it starts other threads, so every thread sees the same one. */
lh_io_writer_t m_runtime_writer = {lh_null, lh_null};

lh_io_writer_t *
lh_runtime_writer_get_mut(void)
{
    return lh_addr_of(m_runtime_writer);
}

const lh_io_writer_t *
lh_runtime_writer_get(void)
{
    return lh_addr_of(m_runtime_writer);
}

lh_ssize_t
lh_runtime_writer_write(const lh_ptr buf, lh_usize_t size)
{
    return lh_io_writer_write(lh_runtime_writer_get_mut(), buf, size);
}

lh_bool_t
lh_runtime_writer_write_all(const lh_ptr buf, lh_usize_t size)
{
    return lh_io_writer_write_all(lh_runtime_writer_get_mut(), buf, size);
}

lh_ssize_t
lh_runtime_writer_print_v(const lh_str_ptr fmt, va_list args)
{
    return lh_io_writer_print_v(lh_runtime_writer_get_mut(), fmt, args);
}

lh_ssize_t
lh_runtime_writer_print(const lh_str_ptr fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const lh_ssize_t n = lh_runtime_writer_print_v(fmt, args);
    va_end(args);
    return n;
}
