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
  Lock *writeLock;
  Lock *readLock;
}

