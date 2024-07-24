#include "platform_thread.h"

#include "core/object/script_language.h"
#include "core/templates/safe_refcount.h"
#include <bitset>

Thread::PlatformFunctions Thread::platform_functions;

uint64_t Thread::_thread_id_hash(const Handle &p_t) {
	u32 id;
	svcGetThreadId(&id, p_t);
	static std::hash<u32> hasher;
	return hasher(p_t);
}

Thread::ID Thread::main_thread_id = _thread_id_hash(_thread_id_hash(CUR_THREAD_HANDLE));
thread_local Thread::ID Thread::caller_id = 0;

void Thread::_set_platform_functions(const PlatformFunctions &p_functions) {
	platform_functions = p_functions;
}

void Thread::callback(ID p_caller_id, const Settings &p_settings, Callback p_callback, void *p_userdata) {
	Thread::caller_id = p_caller_id;
	if (platform_functions.set_priority) {
		platform_functions.set_priority(p_settings.priority);
	}
	if (platform_functions.init) {
		platform_functions.init();
	}
	ScriptServer::thread_enter(); // Scripts may need to attach a stack.
	if (platform_functions.wrapper) {
		platform_functions.wrapper(p_callback, p_userdata);
	} else {
		p_callback(p_userdata);
	}
	ScriptServer::thread_exit();
	if (platform_functions.term) {
		platform_functions.term();
	}
}

void Thread::start(Thread::Callback p_callback, void *p_user, const Settings &p_settings) {
	if (id != _thread_id_hash(0)) {
#ifdef DEBUG_ENABLED
		WARN_PRINT("A Thread object has been re-started without wait_to_finish() having been called on it. Please do so to ensure correct cleanup of the thread.");
#endif
		threadDetach(thread);
		CTRThread empty_thread{ nullptr };
		thread = empty_thread;
	}

	CTRThread new_thread = threadCreate(p_callback, p_user, STACKSIZE, 0x30, -1, false);
	thread = new_thread;
	id = _thread_id_hash(threadGetHandle(thread));
}

bool Thread::is_started() const {
	return id != _thread_id_hash(0);
}

void Thread::wait_to_finish() {
	if (id != _thread_id_hash(0)) {
		ERR_FAIL_COND_MSG(id == get_caller_id(), "A Thread can't wait for itself to finish.");
		threadJoin(thread, U64_MAX);
		CTRThread empty_thread{ nullptr };
		thread = empty_thread;
		id = _thread_id_hash(0);
	}
}

Error Thread::set_name(const String &p_name) {
	if (platform_functions.set_name) {
		return platform_functions.set_name(p_name);
	}

	return ERR_UNAVAILABLE;
}

Thread::Thread() {
	u32 id;
	svcGetThreadId(&id, CUR_THREAD_HANDLE);
	caller_id = _thread_id_hash(id);
}

Thread::~Thread() {
	if (id != _thread_id_hash(0)) {
#ifdef DEBUG_ENABLED
		WARN_PRINT("A Thread object has been destroyed without wait_to_finish() having been called on it. Please do so to ensure correct cleanup of the thread.");
#endif
		threadDetach(thread);
	}
}