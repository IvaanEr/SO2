#ifndef _COREMAP_HH_
#define _COREMAP_HH_

#include "userprog/address_space.hh"
#include "machine/machine.hh"
#include "bitmap.hh"

class CoreMap : public BitMap {
  public:
     CoreMap(int length);
    ~CoreMap();

    int Find(AddressSpace *o, int vpn);

    void UpdateUsage(int ppage);

  private:
    AddressSpace *owner[NUM_PHYS_PAGES];
    int ppnToVpn[NUM_PHYS_PAGES];
    int SelectVictim();

    // FIFO policy
    int FIFO();
    int nextVictim;
    int length;

    // LRU policy
    // This implementation of LRU has the problem the 'int' used for
    // representing the age of the page, will overflow after several
    // runs.
    int LRU();
    void resetAge();
    int age;
    int pageAge[NUM_PHYS_PAGES];
};

#endif
