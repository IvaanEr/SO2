#include "SynchConsole.hh"

Semaphore *SynchConsole::writeSem = new Semaphore("writeSem",0);
Semaphore *SynchConsole::readSem = new Semaphore("readSem",0);

SynchConsole::SynchConsole(const char *readFile, const char *writeFile)
{
  console = new Console(readFile,writeFile,SynchConsole::SynchConsoleReadDone,SynchConsole::SynchConsoleWriteDone,0);
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

void
SynchConsole::SynchConsoleWriteDone(void* arg)
{
  writeSem -> V();
}

void
SynchConsole::SynchConsoleReadDone(void* arg)
{
  readSem -> V();
}

void
SynchConsole::SynchPutChar(char c)
{
  writeLock->Acquire();

  console->PutChar(c);
  writeSem->P();

  writeLock->Release();  
}

char
SynchConsole::SynchGetChar()
{ 
  readLock->Acquire();
  
  readSem->P();
  char c = console->GetChar();

  readLock->Release();
  return c;
}
