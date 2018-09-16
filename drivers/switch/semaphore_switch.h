#pragma once
#if defined(HORIZON_ENABLED)

#include "os/semaphore.h"
#include <switch.h>
class SemaphoreSwitch : public GodotSemaphore {
	Semaphore semaphore;
	static GodotSemaphore *create_semaphore_switch();

public:
	virtual Error wait();
	virtual Error post();
	virtual int get() const;

	static void make_default();
	SemaphoreSwitch();

	~SemaphoreSwitch();
};

#endif