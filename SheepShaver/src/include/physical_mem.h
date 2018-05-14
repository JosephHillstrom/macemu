/*For patches &c. that use hardware addresses*/
#ifndef PHYSICAL_MEM_H
#define PHYSICAL_MEM_H


#include "sysdeps.h"
#include "vm.hpp"


#ifdef ReadMacInt8
#undef ReadMacInt8
#endif
#define ReadMacInt8(addr) vm_read_physical_memory_1((vm_addr_t)addr)

#ifdef WriteMacInt8
#undef WriteMacInt8
#endif
#define WriteMacInt8(addr, v) vm_write_physical_memory_1((vm_addr_t)addr, v)

#ifdef ReadMacInt16
#undef ReadMacInt16
#endif
#define ReadMacInt16(addr) vm_read_physical_memory_2((vm_addr_t)addr)

#ifdef WriteMacInt16
#undef WriteMacInt16
#endif
#define WriteMacInt16(addr, v) vm_write_physical_memory_2((vm_addr_t)addr, v)

#ifdef ReadMacInt32
#undef ReadMacInt32
#endif
#define ReadMacInt32(addr) vm_read_physical_memory_4((vm_addr_t)addr)

#ifdef WriteMacInt32
#undef WriteMacInt32
#endif
#define WriteMacInt32(addr, v) vm_write_physical_memory_4((vm_addr_t)addr, v)

#ifdef ReadMacInt64
#undef ReadMacInt64
#endif
#define ReadMacInt64(addr) vm_read_physical_memory_8((vm_addr_t)addr)

#ifdef WriteMacInt64
#undef WriteMacInt64
#endif
#define WriteMacInt64(addr, v) vm_write_physical_memory_8((vm_addr_t)addr, v)


#define read_bswap_int_8 ReadMacInt8
#define write_bswap_int_8 WriteMacInt8

#ifdef read_bswap_int_16
#undef read_bswap_int_16
#endif
#define read_bswap_int_16(addr) vm_read_physical_memory_2_reversed((vm_addr_t)addr)

#ifdef write_bswap_int_16
#undef write_bswap_int_16
#endif
#define write_bswap_int_16(addr, v) vm_write_physical_memory_2_reversed((vm_addr_t)addr, v)

#ifdef read_bswap_int_32
#undef read_bswap_int_32
#endif
#define read_bswap_int_32(addr) vm_read_physical_memory_4_reversed((vm_addr_t)addr)

#ifdef write_bswap_int_32
#undef write_bswap_int_32
#endif
#define write_bswap_int_32(addr, v) vm_write_physical_memory_4_reversed((vm_addr_t)addr, v)

#ifdef read_bswap_int_64
#undef read_bswap_int_64
#endif
#define read_bswap_int_64(addr) vm_read_physical_memory_8_reversed((vm_addr_t)addr)

#ifdef write_bswap_int_64
#undef write_bswap_int_64
#endif
#define write_bswap_int_64(addr, v) vm_write_physical_memory_8_reversed((vm_addr_t)addr, v)



#endif
