#ifndef LH_INTERVAL_FIELDS_H
#define LH_INTERVAL_FIELDS_H

#include <lh/interval_flags.h>

#define lh_interval_fields(T)                                                  \
    T bounds;                                                                  \
    lh_interval_flags_t flags

#endif // LH_INTERVAL_FIELDS_H
