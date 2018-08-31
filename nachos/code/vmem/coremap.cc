#include "coremap.hh"

#ifdef VMEM

CoreMap::CoreMap(int n) : BitMap (n)
{
  #ifdef LRU_POLICY
  printf("Using LRU policy for SWAP.\n");
  #else
  printf("Using FIFO policy for SWAP.\n");
  #endif

  nextVictim = 0;
  length = n;

  // LRU policy
  age = 0;
  for (int i = 0; i < NUM_PHYS_PAGES; i++)
    pageAge[i] = 0;
}

CoreMap::~CoreMap(){}

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
  #ifdef LRU_POLICY
  return LRU();
  #else
  return FIFO();
  #endif
}

int
CoreMap::FIFO()
{
  int victim = nextVictim;
  nextVictim = (nextVictim+1) % length;
  return victim;
}

int
CoreMap::LRU()
{
  int min_pos = 0;
  int min_val = pageAge[0];
  for (int i = 0; i < NUM_PHYS_PAGES; i++){
    if (pageAge[i]<min_val){
      min_pos = i;
      min_val = pageAge[i];
    }
  }
  return min_pos;
}

void
CoreMap::ResetAge()
{
  age = 0;
  for (int i = 0; i < NUM_PHYS_PAGES; i++)
    pageAge[i] = 0;
}

void
CoreMap::AvoidOverflow()
{
    if (age == 1000000)
      ResetAge();
}

void
CoreMap::UpdateAge(int ppage)
{
  AvoidOverflow();
  pageAge[ppage] = age;
  age++;
}
#endif
