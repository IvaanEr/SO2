#include "../machine/console.hh"
#include "../threads/synch.hh"

class SynchConsole
{
public:
	SynchConsole(const char *readFile,const char *writeFile);
	~SynchConsole();

	static void SynchConsoleWriteDone(void*);
	static void SynchConsoleReadDone(void*);
  	void SynchPutChar(char c);
  	char SynchGetChar();

	static Semaphore *writeSem;
	static Semaphore *readSem;
private:
	Console *console;
	Lock *writeLock;
	Lock *readLock;
};
