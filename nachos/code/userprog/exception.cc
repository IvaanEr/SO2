/// Entry point into the Nachos kernel from user programs.
///
/// There are two kinds of things that can cause control to transfer back to
/// here from user code:
///
/// * System calls: the user code explicitly requests to call a procedure in
///   the Nachos kernel.  Right now, the only function we support is `Halt`.
///
/// * Exceptions: the user code does something that the CPU cannot handle.
///   For instance, accessing memory that does not exist, arithmetic errors,
///   etc.
///
/// Interrupts (which can also cause control to transfer from user code into
/// the Nachos kernel) are handled elsewhere.
///
/// For now, this only handles the `Halt` system call.  Everything else core
/// dumps.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2017 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "syscall.h"
#include "threads/system.hh"
#include "args.cc"
#include "pid_manager.hh"

PidManager *pidmanager = new PidManager();

void
IncrementPC()
{
  int pc = machine->ReadRegister(PC_REG);
  machine->WriteRegister(PREV_PC_REG, pc);
  pc = machine->ReadRegister(NEXT_PC_REG);
  machine->WriteRegister(PC_REG, pc);
  pc += 4; //cada instruccion es de 4bytes
  machine->WriteRegister(NEXT_PC_REG, pc);
}

void
StartProc(void *args)
{
    WriteArgs((char**)args); 
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
                      //funciona?

    machine->Run();
}

#define READSTR(add,out,max) ReadStringFromUser(add,out,max)
#define READBUFF(add,out,byteCount) ReadBufferFromUser(add,out,byteCount)
#define WRITESTR(str,add) WriteStringToUser(str,add)
#define WRITEBUFF(buff,add,byteCount) WriteBufferToUser(buff,add,byteCount)

//Machine::ReadMem(unsigned addr, unsigned size, int *value)
void
ReadStringFromUser(int userAddress, char *outString, unsigned maxByteCount)
{
	int c;
	unsigned i=0;
	
	do{
			ASSERT(machine->ReadMem(userAddress+i,1,&c));
			outString[i] = c;
			i++;
		} while(c != 0 && i<maxByteCount);
}

void
ReadBufferFromUser(int userAddress, char *outBuffer,unsigned byteCount)
{
	int c;
	unsigned i;

	for(i = 0; i<byteCount; i++){
		ASSERT(machine->ReadMem(userAddress+i,1,&c));
		outBuffer[i] = c;
	}
}

//Machine::WriteMem(unsigned addr, unsigned size, int value)
void
WriteStringToUser(const char *string, int userAddress)
{
	for (int i = 0; string[i] != 0; i++)
		ASSERT(machine->WriteMem(userAddress+i, 1, string[i])); //me fijo que sea distinto de 0 o de '\0'??????
}

void
WriteBufferToUser(const char *buffer, int userAddress,unsigned byteCount)
{
	for (unsigned i = 0; i< byteCount; i++)
		ASSERT(machine->WriteMem(userAddress+i, 1, buffer[i]));
}
/// Entry point into the Nachos kernel.  Called when a user program is
/// executing, and either does a syscall, or generates an addressing or
/// arithmetic exception.
///
/// For system calls, the following is the calling convention:
///
/// * system call code in `r2`;
/// * 1st argument in `r4`;
/// * 2nd argument in `r5`;
/// * 3rd argument in `r6`;
/// * 4th argument in `r7`;
/// * the result of the system call, if any, must be put back into `r2`.
///
/// And do not forget to increment the pc before returning. (Or else you will
/// loop making the same system call forever!)
///
/// * `which` is the kind of exception.  The list of possible exceptions is
///   in `machine.hh`.
void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if (which == SYSCALL_EXCEPTION) {
        switch(type){
        	
        	case SC_Halt:
		        DEBUG('a', "Shutdown, initiated by user program.\n");
		        interrupt->Halt();
		        IncrementPC();
		        break;        			
        	
        	case SC_Create:
        		{char name[128];
        		int r4 = machine->ReadRegister(4);
        		READSTR(r4,name,128);
        		if(!(fileSystem->Create(name,0))){
              printf("Couldn't create the file %s\n", name);
            }
            IncrementPC();
						break;
            }
        	case SC_Read://int Read(char *buffer, int size, OpenFileId id);
          { 
            int r4 = machine->ReadRegister(4);
            int size = machine->ReadRegister(5);
        		OpenFileId file_id = machine->ReadRegister(6);
						
            if (file_id == 0){ //ConsoleInput
              char st[size];
              int i;
              for(i = 0; i < size; i++)
                st[i] = synchConsole->SynchGetChar();
              
            WRITEBUFF(st,r4,size);
            machine->WriteRegister(2,i);

						}
						else{
						  char *buffer = new char[128];
            	OpenFile *f = currentThread->GetFile(file_id);
              if(f == NULL)
                printf("The file doesn't exist [Read] \n");
							int count = f->Read(buffer,size);
              WRITEBUFF(buffer,r4,size); 
              machine->WriteRegister(2,count);
              delete [] buffer;
						}
						IncrementPC();
						break;
          }
					case SC_Write://void Write(char *buffer, int size, OpenFileId id);
           {
            int r4 = machine->ReadRegister(4);            
            int size = machine->ReadRegister(5);						
            char *buff = new char[size];
            
            READBUFF(r4,buff,size);
            
						OpenFileId file_id = machine->ReadRegister(6);
           
						if (file_id == 1){
              int i;
              for(i = 0; i < size; i++)
                synchConsole->SynchPutChar(buff[i]);
            }

						else{
              OpenFile *f = currentThread->GetFile(file_id);
              if(f == NULL)
                 printf("The file doesn't exist [Write]\n");
              f->Write(buff,size);
            }
            delete [] buff;
            IncrementPC();
            break;
            }
          case SC_Open://OpenFileId Open(char *name);
            {
            char *name = new char[128];

            int r4 = machine->ReadRegister(4);
            READSTR(r4,name,128);
            printf("Name - r4 : %s\n",name);            
            int returnReg = 2;

            OpenFile *file = fileSystem->Open(name);
            if(file == NULL)
              printf("Could not open the file %s\n",name);

            OpenFileId id = currentThread->AddFile(file);
            machine->WriteRegister(returnReg,id);
            delete name;

            IncrementPC();
            break;
            }


          case SC_Close: //void Close(OpenFileId id);
            {OpenFileId id = machine->ReadRegister(4);
            OpenFile *file = currentThread->GetFile(id);
            if(file != NULL){ //OpenFile() //chequeamos que el archivo existe
              currentThread->RemoveFile(file);
              delete file;
            }
            else printf("The file doesn't exist [Close]\n");
            IncrementPC();
            break;
            }
    
          case SC_Exit:{
             currentThread->returnValue = machine->ReadRegister(4);
             pidmanager->RemovePid(currentThread);              
             currentThread->Finish();
             break;
          }

          case SC_Exec://SpaceId Exec(char *name, char **argv);
          {
            char name[128];
            int r4 = machine->ReadRegister(4);
            READSTR(r4,name,128);
            int r5 = machine->ReadRegister(5);
            
            char **argv = SaveArgs(r5);
            
            if(!argv){
                machine->WriteRegister(2,-1); //terminacion incorrecta
                IncrementPC();
                break;            
            }
            
            OpenFile *executable = fileSystem->Open(name);
            if(!executable){
                machine->WriteRegister(2,-1); //terminacion incorrecta
                IncrementPC();
                break;            
            }
           
            AddressSpace *space = new AddressSpace(executable);
            delete executable;

            Thread *t = new Thread(strdup(name),true); // new joinable thread
            t->space = space;
            
            SpaceId pid_hijo = pidmanager->AddPid(t);
            machine->WriteRegister(2,pid_hijo);
            
            t->Fork(StartProc,(void *)argv);

            
            IncrementPC();
            break;
          }
          case SC_Join:{
             SpaceId pid_hijo = machine->ReadRegister(4);
             Thread *t        = pidmanager->GetThread(pid_hijo);
             int ret          = t -> Join();
             machine->WriteRegister(2,ret);
             IncrementPC();
             break;
          }

            
         default:
            printf("Unexpected user mode exception Default %d %d\n", which, type);
            ASSERT(false);
        }
    } else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(false);
    }
}

