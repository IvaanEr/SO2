#include "pid_manager.hh"


PidManager::PidManager()
{
    Pids = new List<Thread*>;
    pid = 1;
}

PidManager::~PidManager(){
    delete Pids;
}

SpaceId
PidManager::AddPid(Thread* t)
{
    Pids->SortedInsert(t,pid);
    
    int aux = pid;
    pid++;
    return aux;
}

Thread*
PidManager::GetThread(SpaceId id)
{
    Thread* ret = Pids->SortedRemove(&id);
    Pids->SortedInsert(ret,id);
    return ret;
}

void
PidManager::RemovePid(SpaceId id)
{
    Pids->SortedRemove(&id);
}
