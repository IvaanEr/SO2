/// Routines to manage address spaces (executing user programs).
///
/// In order to run a user program, you must:
///
/// 1. Link with the `-N -T 0` option.
/// 2. Run `coff2noff` to convert the object file to Nachos format (Nachos
///    object code format is essentially just a simpler version of the UNIX
///    executable object code format).
/// 3. Load the NOFF file into the Nachos file system (if you have not
///    implemented the file system yet, you do not need to do this last
///    step).
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2017 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  S ee `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "address_space.hh"
#include "bin/noff.h"
#include "threads/system.hh"

BitMap *AddressSpace::bitmap = new BitMap(NUM_PHYS_PAGES);
/// Do little endian to big endian conversion on the bytes in the object file
/// header, in case the file was generated on a little endian machine, and we
/// are re now running on a big endian machine.
static void
SwapHeader(NoffHeader *noffH)
{
    noffH->noffMagic              = WordToHost(noffH->noffMagic);
    noffH->code.size              = WordToHost(noffH->code.size);
    noffH->code.virtualAddr       = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr        = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size          = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr   = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr    = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size        = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr =
      WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr  = WordToHost(noffH->uninitData.inFileAddr);
}

/// Create an address space to run a user program.
///
/// Load the program from a file `executable`, and set everything up so that
/// we can start executing user instructions.
///
/// Assumes that the object code file is in NOFF format.
///
/// First, set up the translation from program memory to physical memory.
/// For now, this is really simple (1:1), since we are only uniprogramming,
/// and we have a single unsegmented page table.
///
/// * `executable` is the file containing the object code to load into
///   memory.

unsigned 
AddressSpace::getNumPages()
{
    return numPages;
}

TranslationEntry 
AddressSpace::getPageTable(int i)
{
    return pageTable[i];
}

void
AddressSpace::copyPage(int i, int virtual_address)
{
    pageTable[virtual_address] = machine->tlb[i];
};

void
AddressSpace::LoadPage(int vaddr)
{
    ASSERT(exe);
    // DEBUG('z',"Loading segment from addr: %u\n",vaddr);
    Segment segment;
    bool flag = false;
    if((vaddr >= noffH.code.virtualAddr) && (vaddr <= noffH.code.virtualAddr + noffH.code.size)){ 
        //Code segment
        segment = noffH.code;
        //DEBUG('z',"Address: [%u] was found to be in code\n", vaddr);
    } else if((vaddr >= noffH.initData.virtualAddr) && (vaddr <= noffH.initData.virtualAddr + noffH.initData.size)){ 
        //InitData segment
        segment = noffH.initData;
        //DEBUG('z',"Address: [%u] was found to be in initData\n", vaddr);
    } else { 
        //UninitData segment
        segment = noffH.uninitData;
        flag = true;
        //DEBUG('z',"Address: [%u] was found to be in uninitData\n", vaddr);
    }
    
    int vpn = vaddr / PAGE_SIZE;

    int ppn = bitmap->Find();

    ASSERT(ppn >= 0);
    pageTable[vpn].physicalPage = ppn;
    int pp  = ppn * PAGE_SIZE;

    // printf("LoadPage #####\n");
    // // exe->Caca();
    // int a = AddressSpace::exe->Length();
    // printf("LoadPage %d\n", a);


    for (int j = 0; (j < (int)PAGE_SIZE) && (j < exe->Length() - vaddr - 40); j++){
        char c;

        if(!flag){ // Load the data
            int res = exe->ReadAt(&c, 1, j + segment.inFileAddr + vpn * PAGE_SIZE - segment.virtualAddr);
            ASSERT(res == 1);
        } else { // Fill the uninitialized data zone with zero's
            c = (char)0;
        }

        int paddr  = pp + j;
        machine->mainMemory[paddr] = c;
    }

    pageTable[vpn].valid = true;
}

AddressSpace::AddressSpace(OpenFile *executable)
{
    unsigned   size;
    ASSERT(executable);
    // Save the executable for later
    exe = executable;

    //int a = exe -> Length();
    //printf("AddressSpace::Exe Length %d\n\n", a);

    // AddressSpace::bitmap -> Print();

    exe->ReadAt((char *) &noffH, sizeof noffH, 0);

    if (noffH.noffMagic != NOFFMAGIC &&
          WordToHost(noffH.noffMagic) == NOFFMAGIC)
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    // How big is address space?

    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
           + USER_STACK_SIZE;
      // We need to increase the size to leave room for the stack.
    numPages = divRoundUp(size, PAGE_SIZE); 
    size = numPages * PAGE_SIZE;

    ASSERT(numPages <= NUM_PHYS_PAGES);
      // Check we are not trying to run anything too big -- at least until we
      // have virtual memory.

    DEBUG('a', "Initializing address space, num pages %u, size %u\n",
          numPages, size);


    // BitMap *bitmap = new BitMap(NUM_PHYS_PAGES);
    // First, set up the translation.

    pageTable = new TranslationEntry[numPages];
    for (unsigned i = 0; i < numPages; i++) {
        #ifdef USE_DML
            pageTable[i].physicalPage  = -1;
            pageTable[i].valid = false;
        #else
            // For now, virtual page number = physical page number. NO MORE!
            pageTable[i].physicalPage = bitmap->Find();
            ASSERT((int)pageTable[i].physicalPage != -1);
            pageTable[i].valid        = true;
        #endif
        // Always do:
        pageTable[i].virtualPage  = i;
        pageTable[i].use          = false;
        pageTable[i].dirty        = false;
        pageTable[i].readOnly     = false;
          // If the code segment was entirely on a separate page, we could
          // set its pages to be read-only.
        #ifndef USE_DML
            bzero(&machine ->mainMemory[pageTable[i].physicalPage * PAGE_SIZE], PAGE_SIZE);
        #endif
    }
    // bitmap -> Print();
#ifndef USE_DML
    for(int j=0; j<noffH.code.size;j++){
        char c;
        exe->ReadAt(&c,1,j+noffH.code.inFileAddr);
        int viraddr = noffH.code.virtualAddr + j;
        int vpn = viraddr / PAGE_SIZE;
        int offset = viraddr % PAGE_SIZE;
        int phispn = pageTable[vpn].physicalPage;
        int paddr_start = phispn*PAGE_SIZE;
        int phisaddr = paddr_start + offset;

        machine->mainMemory[phisaddr] = c;

    }

    for(int j = 0; j<noffH.initData.size;j++){
        char c;
        exe->ReadAt(&c,1,j+noffH.initData.inFileAddr);
        int viraddr = noffH.initData.virtualAddr + j;
        int vpn = viraddr / PAGE_SIZE;
        int offset = viraddr % PAGE_SIZE;
        int phispn = pageTable[vpn].physicalPage;
        int paddr_start = phispn * PAGE_SIZE;
        int phisaddr = paddr_start + offset;

        machine->mainMemory[phisaddr] = c;
    }
#endif
    // Zero out the entire address space, to zero the unitialized data
    // segment and the stack segment.
    // memset(machine->mainMemory, 0, size);

    // // Then, copy in the code and data segments into memory.
    // if (noffH.code.size > 0) {
    //     DEBUG('a', "Initializing code segment, at 0x%X, size %u\n",
    //           noffH.code.virtualAddr, noffH.code.size);
    //     executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
    //                        noffH.code.size, noffH.code.inFileAddr);
    // }
    // if (noffH.initData.size > 0) {
    //     DEBUG('a', "Initializing data segment, at 0x%X, size %u\n",
    //           noffH.initData.virtualAddr, noffH.initData.size);
    //     executable->ReadAt(
    //       &(machine->mainMemory[noffH.initData.virtualAddr]),
    //       noffH.initData.size, noffH.initData.inFileAddr);
    // }

}

/// Deallocate an address space.
///
/// Nothing for now!
AddressSpace::~AddressSpace()
{
    #ifndef USE_TLB
        for (unsigned i = 0; i < numPages; i++)
            bitmap -> Clear(pageTable[i].physicalPage);
        delete [] pageTable;
    #endif
    delete exe;
}

/// Set the initial values for the user-level register set.
///
/// We write these directly into the “machine” registers, so that we can
/// immediately jump to user code.  Note that these will be saved/restored
/// into the `currentThread->userRegisters` when this thread is context
/// switched out.
void
AddressSpace::InitRegisters()
{
    for (unsigned i = 0; i < NUM_TOTAL_REGS; i++)
        machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of `Start`.
    machine->WriteRegister(PC_REG, 0);

    // Need to also tell MIPS where next instruction is, because of branch
    // delay possibility.
    machine->WriteRegister(NEXT_PC_REG, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we do not
    // accidentally reference off the end!
    machine->WriteRegister(STACK_REG, numPages * PAGE_SIZE - 16);
    DEBUG('a', "Initializing stack register to %u\n",
          numPages * PAGE_SIZE - 16);
}

/// On a context switch, save any machine state, specific to this address
/// space, that needs saving.
///
/// For now, nothing!
void AddressSpace::SaveState()
{
#ifdef USE_TLB
    DEBUG('b', "Saving state (TLB)\n");
    unsigned i;
    for(i = 0; i < TLB_SIZE; i++){
	    if(machine->tlb[i].dirty && machine->tlb[i].valid)
            pageTable[machine->tlb[i].virtualPage] = machine->tlb[i];
    }
#endif
}

/// On a context switch, restore the machine state so that this address space
/// can run.
///
/// For now, tell the machine where to find the page table.
void AddressSpace::RestoreState()
{
#ifdef USE_TLB
    DEBUG('b', "Restoring state (TLB)\n");
    unsigned i;

    for(i = 0; i < TLB_SIZE; i++)
        machine->tlb[i].valid = false;
#else
    machine->pageTable     = pageTable;
    machine->pageTableSize = numPages;
#endif
}
