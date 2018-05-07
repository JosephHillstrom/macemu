#include "cpu/ppc/ppc_wrap.h"
void bcctr(regpointer regs, uint32 op)
{
      uint32 rd = 
			if (op & 1)
				*(regs.lr) = *(regs.pc);
			if (!(rd & 4)) {
				ctr--;
				if (rd & 2) {
					if (ctr)
						goto bctr_nobranch;
				} else {
					if (!ctr)
						goto bctr_nobranch;
				}
			}
			if (!(rd & 0x10)) {
				if (rd & 8) {
					if (!(cr & (0x80000000 >> ra)))
						goto bctr_nobranch;
				} else {
					if (cr & (0x80000000 >> ra))
						goto bctr_nobranch;
				}
			}
pc = ctr & 0xfffffffc;
}
