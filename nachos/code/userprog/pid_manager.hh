#include "../thread/list.hh"
#include "syscall.h"

class PidManager
{
    private:
        List<Thread*> *Pids;
        SpaceId pid = 1;
    public:
     SpaceId AddPid(Thread* t);
     Thread* GetThread(SpaceId);
     void RemovePid(SpaceId);
}
