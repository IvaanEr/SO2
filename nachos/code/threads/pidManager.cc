#include "pid_manager.hh"


PidManager::PidManager()
{
    Pids = new List<Thread*>;
    pid = 1;
    lock = new Lock("PidManager Lock");
}

PidManager::~PidManager(){
    delete Pids;
}

SpaceId
PidManager::AddPid(Thread* t)
{
    lock->Acquire();

    Pids->SortedInsert(t,pid);
    int aux = pid;
    pid++;

    lock->Release();
    return aux;
}

Thread*
PidManager::GetThread(SpaceId id)
{
    Thread* ret = Pids->Find(id);
    ASSERT(ret != NULL);
    return ret;
}

void
PidManager::RemovePid(Thread* t)
{
    if(t != NULL)
    Pids->RemoveItem(t);
}
