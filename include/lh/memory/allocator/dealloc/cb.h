#ifndef LH_MEMORY_ALLOCATOR_DEALLOC_CB_H
#define LH_MEMORY_ALLOCATOR_DEALLOC_CB_H

#include <lh/memory/allocator/dealloc/fn.h>
#include <lh/util/ptr.h>

#define lh_memory_allocator_dealloc_cb lh_ptr_of(lh_memory_allocator_dealloc_fn)

#endif // LH_MEMORY_ALLOCATOR_DEALLOC_CB_H