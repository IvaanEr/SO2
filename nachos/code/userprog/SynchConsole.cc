#include "SynchConsole.hh"
#include "../threads/synch.hh"

SynchConsole::SynchConsole(const char *readFile, const char *writeFile)
{
  console = new Console(readFile,writeFile,SynchConsole::SynchConsoleWriteDone,SynchConsole::SynchConsoleReadDone,0);
  writeSem = new Semaphore("writeSem",0);
  readSem = new Semaphore("readSem",0);
  writeLock = new Lock("writeLock");
  readLock = new Lock("readLock");
}

SynchConsole::~SynchConsole()
{
  delete console;
  delete writeSem;
  delete readSem;
  delete writeLock;
  delete readLock;
}

static void
SynchConsole::SynchConsoleWriteDone(void* arg)
{
  writeSem -> V();
}

static void
SynchConsole::SynchConsoleReadDone(void* arg)
{
  readSem -> V();
}

void
SynchPutChar(char c)
{
  writeLock->Acquire();

  console->PutChar(c);
  writeSem -> P();

  writeLock->Release();  
}

char
SynchGetChar()
{ 
  readLock->Acquire();
  
  readSem->P();
  char c = console->GetChar();

  readLock->Release();
  return c;
}
