#include "sysdeps.h"
#include "vm.hpp"
uintptr VMBaseDiff;
void vm_ini(uint8 * mem)
{
	VMBaseDiff = (uintptr)mem;
}
