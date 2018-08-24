#include "coremap.hh"

#ifdef VMEM

CoreMap::CoreMap(int n) : BitMap (n)
{
  nextVictim = 0;
  length = n;
}

CoreMap::~CoreMap()
{
}

int
CoreMap::Find(AddressSpace *o, int vpn )
{
  int free_page = BitMap::Find();

  if (free_page == -1) {
    int victim = SelectVictim();
    ASSERT(0 <= victim && victim < NUM_PHYS_PAGES);
    ASSERT(owner[victim]);
    owner[victim] -> SaveToSwap(ppnToVpn[victim]);
    free_page = victim;
  }

  owner[free_page] = o;
  ppnToVpn[free_page] = vpn;

  return free_page;
}

int
CoreMap::SelectVictim()
{
  int victim = nextVictim;
  nextVictim = (nextVictim+1) % length;
  return victim;
}

#endif
