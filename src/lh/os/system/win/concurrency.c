#include <lh/os/system/concurrency.h>
#include <lh/os/system/error/capture.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <errno.h>

typedef struct lh_os_native_thread
{
    HANDLE handle;
    unsigned id;
    HANDLE gate;
    int proceed;
    lh_os_thread_fn run;
    lh_ptr user;
} lh_os_native_thread_t;

static lh_bool_t
lh_os_concurrency_capture(DWORD code)
{
    SetLastError(code);
    lh_os_system_error_capture();
    return lh_bool_false;
}
static unsigned __stdcall lh_os_thread_native_run(void *arg)
{
    lh_os_native_thread_t *self = arg;
    if (WaitForSingleObject(self->gate, INFINITE) == WAIT_OBJECT_0 && self->proceed)
    {
        self->run(self->user);
    }
    return 0;
}
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
    self->run = run;
    self->user = user;
    self->gate = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!self->gate)
    {
        DWORD error = GetLastError();
        free(self);
        lh_os_concurrency_capture(error);
        return NULL;
    }
    /* CRT per-thread state is released when the callback returns. */
    self->handle = (HANDLE)_beginthreadex(NULL, 0, lh_os_thread_native_run, self, 0, &self->id);
    DWORD error = 0;
    if (!self->handle)
    {
        error = errno == EAGAIN   ? ERROR_MAX_THRDS_REACHED
                : errno == EINVAL ? ERROR_INVALID_PARAMETER
                                  : ERROR_NOT_ENOUGH_MEMORY;
    }
    else if (cpu >= 0 && !SetThreadAffinityMask(self->handle, (DWORD_PTR)1 << cpu))
    {
        error = GetLastError();
    }
    if (error)
    {
        if (self->handle)
        {
            /* Release the startup gate without running user code. */
            SetEvent(self->gate);
            WaitForSingleObject(self->handle, INFINITE);
            CloseHandle(self->handle);
        }
        CloseHandle(self->gate);
        free(self);
        lh_os_concurrency_capture(error);
        return NULL;
    }
    self->proceed = 1;
    SetEvent(self->gate);
    return self;
}
lh_bool_t
lh_os_system_thread_join(lh_os_system_thread_handle_t handle)
{
    lh_os_native_thread_t *self = handle;
    if (!self)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    if (self->id == GetCurrentThreadId())
    {
        return lh_os_concurrency_capture(ERROR_POSSIBLE_DEADLOCK);
    }
    DWORD status = WaitForSingleObject(self->handle, INFINITE);
    if (status != WAIT_OBJECT_0)
    {
        return lh_os_concurrency_capture(status == WAIT_FAILED ? GetLastError()
                                                               : ERROR_GEN_FAILURE);
    }
    if (!CloseHandle(self->handle))
    {
        return lh_os_concurrency_capture(GetLastError());
    }
    CloseHandle(self->gate);
    free(self);
    return lh_bool_true;
}

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

/* CONDITION_VARIABLE is a pointer-sized opaque state. Resolve its API at open,
 * so other LH OS services can still load on systems lacking condition variables.
 * All calls use the Win32 calling convention, including on 32-bit Windows. */
typedef VOID(WINAPI *lh_os_condition_change_fn)(PVOID state);
typedef BOOL(WINAPI *lh_os_condition_wait_fn)(PVOID state, PCRITICAL_SECTION mutex, DWORD timeout);
typedef struct lh_os_native_condition
{
    PVOID state;
    lh_os_condition_change_fn signal, broadcast;
    lh_os_condition_wait_fn wait;
} lh_os_native_condition_t;

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
    self->signal = procedure.change;
    procedure.raw = GetProcAddress(kernel, "WakeAllConditionVariable");
    self->broadcast = procedure.change;
    procedure.raw = GetProcAddress(kernel, "SleepConditionVariableCS");
    self->wait = procedure.wait;
    if (!init || !self->signal || !self->broadcast || !self->wait)
    {
        free(self);
        lh_os_concurrency_capture(ERROR_CALL_NOT_IMPLEMENTED);
        return NULL;
    }
    init(&self->state);
    return self;
}
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
lh_bool_t
lh_os_system_condition_wait(lh_os_system_condition_handle_t handle,
                            lh_os_system_mutex_handle_t mutex)
{
    lh_os_native_condition_t *self = handle;
    if (!self || !mutex)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    if (!self->wait(&self->state, mutex, INFINITE))
    {
        return lh_os_concurrency_capture(GetLastError());
    }
    return lh_bool_true;
}
lh_bool_t
lh_os_system_condition_signal(lh_os_system_condition_handle_t handle)
{
    lh_os_native_condition_t *self = handle;
    if (!self)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    self->signal(&self->state);
    return lh_bool_true;
}
lh_bool_t
lh_os_system_condition_broadcast(lh_os_system_condition_handle_t handle)
{
    lh_os_native_condition_t *self = handle;
    if (!self)
    {
        return lh_os_concurrency_capture(ERROR_INVALID_PARAMETER);
    }
    self->broadcast(&self->state);
    return lh_bool_true;
}
