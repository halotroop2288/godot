#if defined(HORIZON_ENABLED)

#include <switch.h>
#include "mutex_switch.h"
#include "os/memory.h"

MutexSwitch::MutexSwitch()
{
	mutexInit(&mutex);
}

MutexSwitch::~MutexSwitch() {
}

void MutexSwitch::lock()
{
	mutexLock(&mutex);
}

void MutexSwitch::unlock()
{
	mutexUnlock(&mutex);
}

Error MutexSwitch::try_lock()
{
	return mutexTryLock(&mutex) ? OK : ERR_BUSY;
}

GodotMutex *MutexSwitch::create_func_switch(bool p_recursive)
{
	return memnew(MutexSwitch);
}

void MutexSwitch::make_default()
{
	create_func = create_func_switch;
}
#endif