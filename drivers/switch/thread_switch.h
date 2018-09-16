#pragma once
#if defined(HORIZON_ENABLED)

#include "os/thread.h"
#include "script_language.h"
#include <switch.h>

class ThreadSwitch : public GodotThread
{
	void *userdata;
	int id;
	ThreadCreateCallback callback;
	Thread thread;

	static GodotThread *create_thread_switch();

	static void thread_func(void* userdata);

	static GodotThread *create_func_switch(ThreadCreateCallback p_callback, void *userdata, const Settings &);
	static ID get_thread_id_func_switch();
	static void wait_to_finish_func_switch(GodotThread *p_thread);

	ThreadSwitch();

public:
	virtual ID get_id() const;
	static void make_default();

	~ThreadSwitch();
};

#endif