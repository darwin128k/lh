#include <lh/core/runtime/reader.h>
#include <lh/core/attribute/unused.h>

/* One object for the whole program, empty until the host fills it: the core
   has no C library and no defaults. The host fills it before the first use
   and before it starts other threads, so every thread sees the same one. */
lh_io_reader_t m_runtime_reader = {lh_null, lh_null};

lh_io_reader_t *
lh_runtime_reader_get_mut(void)
{
    return lh_addr_of(m_runtime_reader);
}

const lh_io_reader_t *
lh_runtime_reader_get(void)
{
    return lh_addr_of(m_runtime_reader);
}

lh_ssize_t
lh_runtime_reader_read(lh_ptr buf, lh_usize_t size)
{
    return lh_io_reader_read(lh_runtime_reader_get_mut(), buf, size);
}
