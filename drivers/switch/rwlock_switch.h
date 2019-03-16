#pragma once
#if defined(HORIZON_ENABLED)

#include "switch_wrapper.h"
#include "os/rw_lock.h"

class RWLockSwitch : public RWLock {
	RwLock rwlock;

	static RWLock *create_func_switch();

public:
	virtual void read_lock();
	virtual void read_unlock();
	virtual Error read_try_lock();

	virtual void write_lock();
	virtual void write_unlock();
	virtual Error write_try_lock();

	static void make_default();

	RWLockSwitch();

	~RWLockSwitch();
};

#endif