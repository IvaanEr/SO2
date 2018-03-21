
#include "syscall.h"

int main(){
  for(int i = 0; i < 1000; i++)
		Write("A",1,ConsoleOutput);
  Write("\n",1,ConsoleOutput);
  Exit(1);
}
