#include <lh/os/system/win/concurrency/condition.h>
#include <lh/os/system/win/concurrency/thread.h>
#include <lh/os/system/error/capture.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <errno.h>
#include <lh/assert.h>

/**
 * @brief Capture a Win32 failure on this thread and return false.
 */
static lh_bool_t
lh_os_concurrency_capture(DWORD code)
{
    SetLastError(code);
    lh_os_system_error_capture();
    return lh_bool_false;
}

/**
 * @brief Complete the startup handshake and invoke user code only when startup succeeds.
 */
static unsigned __stdcall lh_os_thread_native_run(void *arg)
{
    lh_os_native_thread_t *self = arg;
    if (WaitForSingleObject(lh_os_native_thread_get_gate(self), INFINITE) == WAIT_OBJECT_0 &&
        lh_os_native_thread_get_proceed(self))
    {
        lh_os_native_thread_get_run_fn(self)(lh_os_native_thread_get_user(self));
    }
    return 0;
}

/**
 * @brief Create a native worker and validate its startup options before user code runs.
 */
lh_os_system_thread_handle_t
lh_os_system_thread_start(lh_os_thread_fn run, lh_ptr user, lh_s32_t cpu, lh_u32_t flags)
{
    if (!run || cpu < -1 || cpu >= (lh_s32_t)(8 * sizeof(DWORD_PTR)) ||
        (flags & ~lh_os_thread_block_sigpipe))
    {
        lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    lh_os_native_thread_t *self = calloc(1, sizeof *self);
    if (!self)
    {
        lh_os_concurrency_capture(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    lh_os_native_thread_set_run_fn(self, run);
    lh_os_native_thread_set_user(self, user);
    lh_os_native_thread_set_gate(self, CreateEventW(NULL, TRUE, FALSE, NULL));
    if (!lh_os_native_thread_get_gate(self))
    {
        DWORD error = GetLastError();
        free(self);
        lh_os_concurrency_capture(error);
        return NULL;
    }
    /* CRT per-thread state is released when the callback returns. */
    lh_os_native_thread_set_handle(self,
                                   (HANDLE)_beginthreadex(NULL, 0, lh_os_thread_native_run, self, 0,
                                                          lh_os_native_thread_get_id_mut(self)));
    DWORD error = 0;
    if (!lh_os_native_thread_get_handle(self))
    {
        error = errno == EAGAIN   ? ERROR_MAX_THRDS_REACHED
                : errno == EINVAL ? ERROR_INVALID_PARAMETER
                                  : ERROR_NOT_ENOUGH_MEMORY;
    }
    else if (cpu >= 0 &&
             !SetThreadAffinityMask(lh_os_native_thread_get_handle(self), (DWORD_PTR)1 << cpu))
    {
        error = GetLastError();
    }
    if (error)
    {
        if (lh_os_native_thread_get_handle(self))
        {
            /* Release the startup gate without running user code. */
            SetEvent(lh_os_native_thread_get_gate(self));
            WaitForSingleObject(lh_os_native_thread_get_handle(self), INFINITE);
            CloseHandle(lh_os_native_thread_get_handle(self));
        }
        CloseHandle(lh_os_native_thread_get_gate(self));
        free(self);
        lh_os_concurrency_capture(error);
        return NULL;
    }
    lh_os_native_thread_set_proceed(self, 1);
    SetEvent(lh_os_native_thread_get_gate(self));
    return self;
}

/**
 * @brief Wait for a native worker and release its handle.
 */
lh_bool_t
lh_os_system_thread_join(lh_os_system_thread_handle_t handle)
{
    lh_os_native_thread_t *self = handle;
    if (!self)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    if (lh_os_native_thread_get_id(self) == GetCurrentThreadId())
    {
        return lh_os_concurrency_capture(ERROR_POSSIBLE_DEADLOCK);
    }
    DWORD status = WaitForSingleObject(lh_os_native_thread_get_handle(self), INFINITE);
    if (status != WAIT_OBJECT_0)
    {
        return lh_os_concurrency_capture(status == WAIT_FAILED ? GetLastError()
                                                               : ERROR_GEN_FAILURE);
    }
    if (!CloseHandle(lh_os_native_thread_get_handle(self)))
    {
        return lh_os_concurrency_capture(GetLastError());
    }
    CloseHandle(lh_os_native_thread_get_gate(self));
    free(self);
    return lh_bool_true;
}

/**
 * @brief Allocate and initialize one native mutex object.
 */
lh_os_system_mutex_handle_t
lh_os_system_mutex_open(void)
{
    CRITICAL_SECTION *self = malloc(sizeof *self);
    if (!self)
    {
        lh_os_concurrency_capture(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    if (!InitializeCriticalSectionAndSpinCount(self, 0))
    {
        DWORD error = GetLastError();
        free(self);
        lh_os_concurrency_capture(error);
        return NULL;
    }
    return self;
}

/**
 * @brief Release a native mutex after its last use.
 */
lh_bool_t
lh_os_system_mutex_close(lh_os_system_mutex_handle_t handle)
{
    if (!handle)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    DeleteCriticalSection(handle);
    free(handle);
    return lh_bool_true;
}

/**
 * @brief Lock a native mutex on the calling thread.
 */
lh_bool_t
lh_os_system_mutex_lock(lh_os_system_mutex_handle_t handle)
{
    if (!handle)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    EnterCriticalSection(handle);
    return lh_bool_true;
}

/**
 * @brief Unlock the native mutex held by the calling thread.
 */
lh_bool_t
lh_os_system_mutex_unlock(lh_os_system_mutex_handle_t handle)
{
    if (!handle)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    LeaveCriticalSection(handle);
    return lh_bool_true;
}

/**
 * @brief Allocate and initialize one native condition object.
 */
lh_os_system_condition_handle_t
lh_os_system_condition_open(void)
{
    lh_os_native_condition_t *self = calloc(1, sizeof *self);
    if (!self)
    {
        lh_os_concurrency_capture(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    HMODULE kernel = GetModuleHandleW(L"kernel32.dll");
    if (!kernel)
    {
        DWORD error = GetLastError();
        free(self);
        lh_os_concurrency_capture(error);
        return NULL;
    }
    union
    {
        FARPROC raw;
        lh_os_condition_change_fn change;
        lh_os_condition_wait_fn wait;
    } procedure;
    procedure.raw = GetProcAddress(kernel, "InitializeConditionVariable");
    lh_os_condition_change_fn init = procedure.change;
    procedure.raw = GetProcAddress(kernel, "WakeConditionVariable");
    lh_os_native_condition_set_signal_fn(self, procedure.change);
    procedure.raw = GetProcAddress(kernel, "WakeAllConditionVariable");
    lh_os_native_condition_set_broadcast_fn(self, procedure.change);
    procedure.raw = GetProcAddress(kernel, "SleepConditionVariableCS");
    lh_os_native_condition_set_wait_fn(self, procedure.wait);
    if (!init || !lh_os_native_condition_get_signal_fn(self) ||
        !lh_os_native_condition_get_broadcast_fn(self) || !lh_os_native_condition_get_wait_fn(self))
    {
        free(self);
        lh_os_concurrency_capture(ERROR_CALL_NOT_IMPLEMENTED);
        return NULL;
    }
    init(lh_os_native_condition_get_state_mut(self));
    return self;
}

/**
 * @brief Release a native condition after its last use.
 */
lh_bool_t
lh_os_system_condition_close(lh_os_system_condition_handle_t handle)
{
    if (!handle)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    free(handle);
    return lh_bool_true;
}

/**
 * @brief Wait on a native condition, atomically releasing and reacquiring the mutex.
 */
lh_bool_t
lh_os_system_condition_wait(lh_os_system_condition_handle_t handle,
                            lh_os_system_mutex_handle_t mutex)
{
    lh_os_native_condition_t *self = handle;
    if (!self || !mutex)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    if (!lh_os_native_condition_get_wait_fn(self)(lh_os_native_condition_get_state_mut(self), mutex,
                                                  INFINITE))
    {
        return lh_os_concurrency_capture(GetLastError());
    }
    return lh_bool_true;
}

/**
 * @brief Notify one native condition waiter, if any.
 */
lh_bool_t
lh_os_system_condition_signal(lh_os_system_condition_handle_t handle)
{
    lh_os_native_condition_t *self = handle;
    if (!self)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    lh_os_native_condition_get_signal_fn(self)(lh_os_native_condition_get_state_mut(self));
    return lh_bool_true;
}

/**
 * @brief Notify all current native condition waiters.
 */
lh_bool_t
lh_os_system_condition_broadcast(lh_os_system_condition_handle_t handle)
{
    lh_os_native_condition_t *self = handle;
    if (!self)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    lh_os_native_condition_get_broadcast_fn(self)(lh_os_native_condition_get_state_mut(self));
    return lh_bool_true;
}

HANDLE
lh_os_native_thread_get_handle(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_os_native_thread_set_handle(lh_os_native_thread_t *self, HANDLE handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

unsigned
lh_os_native_thread_get_id(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->id;
}

lh_void
lh_os_native_thread_set_id(lh_os_native_thread_t *self, unsigned id)
{
    lh_assert_runtime_ref(self);
    self->id = id;
}

HANDLE
lh_os_native_thread_get_gate(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gate;
}

lh_void
lh_os_native_thread_set_gate(lh_os_native_thread_t *self, HANDLE gate)
{
    lh_assert_runtime_ref(self);
    self->gate = gate;
}

int
lh_os_native_thread_get_proceed(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->proceed;
}

lh_void
lh_os_native_thread_set_proceed(lh_os_native_thread_t *self, int proceed)
{
    lh_assert_runtime_ref(self);
    self->proceed = proceed;
}

lh_os_thread_fn
lh_os_native_thread_get_run_fn(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->run;
}

lh_void
lh_os_native_thread_set_run_fn(lh_os_native_thread_t *self, lh_os_thread_fn run)
{
    lh_assert_runtime_ref(self);
    self->run = run;
}

lh_ptr
lh_os_native_thread_get_user(const lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_os_native_thread_set_user(lh_os_native_thread_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

const PVOID *
lh_os_native_condition_get_state(const lh_os_native_condition_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->state);
}

PVOID *
lh_os_native_condition_get_state_mut(lh_os_native_condition_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->state);
}

lh_os_condition_change_fn
lh_os_native_condition_get_signal_fn(const lh_os_native_condition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->signal;
}

lh_void
lh_os_native_condition_set_signal_fn(lh_os_native_condition_t *self,
                                     lh_os_condition_change_fn signal)
{
    lh_assert_runtime_ref(self);
    self->signal = signal;
}

lh_os_condition_change_fn
lh_os_native_condition_get_broadcast_fn(const lh_os_native_condition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->broadcast;
}

lh_void
lh_os_native_condition_set_broadcast_fn(lh_os_native_condition_t *self,
                                        lh_os_condition_change_fn broadcast)
{
    lh_assert_runtime_ref(self);
    self->broadcast = broadcast;
}

lh_os_condition_wait_fn
lh_os_native_condition_get_wait_fn(const lh_os_native_condition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wait;
}

lh_void
lh_os_native_condition_set_wait_fn(lh_os_native_condition_t *self, lh_os_condition_wait_fn wait)
{
    lh_assert_runtime_ref(self);
    self->wait = wait;
}

unsigned *
lh_os_native_thread_get_id_mut(lh_os_native_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->id);
}
