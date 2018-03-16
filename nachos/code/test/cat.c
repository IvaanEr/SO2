#include "../userprog/syscall.h"

int main(int argc, char ** argv){
	if(argc != 2){
		Write("Syntax Error -cat-\n",19,ConsoleOutput);
		Exit(-1);
	}
	OpenFileId source = Open(argv[1]);
  Write("12345",5,ConsoleOutput);
	char c;

  while(Read(&c, 1, source)){
		Write(&c,1,ConsoleOutput);
	}
	Close(source);
}
