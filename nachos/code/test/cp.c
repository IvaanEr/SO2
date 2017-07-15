
#include "../userprog/syscall.h"

int
main(int argc, char **argv)
{
	if(argc != 3){
		Write("Syntax Error -cp- \n",19,ConsoleOutput);
	}
	
	OpenFileId source  = Open(argv[1]);
	Create(argv[2]); // el filesys que usamos, que hace si ya existe?
	OpenFileId destiny = Open(argv[2]);

	char c;
	while(Read(&c,1,source))
		Write(&c,1,destiny);

	Close(source);
	Close(destiny);
}