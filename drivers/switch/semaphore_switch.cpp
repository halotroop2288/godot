#if defined(HORIZON_ENABLED)

#include "semaphore_switch.h"
#include "os/memory.h"
#include <switch.h>

Error SemaphoreSwitch::wait()
{
	semaphoreWait(&semaphore);
	return OK;
}

Error SemaphoreSwitch::post()
{
	semaphoreSignal(&semaphore);
	return OK;
}

int SemaphoreSwitch::get() const
{
	return (int)semaphore.count;
}

void SemaphoreSwitch::make_default()
{
	create_func = create_semaphore_switch;
}

GodotSemaphore *SemaphoreSwitch::create_semaphore_switch()
{
	return memnew(SemaphoreSwitch);
}

SemaphoreSwitch::SemaphoreSwitch()
{
	semaphoreInit(&semaphore, 0);
}

SemaphoreSwitch::~SemaphoreSwitch()
{}
#endif