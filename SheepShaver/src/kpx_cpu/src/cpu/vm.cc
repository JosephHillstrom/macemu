#include "sysdeps.h"
#include "vm.hpp"
uintptr VMBaseDiff;
uint32 address_size;
uint8 * sheep_mem;
void vm_ini(uint8 * mem)
{
	VMBaseDiff = (uintptr)mem;
}
void setSize(uint32 size)
{
	address_size = size;
}
void init_sheep_mem(uint8 * mem)
{
	sheep_mem = mem;
}
