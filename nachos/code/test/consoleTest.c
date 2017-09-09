
#include "syscall.h"

int main(){
	// Write("entre",5,ConsoleOutput);
	char **args1 = {"PrintA",0};
	char **args2 = {"PrintB",0};
	// Write("llegue\n",7,ConsoleOutput);	
	Exec("../test/PrintA",args1);
	Exec("../test/PrintB",args2);
}