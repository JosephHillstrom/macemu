#ifndef PPC_CPU_WRAP_H
#define PPC_CPU_WRAP_H
#ifdef __cplusplus
#include "ppc-cpu.hpp"
#include "ppc-registers.hpp"
#define wrap_op_func(func) ##func##_cpp(powerpc_cpu cpu, uint32 op) {func(_regs.regs
#define call_op_func(func, cpu, op) ##func##_cpp(cpu, op)
#endif
#endif
