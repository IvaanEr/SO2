/// Simple program to test whether running a user program works.
///
/// Just do a “syscall” that shuts down the OS.
///
/// NOTE: for some reason, user programs with global data structures
/// sometimes have not worked in the Nachos environment.  So be careful out
/// there!  One option is to allocate data structures as automatics within a
/// procedure, but if you do this, you have to be careful to allocate a big
/// enough stack to hold the automatics!


#include "syscall.h"


int
main(void)
{
	char c[128];

    Create("test.txt");
    Create("test2.txt");          //char buf[128]; Read(buff,128,fd); devuelve cuanto leyo efectivamnete...
    int o = Open("test.txt");
    int a = Open("test2.txt");
    Write("Hello world\n",12,o);
  	Write("Hola\n",5,a);
  	
	Write("Chauuu\n",7,o);
  	Write("Chauuu\n",7,a);
  	// devuelve void.
    Close(o);
    Close(a);
    Exit(0);
    // Not reached.
}
