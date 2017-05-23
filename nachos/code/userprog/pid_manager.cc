#include "pid_manager.hh"


PidManager::PidManager(){
    Pids = new List<Thread*>;
    pid = 1;
}

PidManager::~PidManager(){
    delete Pids;
}
SpaceId AddPid(Thread* t);
     Thread* GetThread(SpaceId);
     void RemovePid(SpaceId);
