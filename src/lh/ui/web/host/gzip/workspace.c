/**
 * @file workspace.c
 * @brief Accessors for caller-owned gzip web dispatch storage.
 */

#include <lh/ui/web/host/gzip/workspace.h>
#include <lh/core/assert.h>

lh_byte_t *
lh_ui_web_host_gzip_workspace_get_scratch(lh_ui_web_host_gzip_workspace_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch;
}

lh_byte_t *
lh_ui_web_host_gzip_workspace_get_history(lh_ui_web_host_gzip_workspace_t *self)
{
    lh_assert_runtime_ref(self);
    return self->history;
}

lh_compress_gzip_header_decoder_t *
lh_ui_web_host_gzip_workspace_get_header(lh_ui_web_host_gzip_workspace_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->header);
}

lh_compress_deflate_reader_t *
lh_ui_web_host_gzip_workspace_get_deflate(lh_ui_web_host_gzip_workspace_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->deflate);
}

lh_compress_gzip_reader_t *
lh_ui_web_host_gzip_workspace_get_reader(lh_ui_web_host_gzip_workspace_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->reader);
}
