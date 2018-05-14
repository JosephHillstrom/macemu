#include <stdint.h>
#include "cpu/ppc/OLDOPS.H"
#define MAKE_RD(op) ((op >> 21) & 0x1F)
#define MAKE_RA(op) ((op >> 16) & 0x1F)
#define MAKE_RB(op) ((op & 0x0000FC00) >> 10)
#define MAKE_IMMEDIATE(op) (op & 0x0000FFFF)
#define OPC_UPDATE_CRO(opcode) (opcode & 1)
#define CLEAR_CRO(cr) (cr) &= 0x0FFFFFFFF
#define OPC_UPDATE_OV(opcode) (opcode & 0x00000800)
#define SET_OV(regs) (*regs.xer.ov) = 1; (*regs.xer.so) = 1;
#define CLEAR_OV(regs) (*regs.xer.ov) = 0;
static void record(regpointer regs, uint32 val)
{
    uint32 crf = 0;
    if (val == 0) {
        crf |= 0x20000000;
    }
    else if (val & 0x80000000) {
        crf |= 0x80000000;
    }
    else {
        crf |= 0x40000000;
    }
    if (*regs.xer.so) {
        crf |= 0x10000000;
    }
    (*regs.cr) = ((*regs.cr) & 0x0fffffff) | crf;
}
uint32 make_mask(uint32 mstart, uint32 mstop)
{
    uint32 tmp = 0;
    if (mstart < (mstop + 1)) {
        for (int i = mstart; i < mstop; i ++) {
            tmp |= (1 << (32 - i));
        }
    }
    else if (mstart == (mstop + 1)) {
        tmp = 0xFFFFFFFF;
    }
    else {
        tmp = 0xFFFFFFFF;
        for (int i = (mstop + 1); i < (mstart - 1); i ++) {
            tmp &= (~(1 << (32 - i)));
        }
    }
    return tmp;
}


void bcctr(regpointer regs, uint32 op)
{
	uint32 rd = MAKE_RD(op);
	uint32 ra = MAKE_RA(op);
	if (op & 1) {
		*(regs.lr) = *(regs.pc);
	}
	if (!(rd & 4)) {
		*(regs.ctr)--;
		if (rd & 2) {
			if (*(regs.ctr)) {
				return;
			}
		} 
		else {
			if (!(*(regs.ctr))) {
				return;
			}
		}
	}
	if (!(rd & 0x10)) {
		if (rd & 8) {
			if (!((*(regs.cr)) & (0x80000000 >> ra))) {
				return;
			}
		} 
		else {
			if ((*(regs.cr)) & (0x80000000 >> ra)) {
				return;
			}
		}
	}
	*(regs.pc) = ((*(regs.ctr)) & 0xfffffffc);
}
void dozi(regpointer regs, uint32 op)
{
    int rD = MAKE_RD(op);
    int rA = MAKE_RA(op);
    uint32 immediate = MAKE_IMMEDIATE(op);
    if ((signed)(int32)(regs.gpr[rA]) >= (signed)(int32)immediate) {
        regs.gpr[rD] = 0;
    }
    else {
        regs.gpr[rD] = (immediate - (regs.gpr[rA]));
    }
}
void maskg(regpointer regs, uint32 op)
{
    int rS = MAKE_RD(op);
    int rA = MAKE_RA(op);
    int rB = MAKE_RB(op);
    regs.gpr[rA] = make_mask((regs.gpr[rS] & 0x0000001F), (regs.gpr[rB] & 0x0000001F));
    if (OPC_UPDATE_CRO(op)) {
        record(regs, regs.gpr[rA]);
    }
}

void power_opc_abs(regpointer gCPU, uint32 op)
{
    uint32 rD = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    int32 signedval = (int32)gCPU.gpr[rA];
    if ((uint32)signedval == 0x80000000) {
        if (OPC_UPDATE_CRO(op)) {
            CLEAR_CRO((*gCPU.cr));
            (*gCPU.cr) |= (1 << 28);
        }
        if (OPC_UPDATE_OV(op)) {
            SET_OV(gCPU);
        }
        gCPU.gpr[rD] = 0x80000000;
    }
    else if (signedval > 0) {
        if (OPC_UPDATE_CRO(op)) {
            (*gCPU.cr) &= 0x0FFFFFFF;
            (*gCPU.cr) |= (4 << 28);
        }
        if (OPC_UPDATE_OV(op)) {
            CLEAR_OV(gCPU);
        }
        gCPU.gpr[rD] = gCPU.gpr[rA];
    }
    else if (signedval == 0) {
        if (OPC_UPDATE_CRO(op)) {
            CLEAR_CRO(*gCPU.cr);
            (*gCPU.cr) |= (2 << 28);
        }
        if (OPC_UPDATE_OV(op)) {
            CLEAR_OV(gCPU);
        }
        gCPU.gpr[rD] = 0;
    }
    else if (signedval < 0) {
        if (OPC_UPDATE_CRO(op)) {
            CLEAR_CRO(*gCPU.cr);
            (*gCPU.cr) |= (8 << 28);
        }
        if (OPC_UPDATE_OV(op)) {
            CLEAR_OV(gCPU);
        }
        gCPU.gpr[rD] = (-signedval);
    }
}
