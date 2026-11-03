/**
 * @file allocator.h
 * @brief Runtime allocator accessor and convenience allocation API.
 *
 * Same shape as ::lh_runtime_writer / ::lh_runtime_terminate: a slot the host
 * wires, then thin wrappers for everyday use. With
 * ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB on it starts as malloc/free;
 * off (bare-metal), empty until the application installs callbacks.
 *
 * The slot has two levels. The process slot (::lh_runtime_allocator_process)
 * is one for the whole program; every thread's own slot
 * (::lh_runtime_allocator) starts as a copy of it, taken on that thread's
 * first use. A host therefore installs its allocator once and every thread
 * sees it: the threads LH starts (logger queue, worker, loop) and the host's
 * own alike. A thread that needs another allocator replaces its own slot; no
 * other thread sees that.
 *
 * The process slot holds the address of an allocator the host owns and never
 * changes, published atomically (::lh_atomic_ptr_t): a thread taking its copy
 * while the host installs another gets one whole allocator, the old or the
 * new, never the callbacks of two heaps.
 *
 * @code
 * static const lh_memory_allocator_t pool = lh_memory_allocator_initializer(pool_alloc, pool_free);
 * lh_runtime_allocator_set_process(&pool); // at start-up
 * lh_ptr p = lh_runtime_allocator_alloc(64); // any thread
 * @endcode
 *
 * With ::LH_LIBRARY_OPTION_THREAD_LOCAL off, the thread slot is one more
 * program-wide object: it is seeded once, and the two levels act as one.
 *
 * ::lh_runtime_allocator() remains for hosts that need the underlying
 * ::lh_memory_allocator_t (context, realloc, …).
 */

#ifndef LH_RUNTIME_ALLOCATOR_H
#define LH_RUNTIME_ALLOCATOR_H

#include <lh/memory/allocator.h>

LH_COMPILER_EXTERN_C_BEGIN

/** The default allocation callback: ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC. */
lh_ptr
lh_runtime_allocator_default_alloc(lh_self_ptr self, lh_usize_t size);

/** The default release callback: ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC. */
lh_void
lh_runtime_allocator_default_dealloc(lh_self_ptr self, lh_ptr ptr);

/** The default resize callback: ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC; @p old_size
 * is unused. */
lh_ptr
lh_runtime_allocator_default_realloc(lh_self_ptr self, lh_ptr ptr, lh_usize_t old_size,
                                     lh_usize_t new_size);

/**
 * @brief Return the process runtime allocator.
 *
 * The allocator each thread's slot (::lh_runtime_allocator) is copied from on
 * that thread's first use. It is read with acquire order, so the allocator
 * the host published is seen whole. Starts as the configured C functions with
 * ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB on, empty with it off.
 *
 * @return The process allocator (never null); read-only, owned by whoever
 *         installed it (LH for the initial one).
 */
const lh_memory_allocator_t *
lh_runtime_allocator_process(void);

/**
 * @brief Install @p allocator as the process runtime allocator.
 *
 * Publishes the address with release order. Threads that use their slot
 * afterwards for the first time start from @p allocator; a thread that already
 * has its slot keeps it. So call it at start-up, before other threads use LH,
 * for every thread to get it. Safe while other threads run: each one sees the
 * old allocator or the new one, whole.
 *
 * @param allocator The allocator; must not be null. Not copied: the caller
 *        keeps it alive and unchanged until the end of the program, since
 *        threads copy it at any time (a static `const` object is the usual
 *        choice). Its callbacks are shared by every thread seeded from it, so
 *        they must be thread-safe.
 */
lh_void
lh_runtime_allocator_set_process(const lh_memory_allocator_t *allocator);

/**
 * @brief Return the address of the calling thread's runtime allocator.
 *
 * The object stores the alloc/dealloc callbacks LH uses for runtime memory on
 * this thread. On the thread's first call it is filled with a copy of
 * ::lh_runtime_allocator_process; after that it belongs to the thread alone:
 * writing it changes no other thread, and a later change of the process slot
 * does not reach it. With ::LH_LIBRARY_OPTION_THREAD_LOCAL off it is one
 * object for every thread.
 *
 * @return Pointer to the thread's ::lh_memory_allocator_t (never null).
 */
lh_memory_allocator_t *
lh_runtime_allocator(void);

/**
 * @brief Install alloc/dealloc on ::lh_runtime_allocator().
 *
 * Same as ::lh_memory_allocator_init(::lh_runtime_allocator(), …). Use this on
 * freestanding builds; with the stdlib option on, the slot is already wired.
 *
 * @param alloc_cb   Allocation callback (may be ::lh_null).
 * @param dealloc_cb Deallocation callback (may be ::lh_null).
 */
lh_void
lh_runtime_allocator_init(lh_memory_allocator_alloc_cb alloc_cb,
                          lh_memory_allocator_dealloc_cb dealloc_cb);

/**
 * @brief Allocate @p size bytes using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_alloc(::lh_runtime_allocator(), @p size).
 *
 * @param size Requested size in bytes.
 * @return Pointer to allocated memory.
 */
lh_ptr
lh_runtime_allocator_alloc(lh_usize_t size);

/**
 * @brief Free @p ptr using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_dealloc(::lh_runtime_allocator(), @p ptr).
 *
 * @param ptr Block to release (null is a no-op).
 */
lh_void
lh_runtime_allocator_free(lh_ptr ptr);

/**
 * @brief Resize or (re)allocate using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_realloc(::lh_runtime_allocator(), ...).
 *
 * @param old_ptr Previous block (or null).
 * @param old_size Previous size in bytes.
 * @param new_size New size in bytes.
 * @return Resized pointer, @p old_ptr, or null per ::lh_memory_allocator_realloc.
 */
lh_ptr
lh_runtime_allocator_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size);

/** Same semantics as ::lh_memory_allocator_alloc_zero(::lh_runtime_allocator(), ...). */
lh_ptr
lh_runtime_allocator_alloc_zero(lh_usize_t count, lh_usize_t item_size);

/** Same semantics as ::lh_memory_allocator_try_alloc(::lh_runtime_allocator(), @p size). */
lh_ptr
lh_runtime_allocator_try_alloc(lh_usize_t size);

/** Same semantics as ::lh_memory_allocator_try_alloc_zero(::lh_runtime_allocator(), ...). */
lh_ptr
lh_runtime_allocator_try_alloc_zero(lh_usize_t count, lh_usize_t item_size);

/** Same semantics as ::lh_memory_allocator_try_dealloc(::lh_runtime_allocator(), @p ptr). */
lh_bool_t
lh_runtime_allocator_try_free(lh_ptr ptr);

/** Same semantics as ::lh_memory_allocator_try_realloc(::lh_runtime_allocator(), ...). */
lh_ptr
lh_runtime_allocator_try_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_ALLOCATOR_H */
