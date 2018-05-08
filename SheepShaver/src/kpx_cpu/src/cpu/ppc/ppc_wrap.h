#ifndef PPC_CPU_WRAP_H
#define PPC_CPU_WRAP_H
#include "ppc-registers.hpp"
#ifdef __cplusplus
#include "ppc-cpu.hpp"
extern "C" {
#endif
#include "OLDOPS.H"
extern regpointer c_registers;
#ifdef __cplusplus
}
void init_c_registers(struct powerpc_registers regs);
static inline void bcctr_cpp(powerpc_cpu cpu, uint32 op) {bcctr(c_registers, op);}
#endif
#endif
