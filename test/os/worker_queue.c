#include <lh/os/worker.h>
#include <lh/os/thread.h>
#include <lh/spsc.h>
#include <lh/runtime/allocator.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                        \
            abort();                                                                               \
        }                                                                                          \
    } while (0)
static atomic_uint worked;
static unsigned destroyed;
static lh_os_worker_t *closing;

static void
work(void *p)
{
    (void)p;
    atomic_fetch_add(&worked, 1);
}

static void
dispose(void *p)
{
    (void)p;
    ++destroyed;
    if (closing)
    {
        CHECK(!lh_os_worker_submit(closing, work, NULL, NULL, NULL));
    }
}

static lh_spsc_t *queue;

static void
produce(void *p)
{
    unsigned i;
    (void)p;
    for (i = 0; i < 100000; ++i)
    {
        while (!lh_spsc_push(queue, &i))
        {
            lh_os_thread_sleep(0);
        }
    }
}

typedef struct allocator_case
{
    unsigned live;
    int reject;
} allocator_case_t;

static lh_ptr
tracked_alloc(lh_ptr context, lh_usize_t size)
{
    allocator_case_t *test = context;
    lh_ptr block;
    if (test->reject)
    {
        return NULL;
    }
    block = malloc(size);
    if (block)
    {
        ++test->live;
    }
    return block;
}

static lh_void
tracked_free(lh_ptr context, lh_ptr block)
{
    allocator_case_t *test = context;
    CHECK(test->live);
    --test->live;
    free(block);
}

static void
test_allocator(void)
{
    allocator_case_t test = {0};
    lh_memory_allocator_t previous = *lh_runtime_allocator();
    lh_memory_allocator_t custom = previous;
    lh_os_worker_t *worker;
    lh_os_worker_job_t *job = NULL;
    lh_spsc_t *records;
    unsigned storage[2], i;
    lh_memory_allocator_set(&custom, tracked_alloc, tracked_free);
    lh_memory_allocator_set_context(&custom, &test);
    *lh_runtime_allocator() = custom;
    test.reject = 1;
    CHECK(!lh_spsc_create(storage, 2, sizeof storage[0]));
    CHECK(!lh_os_worker_create(2));
    CHECK(test.live == 0);
    test.reject = 0;
    records = lh_spsc_create(storage, 2, sizeof storage[0]);
    CHECK(records);
    worker = lh_os_worker_create(2);
    CHECK(worker);
    test.reject = 1;
    CHECK(!lh_os_worker_submit(worker, work, NULL, NULL, NULL));
    test.reject = 0;
    CHECK(lh_os_worker_submit(worker, work, NULL, NULL, NULL));
    for (i = 0; !job && i < 5000; ++i)
    {
        job = lh_os_worker_take(worker);
        if (!job)
        {
            lh_os_thread_sleep(1);
        }
    }
    CHECK(job);
    /* Taken records and queues retain their original allocator after runtime changes. */
    *lh_runtime_allocator() = previous;
    lh_os_worker_destroy(worker);
    lh_os_worker_dispose(job);
    lh_spsc_destroy(records);
    CHECK(test.live == 0);
}

int
main(void)
{
    unsigned storage[8], i, value;
    lh_os_thread_t producer;
    lh_os_worker_job_t *job;
    lh_os_worker_t *w;
    CHECK(!lh_spsc_create(storage, 3, sizeof(unsigned)));
    queue = lh_spsc_create(storage, 8, sizeof(unsigned));
    CHECK(queue);
    CHECK(!lh_spsc_peek(queue));
    for (i = 0; i < 8; ++i)
    {
        CHECK(lh_spsc_push(queue, &i));
    }
    CHECK(!lh_spsc_push(queue, &i));
    for (i = 0; i < 8; ++i)
    {
        CHECK(*(const unsigned *)lh_spsc_peek(queue) == i);
        lh_spsc_release(queue);
    }
    lh_os_thread_init(&producer);
    CHECK(lh_os_thread_start(&producer, produce, NULL, -1, 0));
    for (i = 0; i < 100000; ++i)
    {
        const unsigned *item;
        while (!(item = lh_spsc_peek(queue)))
        {
            lh_os_thread_sleep(0);
        }
        value = *item;
        CHECK(value == i);
        lh_spsc_release(queue);
    }
    CHECK(lh_os_thread_join(&producer));
    CHECK(!lh_spsc_peek(queue));
    lh_spsc_destroy(queue);
    CHECK(!lh_os_worker_create(0));
    w = lh_os_worker_create(2);
    CHECK(w);
    CHECK(lh_os_worker_submit(w, work, NULL, dispose, NULL));
    CHECK(lh_os_worker_submit(w, work, NULL, dispose, NULL));
    for (i = 0; atomic_load(&worked) != 2 && i < 5000; ++i)
    {
        lh_os_thread_sleep(1);
    }
    CHECK(atomic_load(&worked) == 2);
    /* Completed records continue to occupy capacity until the caller takes them. */
    CHECK(!lh_os_worker_submit(w, work, NULL, dispose, NULL));
    job = lh_os_worker_take(w);
    CHECK(job);
    lh_os_worker_dispose(job);
    CHECK(destroyed == 1);
    CHECK(lh_os_worker_submit(w, work, NULL, dispose, NULL));
    lh_os_worker_stop(w);
    CHECK(!lh_os_worker_submit(w, work, NULL, dispose, NULL));
    closing = w;
    lh_os_worker_destroy(w);
    closing = NULL;
    CHECK(destroyed == 3);
    test_allocator();
    return 0;
}
