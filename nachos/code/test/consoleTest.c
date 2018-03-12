
#include "syscall.h"

int main(){
	char **args1 = {"PrintA", 0};
	char **args2 = {"PrintB", 0};
	char **args3 = {"PrintC", 0};
	int i = Exec("../test/PrintA",args1);
	int j = Exec("../test/PrintB",args2);
	int k = Exec("../test/PrintC",args3);

	Write("Hola Pelotudo\n",14,ConsoleOutput);
	
	// Join(i);

	Exit(0);
}