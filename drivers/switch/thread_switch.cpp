#if defined(HORIZON_ENABLED)
#include "thread_switch.h"
#include "os/memory.h"
#include <switch.h>
#include <switch/arm/tls.h>

// Internals.
// :(

// This structure is exactly 0x20 bytes, if more is needed modify getThreadVars() below
typedef struct {
    // Magic value used to check if the struct is initialized
    u32 magic;

    // Thread handle, for mutexes
    Handle handle;

    // Pointer to the current thread (if exists)
    Thread* thread_ptr;

    // Pointer to this thread's newlib state
    struct _reent* reent;

    // Pointer to this thread's thread-local segment
    void* tls_tp; // !! Offset needs to be TLS+0x1F8 for __aarch64_read_tp !!
} ThreadVars;

static inline ThreadVars* getThreadVars(void) {
    return (ThreadVars*)((u8*)armGetTls() + 0x1E0);
}

void ThreadSwitch::thread_func(void* userdata)
{
	ThreadSwitch *t = reinterpret_cast<ThreadSwitch *>(userdata);
	ScriptServer::thread_enter(); //scripts may need to attach a stack

	t->id = (ID)(t->thread.handle); // get thread id
	t->callback(t->userdata);

	ScriptServer::thread_exit();
}

static int last_thread_id = 1;
GodotThread *ThreadSwitch::create_func_switch(ThreadCreateCallback callback, void *userdata, const Settings &)
{
	ThreadSwitch *thread = memnew(ThreadSwitch);
	thread->callback = callback;
	thread->userdata = userdata;
	threadCreate(&thread->thread, thread_func, (void*)thread, 0x1000, 0x3B, -2);
	threadStart(&thread->thread);
	return thread;
}

GodotThread::ID ThreadSwitch::get_thread_id_func_switch()
{
	return (GodotThread::ID)getThreadVars()->handle;
}

void ThreadSwitch::wait_to_finish_func_switch(GodotThread *p_thread)
{
	ThreadSwitch *t = reinterpret_cast<ThreadSwitch *>(p_thread);
	threadWaitForExit(&t->thread);
}

GodotThread::ID ThreadSwitch::get_id() const
{
	return id;
}

void ThreadSwitch::make_default()
{
	create_func = create_func_switch;
	get_thread_id_func = get_thread_id_func_switch;
	wait_to_finish_func = wait_to_finish_func_switch;
}

ThreadSwitch::ThreadSwitch()
{}

ThreadSwitch::~ThreadSwitch()
{}

#endif