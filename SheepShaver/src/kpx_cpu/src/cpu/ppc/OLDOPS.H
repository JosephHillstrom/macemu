#ifndef PPC_CPU_WRAP_H
#include "cpu/ppc/ppc_wrap.h"
#endif
#include "cpu/ppc/ppc-registers.hpp"
/* ^ since ppc_wrap.h includes us */
void bcctr(regpointer regs, uint32 op);
void dozi(regpointer regs, uint32 op);
void maskg(regpointer regs, uint32 op);
void power_opc_abs(regpointer gCPU, uint32 op);
