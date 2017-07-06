#include "../userprog/syscall.h"

int main(){
	char c;
	char n = '\n';
	Read(&c,1,ConsoleInput);
	//Write("Hola Mundo!!\n",13,ConsoleOutput);
	Write(&c,1,ConsoleOutput);
	Write(&n,1,ConsoleOutput);
	Write("Son todos putos\n",17,ConsoleOutput)
;	Exit(0);
}