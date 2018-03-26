#include "pidManager.hh"


PidManager::PidManager()
{
  lock = new Lock("PidManager Lock");
  for (int i = 0; i < MAX_NUMBER_PROC; i++)
    table[i] = NULL;
}

PidManager::~PidManager(){
  delete lock;
}

SpaceId
PidManager::AddPid(Thread* process)
{
    int res = -1;

    lock->Acquire();

    for(int i=0; i<MAX_NUMBER_PROC; i++){
      if(table[i]==NULL){
          table[i]=process;
          res = i;
          break;
      }
    }

    lock->Release();
    return res;
}

Thread*
PidManager::GetThread(SpaceId id)
{
  if (id >= 0 && id < MAX_NUMBER_PROC) {
    ASSERT(table[id] != NULL);
    return table[id];
  }
  else{
    ASSERT(false);
    return NULL;
  }
}

void
PidManager::RemovePid(Thread* process)
{
    int i;

    lock -> Acquire();
    for(i=0; i<MAX_NUMBER_PROC; i++){
      if(table[i]==process){
        table[i] = NULL;
        break;
      }
    }
    if(i==MAX_NUMBER_PROC)
      ASSERT(false);
    lock -> Release();
}

SpaceId
PidManager::GetPid(Thread *process)
{
  int res = -1;
  for(int i=0; i<MAX_NUMBER_PROC; i++){
      if(table[i]==process){
        res = i;
        break;
      }
    }
  return res;
}
