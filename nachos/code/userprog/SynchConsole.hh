
#include "../machine/console.hh"

class SynchConsole
{
public:
	SynchConsole(const char *readFile,const char *writeFile);
	~SynchConsole();

	static void SynchConsoleWriteDone(void*);
	static void SynchConsoleReadDone(void*);

private:
	Console *console;
	Semaphore *writeSem;
	Semaphore *readSem;


}