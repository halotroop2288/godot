#define Mutex LibnxMutex
#define Semaphore LibnxSemaphore
#define Thread LibnxThread
#define AudioDriver LibnxAudioDriver
#include <switch.h>
#undef AudioDriver
#undef Thread
#undef Mutex
#undef Semaphore
