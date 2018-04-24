#include "vm.hpp"
uint8ptr VMBaseDiff;
void vm_ini(uint8 * mem)
{
	VMBaseDiff = (uintptr)mem;
}
