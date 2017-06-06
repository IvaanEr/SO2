
#include "../machine/console.hh"

class SynchConsole
{
public:
	SynchConsole(const char *readFile,const char *writeFile);
	~SynchConsole();

	static void SynchConsoleWriteDone(void*);
	static void SynchConsoleReadDone(void*);
  void SynchPutChar(char c);
  char SynchGetChar();

private:
	Console *console;
	static Semaphore *writeSem;
	static Semaphore *readSem;
  Lock *lock;
}

SynchConsole::SynchConsole(const char *readFile, const char *writeFile)
{
  console = new Console(readFile,writeFile,SynchConsole::SynchConsoleWriteDone,SynchConsole::SynchConsoleReadDone,0);
  writeSem = new Semaphore("writeSem",0);
  readSem = new Semaphore("readSem",0);
  lock = new Lock("lock");
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
  lock->Acquire();

  console->PutChar(c);
  writeSem -> P();

  lock->Release();  
}

char
SynchGetChar()
{ //OTRO LOCK!!! ----------
  lock->Acquire();
  readSem->P();
  char c = console->GetChar();
  lock->Release();
  return c;
}