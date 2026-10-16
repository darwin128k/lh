#include <lh/runtime/stream.h>
#include <lh/attribute/thread_local.h>
#include <lh/runtime/reader.h>
#include <lh/runtime/writer.h>

LH_ATTRIBUTE_THREAD_LOCAL
lh_io_stream_t m_runtime_stream;

lh_io_stream_t *
lh_runtime_stream(void)
{
    lh_io_stream_init(lh_addr_of(m_runtime_stream), lh_runtime_reader(), lh_runtime_writer());
    return lh_addr_of(m_runtime_stream);
}

lh_ssize_t
lh_runtime_stream_read(lh_ptr buf, lh_usize_t size)
{
    return lh_io_stream_read(lh_runtime_stream(), buf, size);
}

lh_ssize_t
lh_runtime_stream_write(const lh_ptr buf, lh_usize_t size)
{
    return lh_io_stream_write(lh_runtime_stream(), buf, size);
}
