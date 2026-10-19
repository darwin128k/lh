/**
 * @file fields.h
 * @brief Member fields of ::lh_ini_entry_t.
 */

#ifndef LH_INI_ENTRY_FIELDS_H
#define LH_INI_ENTRY_FIELDS_H

/**
 * @def lh_ini_entry_fields(kind_type, section_type, pair_type)
 * @brief What the entry is, the section it is in, and its key and value.
 *
 * @param kind_type    Type of `kind` (::lh_ini_entry_kind_t).
 * @param section_type Type of `section` (::lh_str_view_t).
 * @param pair_type    Type of `pair` (::lh_pair_t).
 */
#define lh_ini_entry_fields(kind_type, section_type, pair_type)                                    \
    kind_type kind;                                                                                \
    section_type section;                                                                          \
    pair_type pair

#endif /* LH_INI_ENTRY_FIELDS_H */
