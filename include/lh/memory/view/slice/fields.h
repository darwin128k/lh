#ifndef LH_MEMORY_VIEW_SLICE_FIELDS_H
#define LH_MEMORY_VIEW_SLICE_FIELDS_H

#include <lh/interval/bounds/fields.h>

#define lh_memory_view_slice_fields(T) lh_interval_bounds_fields(const T *)

#endif // LH_MEMORY_VIEW_SLICE_FIELDS_H
