#include "../threads/list.hh"
#include "../threads/thread.hh"
#include "syscall.h"
#include "synch.hh"

class PidManager
{
    private:
        List<Thread*> *Pids;
        SpaceId pid;
        Lock *lock;
    public:
     PidManager();
     ~PidManager();
     SpaceId AddPid(Thread* t);
     Thread* GetThread(SpaceId);
     void RemovePid(Thread* t);
};
