#pragma once
#if defined(HORIZON_ENABLED)
#include "os/mutex.h"
#include <switch.h>

class MutexSwitch : public GodotMutex {
	Mutex mutex;
	static GodotMutex *create_func_switch(bool p_recursive);

public:
	virtual void lock();
	virtual void unlock();
	virtual Error try_lock();

	static void make_default();

	MutexSwitch();
	~MutexSwitch();
};

#endif