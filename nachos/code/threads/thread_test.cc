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

#ifdef SEMAPHORE_TEST
  #include "synch.hh"
  Semaphore* sem = new Semaphore("test",3);
#endif

#ifdef LOCK_TEST
  #include "synch.hh"
  Lock *l = new Lock("l1");
#endif

void
SimpleThread(void *name_)
{
    // Reinterpret arg `name` as a string.
    char *name = (char *) name_;


    // If the lines dealing with interrupts are commented, the code will
    // behave incorrectly, because printf execution may cause race
    // conditions.
    for (unsigned num = 0; num < 10; num++) {
      #ifdef SEMAPHORE_TEST
        sem->P(); //take the sem
        DEBUG('s',"*** Thread `%s` has done P\n", name);
      #endif
      #ifdef LOCK_TEST
        l -> Acquire();
        DEBUG('s',"*** Thread `%s` has done Acquire\n", name);
      #endif

        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
        printf("*** Thread `%s` is running: iteration %d\n", name, num);
        //interrupt->SetLevel(oldLevel);
      
      #ifdef SEMAPHORE_TEST
        sem->V(); // release the sem
        DEBUG('s',"*** Thread `%s` has done V\n", name);
      #endif    
      #ifdef LOCK_TEST
        l -> Release();
        DEBUG('s',"*** Thread `%s` has done Release\n", name);
      #endif
      currentThread->Yield();
    }
    
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf("!!! Thread `%s` has finished\n", name);
    //interrupt->SetLevel(oldLevel);
}

#ifdef COND_TEST
#include "synch.hh"

Lock *l1 = new Lock("cond_lock1");
Lock *l2 = new Lock("cond_lock2");
Lock *l  = new Lock("lock");
Condition *lleno = new Condition("lleno",l);
Condition *vacio = new Condition("vacio",l);

#define T 10

int buff [T];

int in = 0, out = 0, n = 0; //donde meto, saco y cuantos tengo.
// List<int> *buff  = new List<int>();

void
Productor(void*m)
{
  int i = 0;
  while(1){

    l -> Acquire();
    DEBUG('s',"Productor ---> Acquire\n");
    while(n>=T){
      lleno -> Wait();
      DEBUG('s',"Productor ---> Wait\n");
    }


    buff[in] = i++;
    in = (in+1)%T;
    n++;
    printf("Produci %d\n",buff[in]);
    vacio -> Signal();
    DEBUG('s',"Productor ---> Signal\n");
    l -> Release();
    DEBUG('s',"Productor ---> Release\n");
  }
}

void
Consumidor(void*t)
{
  while(1){
    l -> Acquire();
    
    while(n == 0)
      vacio->Wait();
    

    printf("Consumi %d\n",buff[out]);
    out = (out+1)%T;
    n--;
    lleno->Signal();
    l->Release();
  }
}
#endif


/// Set up a ping-pong between several threads.
///
/// Do it by launching ten threads which call `SimpleThread`, and finally
/// calling `SimpleThread` ourselves.
void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    // for(int i = 1; i < 5; i++){
    //     char *name = new char[64];
    //     sprintf(name, "%d thread",i+1);
    //     // strncpy(name, "2nd", 64);
    //     Thread *newThread = new Thread(name);
    //     newThread->Fork(SimpleThread, (void *) name);
    // }
    // SimpleThread((void *) "1 thread");v
    #ifdef COND_TEST
    void *m = NULL;
    Thread *cons = new Thread("cons");
    Thread *prod = new Thread("prod");
    
    prod->Fork(Consumidor,m);
    cons->Fork(Productor,m);
    #endif
} 
