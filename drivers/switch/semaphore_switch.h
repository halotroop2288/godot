#pragma once
#if defined(HORIZON_ENABLED)

#include "os/semaphore.h"
#include "switch_wrapper.h"

class SemaphoreSwitch : public Semaphore {
	LibnxSemaphore semaphore;
	static Semaphore *create_semaphore_switch();

public:
	virtual Error wait();
	virtual Error post();
	virtual int get() const;

	static void make_default();
	SemaphoreSwitch();

	~SemaphoreSwitch();
};

#endif