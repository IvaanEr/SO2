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

#define MAX_LONG_NAME 128

#define READSTR(add,out,max) ReadStringFromUser(add,out,max)
#define READBUFF(add,out,byteCount) ReadBufferFromUser(add,out,byteCount)
#define WRITESTR(str,add) WriteStringToUser(str,add)
#define WRITEBUFF(buff,add,byteCount) WriteBufferToUser(buff,add,byteCount)


void insert_translation_entry (TranslationEntry translation_entry){
    int i;
    for(i = 0; i < TLB_SIZE; i++){
      if(!machine->tlb[i].valid){
        machine->tlb[i] = translation_entry;
        return;
      }
    }
    i = rand() % TLB_SIZE;
    ASSERT(0 <= i && i < TLB_SIZE);
    currentThread->space->copyPage(i, machine->tlb[i].virtualPage);
    machine->tlb[i] = translation_entry;
}

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
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    WriteArgs((char**)args);
    machine->Run();
}

//Machine::ReadMem(unsigned addr, unsigned size, int *value)
void
ReadStringFromUser(int userAddress, char *outString, unsigned maxByteCount)
{
	int c;
	unsigned i=0;

	do{
      if(!machine->ReadMem(userAddress+i,1,&c)){
        ASSERT(machine->ReadMem(userAddress+i,1,&c));
      }      // DEBUG('p', "Estamos en ReadString. Leimos: %c\n", c);
			outString[i] = (char) c;
			i++;
		} while(c != '\0' && i<maxByteCount);
}



void
ReadBufferFromUser(int userAddress, char *outBuffer, unsigned byteCount)
{
	int c;
	unsigned i;

	for(i = 0; i<byteCount; i++){
    
		if(!machine->ReadMem(userAddress+i, 1, &c)){
      ASSERT(machine->ReadMem(userAddress+i, 1, &c));
    }
		outBuffer[i] = c;
	}
}

//Machine::WriteMem(unsigned addr, unsigned size, int value)
void
WriteStringToUser(const char *string, int userAddress)
{
  unsigned i=0;
  do {
    if(!machine->WriteMem(userAddress+i, 1, string[i])){
		  ASSERT(machine->WriteMem(userAddress+i, 1, string[i]));
    }
    i++;
  } while(string[i-1] != '\0'); // Primero copio y después comparo para no dejar afuera al terminador
}

void
WriteBufferToUser(const char *buffer, int userAddress,unsigned byteCount)
{
	for (unsigned i = 0; i < byteCount; i++)
    if(!machine->WriteMem(userAddress+i, 1, buffer[i])){
      ASSERT(machine->WriteMem(userAddress+i, 1, buffer[i]));
    }
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
          {
		        DEBUG('p', "Shutdown, initiated by user program.\n");
		        interrupt->Halt();
		        break;
          }
        	case SC_Create:
      		{
            char name[MAX_LONG_NAME];
        		int nameReg = machine->ReadRegister(4); // Leo el nombre que viene en registro
        		READSTR(nameReg, name, MAX_LONG_NAME);

        		if(fileSystem -> Create(name,0)){
              DEBUG('p', "New file: %s\n", name);
            } else {
              DEBUG('p', "Error while creating file: %s\n", name);
            }

						break;
          }
        	case SC_Read://int Read(char *buffer, int size, OpenFileId id);
          {
            int user_buffer = machine -> ReadRegister(4);
            int size = machine -> ReadRegister(5);
        		OpenFileId file_id = machine -> ReadRegister(6);

            char my_buffer[size];

            if (file_id == 0) { //ConsoleInput
              // DEBUG('p', "Reading from console...");

              for(int i = 0; i < size; i++) {
                my_buffer[i] = synchConsole->SynchGetChar();
              }

              WRITEBUFF(my_buffer, user_buffer, size);
              machine->WriteRegister(2,size); // Devuelvo la cantidad de bytes leídos
            } else {
            	OpenFile *file = currentThread->GetFile(file_id);

              if(file) {
  							int readBytes = file -> Read(my_buffer,size);
                DEBUG('p', "Reading %d bytes from file id: %d\n", readBytes, file_id);
                WRITEBUFF(my_buffer,user_buffer,readBytes);
                machine->WriteRegister(2,readBytes);
              }
              else {
                DEBUG('p', "[Error] Trying to read from invalid file id: %d\n", file_id);
                machine->WriteRegister(2,-1); // Devuelvo -1 para indicar el error
              }
						}
						break;
          }
					case SC_Write://void Write(char *buffer, int size, OpenFileId id);
          {
            int user_buffer = machine->ReadRegister(4);
            int size = machine->ReadRegister(5);
						OpenFileId file_id = machine->ReadRegister(6);

            char my_buffer[size];

            READBUFF(user_buffer, my_buffer, size);


						if (file_id == 1){
              // DEBUG('p', "Writing to console...");

              for(int i = 0; i < size; i++)
                synchConsole -> SynchPutChar(my_buffer[i]);

            } else {
              OpenFile *file = currentThread -> GetFile(file_id);
              if(file) {
                DEBUG('p', "Writing %d bytes from file id: %d\n", size, file_id);

                file -> Write(my_buffer, size);
              } else {
                DEBUG('p', "[Error] Trying to write invalid file id: %d\n", file_id);
              }
            }
            break;
          }
          case SC_Open://OpenFileId Open(char *name);
          {
            int nameReg = machine -> ReadRegister(4);
            char name[MAX_LONG_NAME];

            READSTR(nameReg, name, MAX_LONG_NAME);

            OpenFile *file = fileSystem -> Open(name);

            if(file) {
              DEBUG('p', "Opening file: %s\n", name);
              OpenFileId file_id = currentThread->AddFile(file);
              machine->WriteRegister(2,file_id);
            } else {
              DEBUG('p', "[Error] Could not open file: %s\n", name);
              machine->WriteRegister(2,-1);
            }

            break;
          }

          case SC_Close: //void Close(OpenFileId id);
          {
            OpenFileId file_id = machine->ReadRegister(4);
            OpenFile *file = currentThread->GetFile(file_id);
            if(file) { //OpenFile() //chequeamos que el archivo existe
              DEBUG('p', "Closing file with id: %d\n", file_id);
              currentThread->RemoveFile(file);
              delete file;
            } else
              DEBUG('p', "[Error] Could not close file with id: %d\n", file_id);
            break;
          }

          case SC_Exit:
          {
            //  printf("Exit 1\n");
             int end_code = machine -> ReadRegister(4);
            //  printf("Exit 2 %d\n", end_code);
             // Agrego esto para debuggin pero es costo computacional al pedo
             SpaceId end_id = pidManager -> GetPid(currentThread);
             //////////////
              // printf("Exit 3\n");
             DEBUG('p', "Process with pid %d exiting with status code: %d\n", end_id, end_code);
             currentThread -> returnValue = end_code;
            //  printf("estoy por stats->print()\n\n");
            //  stats -> Print();
             currentThread -> Finish();
             break;
          }

          case SC_Exec: //SpaceId Exec(char *name, char **argv);
          {
            char *name = new char[MAX_LONG_NAME];
            int nameReg = machine->ReadRegister(4);
            DEBUG('e', "Register 4: %d\n", nameReg);
            int argsAdress = machine->ReadRegister(5);
            DEBUG('e', "Register 5: %d\n", argsAdress);

            READSTR(nameReg, name, MAX_LONG_NAME);
            OpenFile *exe = fileSystem->Open(name);

            if (exe) {
              char **argv = SaveArgs(argsAdress);
              ASSERT(argv); // Chequeo que haya argumentos

              Thread *exe_thread = new Thread(strdup(name), true, 9);
              exe_thread -> Fork(StartProc, argv);
              SpaceId pid_hijo = pidManager->AddPid(exe_thread);

              AddressSpace *exe_space =  new AddressSpace(exe);
              exe_thread -> space = exe_space;

              DEBUG('p', "Executing binary %s with id %d\n", name, pid_hijo);
              machine->WriteRegister(2, pid_hijo);
              
            } else {
              DEBUG('p', "[Error] Could not open executable: %s\n", name);
              machine->WriteRegister(2, -1);
            }

            break;
          }

          case SC_Join:
          {
             SpaceId pid_hijo = machine -> ReadRegister(4);
             Thread *t        = pidManager -> GetThread(pid_hijo);
             const char *name = t->getName();
            //  printf("Joining %s \n", name);
             if (t) {
              //  printf("entre el if\n");
               DEBUG('p', "Waiting for process %d to finish\n", pid_hijo);
               int end_code = t -> Join();
               pidManager -> RemovePid(t);
               machine -> WriteRegister(2, end_code);
           //  DEBUG('p', "Process %d returned %d\n", pid_hijo, end_code);
             } else {

               DEBUG('p', "[Error] Could not join process %d\n", pid_hijo);
               machine -> WriteRegister(2, -1);
             }

             break;
          }

         default:
            printf("Unexpected type of syscall exception %d %d\n", which, type);
            ASSERT(false);
        }
      IncrementPC();
    }
    else if (which == PAGE_FAULT_EXCEPTION) {
      DEBUG('a', "Page Fault Exception \n");
      int virtual_addr = machine->ReadRegister(BAD_VADDR_REG);
      int vpn = virtual_addr / PAGE_SIZE;

      if(virtual_addr < 0 || virtual_addr >= (currentThread->space->getNumPages() * PAGE_SIZE)){
        printf("fuera de rango\n");
        machine->RaiseException(ADDRESS_ERROR_EXCEPTION, virtual_addr);
      }
      else {
        #ifdef USE_DML
        if(currentThread->space->getPageTable(vpn).physicalPage == -1)
            currentThread->space->LoadPage(virtual_addr);
        #endif
        #ifdef USE_TLB
        TranslationEntry te = currentThread->space->getPageTable(vpn);
        insert_translation_entry(te);
        #endif

      }
    }
    else {
         printf("Unexpected user mode exception %d %d\n", which, type);
         ASSERT(false);
    }
}
