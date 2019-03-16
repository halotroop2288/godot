#if defined(HORIZON_ENABLED)

#include "rwlock_switch.h"
#include "os/memory.h"

bool rwlockTryReadLock(RwLock* r)
{
	bool success = false;
	rmutexLock(&r->r);

    if (r->b == 0)
    {
    	r->b++;
    	success = true;
        rmutexLock(&r->g);
    }

    rmutexUnlock(&r->r);
    return success;
}

bool rwlockTryWriteLock(RwLock* r)
{
    return rmutexTryLock(&r->g);
}

RWLock* RWLockSwitch::create_func_switch()
{
	return memnew(RWLockSwitch);
}

void RWLockSwitch::read_lock()
{
	rwlockReadLock(&rwlock);
}

void RWLockSwitch::read_unlock()
{
	rwlockReadUnlock(&rwlock);
}

Error RWLockSwitch::read_try_lock()
{
	return rwlockTryReadLock(&rwlock) ? OK : ERR_BUSY;
}

void RWLockSwitch::write_lock()
{
	rwlockWriteLock(&rwlock);
}

void RWLockSwitch::write_unlock()
{
	rwlockWriteUnlock(&rwlock);
}

Error RWLockSwitch::write_try_lock()
{
	return rwlockTryWriteLock(&rwlock) ? OK : ERR_BUSY;
}

void RWLockSwitch::make_default()
{
	create_func = create_func_switch;
}

RWLockSwitch::RWLockSwitch()
{
	rwlockInit(&rwlock);
}

RWLockSwitch::~RWLockSwitch() {
}


#endif