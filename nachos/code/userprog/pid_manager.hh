#include "../threads/list.hh"
#include "../threads/thread.hh"
#include "syscall.h"

class PidManager
{
    private:
        List<Thread*> *Pids;
        SpaceId pid;
    public:
     PidManager();
     ~PidManager();
     SpaceId AddPid(Thread* t);
     Thread* GetThread(SpaceId);
     void RemovePid(SpaceId);
};
