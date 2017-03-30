/// Simple test case for the threads assignment.
///
/// Create several threads, and have them context switch back and forth
/// between themselves by calling `Thread::Yield`, to illustrate the inner
/// workings of the thread system.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2017 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "system.hh"
#include <stdio.h>

/// Loop 10 times, yielding the CPU to another ready thread each iteration.
///
/// * `name` points to a string with a thread name, just for debugging
///   purposes.
#ifdef THREADS
void
SimpleThread(void *name_)
{
    // Reinterpret arg `name` as a string.
    char *name = (char *) name_;

    // If the lines dealing with interrupts are commented, the code will
    // behave incorrectly, because printf execution may cause race
    // conditions.
    for (unsigned num = 0; num < 10; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
        printf("*** Thread `%s` is running: iteration %d\n", name, num);
        //interrupt->SetLevel(oldLevel);
        currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf("!!! Thread `%s` has finished\n", name);
    //interrupt->SetLevel(oldLevel);
}

/// Set up a ping-pong between several threads.
///
/// Do it by launching ten threads which call `SimpleThread`, and finally
/// calling `SimpleThread` ourselves.
void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    for(int i = 1; i < 5; i++){
        char *name = new char[64];
        sprintf(name, "%d thread",i+1);
        // strncpy(name, "2nd", 64);
        Thread *newThread = new Thread(name);
        newThread->Fork(SimpleThread, (void *) name);
    }
    SimpleThread((void *) "1 thread");
}
#endif


#ifdef SEMAPHORE_TEST

#include "synch.hh"

Semaphore* sem = new Semaphore("test",3);

void
SimpleThreadSem(void *name_)
{
    // Reinterpret arg `name` as a string.
    char *name = (char *) name_;

    // If the lines dealing with interrupts are commented, the code will
    // behave incorrectly, because printf execution may cause race
    // conditions.

    sem->P(); //take the sem
    DEBUG('s',"*** Thread `%s` has done P\n", name);

    for (unsigned num = 0; num < 10; num++) {
        IntStatus oldLevel = interrupt->SetLevel(INT_OFF);
        
        
        printf("*** Thread `%s` is running: iteration %d\n", name, num);
        currentThread->Yield();
        
        interrupt->SetLevel(oldLevel);
        
    }

    sem->V(); // release the sem
    DEBUG('s',"*** Thread `%s` has done V\n", name);
    
    IntStatus oldLevel = interrupt->SetLevel(INT_OFF);
    printf("!!! Thread `%s` has finished\n", name);
    interrupt->SetLevel(oldLevel);
}

/// Set up a ping-pong between several threads.
///
/// Do it by launching ten threads which call `SimpleThread`, and finally
/// calling `SimpleThread` ourselves.
void
ThreadTestSem()
{
    DEBUG('t', "Entering SimpleTest");

    for(int i = 1; i < 5; i++){
        char *name = new char[64];
        sprintf(name, "%d thread",i+1);
        // strncpy(name, "2nd", 64);
        Thread *newThread = new Thread(name);
        newThread->Fork(SimpleThreadSem, (void *) name);
    }
    SimpleThreadSem((void *) "1 thread");
}

#endif