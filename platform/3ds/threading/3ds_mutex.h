#ifndef N3DS_MUTEX_H
#define N3DS_MUTEX_H
#define Thread CTRThread
#include <3ds.h>
#undef Thread
#include <atomic>
#include <functional>
namespace std {
class condition_variable;
class mutex {
    private:
        LightLock _lock;
    public:
        mutex(){
            LightLock_Init(&_lock);
        }
        void lock() {
            LightLock_Lock(&_lock);
        }
        bool try_lock() {
            return !LightLock_TryLock(&_lock);
        }
        void unlock() {
            LightLock_Unlock(&_lock);
        }
        friend condition_variable;
};

class recursive_mutex {
    private:
        RecursiveLock _lock;
    public:
        recursive_mutex(){
            RecursiveLock_Init(&_lock);
        }
        void lock() {
            RecursiveLock_Lock(&_lock);
        }
        bool try_lock() {
            return !RecursiveLock_TryLock(&_lock);
        }
        void unlock() {
            RecursiveLock_Unlock(&_lock);
        }
        friend condition_variable;
};

class shared_timed_mutex {
    atomic<int> refcount{0};
    LightLock _lock;
public:
    shared_timed_mutex() {
        LightLock_Init(&_lock);
    }
    void lock() // write lock
    {
        int val;
        do {
            val = 0; // Can only take a write lock when refcount == 0

        } while (!refcount.compare_exchange_weak(val, -1, std::memory_order_acquire));
        LightLock_Lock(&_lock);
        // can memory_order_relaxed be used if only a single thread takes write locks ?
    }

    void unlock() // write unlock
    {
        refcount.store(0, std::memory_order_release);
        LightLock_Unlock(&_lock);
    }

    void lock_shared() // read lock
    {
        int val;
        do {
            do {
                val = refcount.load(std::memory_order_relaxed);

            } while (val == -1); // spinning until the write lock is released

        } while (!refcount.compare_exchange_weak(val, val+1, std::memory_order_acquire));
        LightLock_Lock(&_lock);
    }

    bool try_lock() {
        return !LightLock_TryLock(&_lock);
    }
    bool try_lock_shared() // read lock
    {
        int val;
        do {
            do {
                val = refcount.load(std::memory_order_relaxed);

            } while (val == -1); // spinning until the write lock is released

        } while (!refcount.compare_exchange_weak(val, val+1, std::memory_order_acquire));
        return !LightLock_TryLock(&_lock);
    }

    void unlock_shared() // read unlock
    {
        // This must be a release operation (see answer)
        refcount.fetch_sub(1, std::memory_order_release);
        LightLock_Unlock(&_lock);
    }  
};

class condition_variable
{
    private:
    CondVar condition_variable;
    public:
    void wait(mutex &__lock)
    {
        CondVar_Wait(&condition_variable,&__lock._lock);
    }
    void wait(mutex &__lock,std::function<void()> lambda)
    {
        wait(__lock);
        lambda();
    }
    void notify_one()
    {
        CondVar_Signal(&condition_variable);
    }
};
}
#endif