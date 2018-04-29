#include "sysdeps.h"
#include "vm.hpp"
uintptr VMBaseDiff;
uint32 address_size;
void vm_ini(uint8 * mem)
{
	VMBaseDiff = (uintptr)mem;
}
void setSize(uint32 size)
{
	address_size = size;
}