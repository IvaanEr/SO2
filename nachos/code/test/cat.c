#include "../userprog/syscall.h"

int main(int argc, char ** argv){
	if(argc != 2){
		Write("Syntax Error.",13,ConsoleOutput);
	}
	OpenFileId source = Open(argv[1]);
	char c;

	while(Read(&c, 1, source)){
		Write(&c,1,ConsoleOutput);
	}
	Close(source);
}
