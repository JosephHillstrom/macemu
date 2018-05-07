#ifndef PPC_CPU_WRAP_H
#define PPC_CPU_WRAP_H
#include "ppc-registers.hpp"
#ifdef __cplusplus
#include "ppc-cpu.hpp"
#define wrap_op_func(func) static inline void func##_cpp(powerpc_cpu cpu, uint32 op) {func(c_registers, op);}
#define call_op_func(func, cpu, op) func##_cpp(cpu, op)
extern "C" {
#endif
#include "OLDOPS.H"
extern regpointer c_registers;
#ifdef __cplusplus
}
void init_c_registers(struct powerpc_registers regs);
wrap_op_func(bcctr)
#endif
#endif
