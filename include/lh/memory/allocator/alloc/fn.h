#ifndef LH_MEMORY_ALLOCATOR_ALLOC_FN_H
#define LH_MEMORY_ALLOCATOR_ALLOC_FN_H

#include <lh/ptr.h>
#include <lh/size.h>

typedef lh_ptr(lh_memory_allocator_alloc_fn)(lh_usize_t size);

#endif // LH_MEMORY_ALLOCATOR_ALLOC_FN_H