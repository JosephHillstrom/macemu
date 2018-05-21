#ifndef PPC_CPU_WRAP_H
#include "cpu/ppc/ppc_wrap.h"
#endif
#include "cpu/ppc/ppc-registers.hpp"
/* ^ since ppc_wrap.h includes us */
void bcctr(regpointer regs, uint32 op);
void dozi(regpointer regs, uint32 op);
void maskg(regpointer regs, uint32 op);
void power_opc_abs(regpointer gCPU, uint32 op);
void power_opc_clcs(regpointer gCPU, uint32 op);
void power_opc_doz(regpointer gCPU, uint32 op);
void power_opc_lscbx(regpointer gCPU, uint32 op);
void power_opc_maskir(regpointer gCPU, uint32 op);
void power_opc_nabs(regpointer gCPU, uint32 op);
void power_opc_rlmi(regpointer gCPU, uint32 op);
void power_opc_rrib(regpointer gCPU, uint32 op);
void power_opc_div(regpointer gCPU, uint32 op);
void power_opc_divs(regpointer gCPU, uint32 op);
void power_opc_mul(regpointer gCPU, uint32 op);
void power_opc_sle(regpointer gCPU, uint32 op);
void power_opc_sleq(regpointer gCPU, uint32 op);
void power_opc_sliq(regpointer gCPU, uint32 op);
void power_opc_sllq(regpointer gCPU, uint32 op);
void power_opc_slq(regpointer gCPU, uint32 op);
void power_opc_sraiq(regpointer gCPU, uint32 op);
void power_opc_sraq(regpointer gCPU, uint32 op);
void power_opc_sre(regpointer gCPU, uint32 op);
void power_opc_srea(regpointer gCPU, uint32 op);




uint32 get_mq(void);
void set_mq(uint32 val);
