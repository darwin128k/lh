/**
 * @file workspace.h
 * @brief Fixed caller-owned state for one synchronous gzip web dispatch.
 *
 * The workspace groups the buffers and nested decoder states whose lifetimes
 * must cover the same handler call. It owns no socket or request and performs
 * no allocation. Its storage is available through level-one getters; the
 * gzip reader initializer configures its members before use.
 */

#ifndef LH_UI_WEB_HOST_GZIP_WORKSPACE_H
#define LH_UI_WEB_HOST_GZIP_WORKSPACE_H

#include <lh/core/compress/gzip/reader.h>
#include <lh/ui/web/host/gzip/workspace/fields.h>

/** @brief Scratch and decoder states for one gzip dispatch. */
struct lh_ui_web_host_gzip_workspace
{
    /** Members: ::lh_ui_web_host_gzip_workspace_fields. */
    lh_ui_web_host_gzip_workspace_fields(lh_byte_t, lh_compress_gzip_header_decoder_t,
                                         lh_compress_deflate_reader_t, lh_compress_gzip_reader_t);
};
/** @copybrief lh_ui_web_host_gzip_workspace */
typedef struct lh_ui_web_host_gzip_workspace lh_ui_web_host_gzip_workspace_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Borrow the workspace's 1024-byte DEFLATE scratch.
 * @param self Valid workspace, retained by its owner.
 * @return Pointer to storage owned by self, valid while self exists.
 */
lh_byte_t *
lh_ui_web_host_gzip_workspace_get_scratch(lh_ui_web_host_gzip_workspace_t *self);

/**
 * @brief Borrow the workspace's 32768-byte DEFLATE history.
 * @param self Valid workspace, retained by its owner.
 * @return Pointer to storage owned by self, valid while self exists.
 */
lh_byte_t *
lh_ui_web_host_gzip_workspace_get_history(lh_ui_web_host_gzip_workspace_t *self);

/**
 * @brief Borrow the workspace's gzip header state.
 * @param self Valid workspace, retained by its owner.
 * @return Pointer to storage owned by self, valid while self exists.
 */
lh_compress_gzip_header_decoder_t *
lh_ui_web_host_gzip_workspace_get_header(lh_ui_web_host_gzip_workspace_t *self);

/**
 * @brief Borrow the workspace's DEFLATE reader state.
 * @param self Valid workspace, retained by its owner.
 * @return Pointer to storage owned by self, valid while self exists.
 */
lh_compress_deflate_reader_t *
lh_ui_web_host_gzip_workspace_get_deflate(lh_ui_web_host_gzip_workspace_t *self);

/**
 * @brief Borrow the workspace's gzip reader state.
 * @param self Valid workspace, retained by its owner.
 * @return Pointer to storage owned by self, valid while self exists.
 */
lh_compress_gzip_reader_t *
lh_ui_web_host_gzip_workspace_get_reader(lh_ui_web_host_gzip_workspace_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_GZIP_WORKSPACE_H */
