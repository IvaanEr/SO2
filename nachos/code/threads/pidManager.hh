#include "../threads/thread.hh"
#include "syscall.h"
#include "synch.hh"

#define MAX_NUMBER_PROC 100

class PidManager
{
    private:
        Thread *table[MAX_NUMBER_PROC];
        Lock *lock;
    public:
     PidManager();
     ~PidManager();
     SpaceId AddPid(Thread* t);
     Thread* GetThread(SpaceId);
     void RemovePid(Thread* t);
};
