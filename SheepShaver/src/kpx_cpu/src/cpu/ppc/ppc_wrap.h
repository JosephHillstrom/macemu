#ifndef PPC_CPU_WRAP_H
#define PPC_CPU_WRAP_H
#include "cpu/ppc/ppc-registers.hpp"
#ifdef __cplusplus
#include "ppc-cpu.hpp"
#define wrap_op_func(func) ##func##_cpp(powerpc_cpu cpu, uint32 op) {func(make_regptr(_regs.regs), op);}
#define call_op_func(func, cpu, op) ##func##_cpp(cpu, op)
static inline regpointer make_regptr(struct powerpc_registers regs)
{
  regpointer ret;
  ret.gpr = &(regs.gpr[0]);
  ret.fpr = &(regs.fpr[0]);
  ret.vr = &(regs.vr[0]);
  ret.cr = &(regs.cr.cr);
  ret.xer.so = &(regs.xer.so);
  ret.xer.ov = &(regs.xer.ov);
  ret.xer.ca = &(regs.xer.ca);
  ret.xer.byte_count = &(regs.xer.byte_count);
  ret.vscr = &(regs.vscr.vscr);
  ret.vrsave = &(regs.vrsave);
  ret.fpscr = &(regs.fpscr);
  ret.lr = &(regs.lr);
  ret.ctr = &(regs.ctr);
  ret.pc = &(regs.pc);
  return ret;
}
#endif
