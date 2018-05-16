#include <stdint.h>
#include "sysdeps.h"
#include "ppc_wrap.h"
extern "C" {
regpointer c_registers;
}
regpointer make_regptr(struct powerpc_registers * regs)
{
  regpointer ret;
  ret.gpr = &(regs->gpr[0]);
  ret.fpr = &(regs->fpr[0]);
  ret.vr = &(regs->vr[0]);
  ret.cr = &(regs->cr.cr);
  ret.xer.so = &(regs->xer.so);
  ret.xer.ov = &(regs->xer.ov);
  ret.xer.ca = &(regs->xer.ca);
  ret.xer.byte_count = &(regs->xer.byte_count);
  ret.xer.reserved = &(regs->xer.reserved);
  ret.vscr = &(regs->vscr.vscr);
  ret.vrsave = &(regs->vrsave);
  ret.fpscr = &(regs->fpscr);
  ret.lr = &(regs->lr);
  ret.ctr = &(regs->ctr);
  ret.pc = &(regs->pc);
  return ret;
}
void init_c_registers(powerpc_registers & regs)
{
    c_registers = make_regptr(&regs);
}
