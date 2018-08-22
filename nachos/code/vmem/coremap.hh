#ifndef _COREMAP_HH_
#define _COREMAP_HH_

#include "userprog/address_space.hh"
#include "machine/machine.hh"
#include "bitmap.hh"
// #include "system.hh"

// class AddressSpace;

class CoreMap : public BitMap {
  public:
     CoreMap(int length);
    ~CoreMap();
    // void Map(unsigned int phys, TranslationEntry *virt, AddrSpace *s);
    // void Unmap(unsigned int phys);

    int Find(AddressSpace *o, int vpn);

  private:
    AddressSpace *owner[NUM_PHYS_PAGES];
    int ppnToVpn[NUM_PHYS_PAGES];

    // Right now we have FIFO policy
    int SelectVictim();
    int nextVictim;
    int length;
};

#endif
