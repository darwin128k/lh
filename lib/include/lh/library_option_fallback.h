#ifndef LH_LIBRARY_OPTION_FALLBACK_H
#define LH_LIBRARY_OPTION_FALLBACK_H

#include <lh/library_option.h>

// ---------------------------------------------------------------------------------------------
// //
//                                             BUILD //
// ---------------------------------------------------------------------------------------------
// //

#ifndef LH_LIBRARY_OPTION_SHARED_BUILD
#    define LH_LIBRARY_OPTION_SHARED_BUILD LH_LIBRARY_OPTION_OFF
#endif // LH_LIBRARY_OPTION_SHARED_BUILD

#ifndef LH_LIBRARY_OPTION_STATIC_BUILD
#    define LH_LIBRARY_OPTION_STATIC_BUILD LH_LIBRARY_OPTION_OFF
#endif // LH_LIBRARY_OPTION_STATIC_BUILD

// ---------------------------------------------------------------------------------------------
// //
//                                            THREAD //
// ---------------------------------------------------------------------------------------------
// //

#ifndef LH_LIBRARY_OPTION_THREAD_LOCAL
#    define LH_LIBRARY_OPTION_THREAD_LOCAL LH_LIBRARY_OPTION_OFF
#endif

// ---------------------------------------------------------------------------------------------
// //
//                                           ALLOCATOR //
// ---------------------------------------------------------------------------------------------
// //

#ifndef LH_LIBRARY_OPTION_ARRAY_OPTIMIZE_RESIZE
#    define LH_LIBRARY_OPTION_ARRAY_OPTIMIZE_RESIZE LH_LIBRARY_OPTION_OFF
#endif // LH_LIBRARY_OPTION_ARRAY_OPTIMIZE_RESIZE

// ---------------------------------------------------------------------------------------------
// //
//                                             ARRAY //
// ---------------------------------------------------------------------------------------------
// //

#ifndef LH_LIBRARY_OPTION_ARRAY_DEFAULT_SHRINK_RATIO
#    define LH_LIBRARY_OPTION_ARRAY_DEFAULT_SHRINK_RATIO 2
#endif // LH_LIBRARY_OPTION_ARRAY_DEFAULT_SHRINK_RATIO

#ifndef LH_LIBRARY_OPTION_ARRAY_DEFAULT_GROWTH_RATIO
#    define LH_LIBRARY_OPTION_ARRAY_DEFAULT_GROWTH_RATIO 1500
#endif // LH_LIBRARY_OPTION_ARRAY_DEFAULT_GROWTH_RATIO

#ifndef LH_LIBRARY_OPTION_ARRAY_RESERVE_OPTIMIZE
#    define LH_LIBRARY_OPTION_ARRAY_RESERVE_OPTIMIZE LH_LIBRARY_OPTION_OFF
#endif // LH_LIBRARY_OPTION_ARRAY_RESERVE_OPTIMIZE

// ---------------------------------------------------------------------------------------------
// //
//                                            RUNTIME //
// ---------------------------------------------------------------------------------------------
// //

#ifndef LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX
#    define LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX 255
#endif // LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX

#ifndef LH_LIBRARY_OPTION_RUNTIME_ALLOCATOR_USE_STDLIB
#    define LH_LIBRARY_OPTION_RUNTIME_ALLOCATOR_USE_STDLIB LH_LIBRARY_OPTION_OFF
#endif // LH_LIBRARY_OPTION_RUNTIME_ALLOCATOR_USE_STDLIB

#ifndef LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB
#    define LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB LH_LIBRARY_OPTION_OFF
#endif // LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB

#endif // LH_LIBRARY_OPTION_FALLBACK_H