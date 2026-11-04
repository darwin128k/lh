/**
 * @file fields.h
 * @brief Fields of an HTTP body reader over a caller-owned byte source.
 */

#ifndef LH_HTTP_BODY_READER_FIELDS_H
#define LH_HTTP_BODY_READER_FIELDS_H

/** @brief Source, scratch, framing state and sticky failure flag. */
#define lh_http_body_reader_fields(source_type, decoder_type, ptr_type, size_type, bool_type)      \
    source_type source;                                                                            \
    decoder_type decoder;                                                                          \
    ptr_type wire;                                                                                 \
    size_type wire_capacity;                                                                       \
    size_type wire_size;                                                                           \
    size_type wire_pos;                                                                            \
    size_type remaining;                                                                           \
    bool_type failed

#endif /* LH_HTTP_BODY_READER_FIELDS_H */
