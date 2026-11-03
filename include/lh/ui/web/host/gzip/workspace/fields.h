/**
 * @file fields.h
 * @brief Storage fields of the web host's gzip decoding workspace.
 *
 * The workspace owns fixed scratch, history and decoder states for one
 * synchronous dispatch; it does not own the request or its source.
 */

#ifndef LH_UI_WEB_HOST_GZIP_WORKSPACE_FIELDS_H
#define LH_UI_WEB_HOST_GZIP_WORKSPACE_FIELDS_H

/** @brief Expand fixed buffers and member decoder states. */
#define lh_ui_web_host_gzip_workspace_fields(byte_type, header_type, deflate_type, gzip_type)      \
    byte_type scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];                                      \
    byte_type history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];                                    \
    header_type header;                                                                            \
    deflate_type deflate;                                                                          \
    gzip_type reader

#endif /* LH_UI_WEB_HOST_GZIP_WORKSPACE_FIELDS_H */
