#include <stdint.h>
#include "cpu/ppc/OLDOPS.H"
#include "sysdeps.h"
#include "vm.hpp"
#define MAKE_RD(op) ((op >> 21) & 0x1F)
#define MAKE_RA(op) ((op >> 16) & 0x1F)
#define MAKE_RB(op) ((op & 0x0000FC00) >> 10)
#define MAKE_IMMEDIATE(op) (op & 0x0000FFFF)
#define OPC_UPDATE_CRO(opcode) (opcode & 1)
#define CLEAR_CRO(cr) (cr) &= 0x0FFFFFFFF
#define OPC_UPDATE_OV(opcode) (opcode & 0x00000800)
#define SET_OV(regs) (*regs.xer.ov) = 1; (*regs.xer.so) = 1
#define CLEAR_OV(regs) (*regs.xer.ov) = 0
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
uint32 use_mask(uint32 mask, uint32 value, uint32 start)
{
	uint32 ret = start;
	int i;
	for (i = 0; i < 32; i ++) {
		if (mask & (1 << i)) {
			ret &= (~(1 << i));
			ret |= (value & (1 << i));
		}
	}
    return ret;
}
uint32 mq;
uint32 get_mq(void)
{
	return mq;
}
void set_mq(uint32 val)
{
	mq = val;
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

void power_opc_clcs(regpointer gCPU, uint32 op)
{
    uint32 rD = MAKE_RD(op);
    gCPU.gpr[rD] = 32;
    /* we just give the cache line size for powerpc  *
     * instead of saying, that the cache is disabled */
}

void power_opc_doz(regpointer gCPU, uint32 op)
{
    uint32 rD = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 rB = MAKE_RB(op);
    if ((int32)gCPU.gpr[rA] >= (int32)gCPU.gpr[rB]) {
        if (OPC_UPDATE_CRO(op)) {
            CLEAR_CRO(*gCPU.cr);
            (*gCPU.cr) |= (2 << 28);
        }
        if (OPC_UPDATE_OV(op)) {
            if(gCPU.gpr[rA] == gCPU.gpr[rB]) {
                /* unsigned overflow *
                 * ((~x) + x + 1)    *
                 * ALWAYS overflows  *
                 * for ANY value of  *
                 * X. doz uses ~rA   *
                 * + rB + 1, rather  *
                 * than (signed)rA - *
                 * (signed) rB       */
                SET_OV(gCPU);
            }
            else {
                CLEAR_OV(gCPU);
            }
        }
        gCPU.gpr[rD] = 0;
    }
    else {
        if (OPC_UPDATE_CRO(op)) {
            CLEAR_CRO(*gCPU.cr);
            (*gCPU.cr) |= (4 << 28);
        }
        if (OPC_UPDATE_OV(op)) {
            CLEAR_OV(gCPU);
        }
        gCPU.gpr[rD] = ((~gCPU.gpr[rA]) + gCPU.gpr[rB] + 1);
    }
}

void power_opc_lscbx(regpointer gCPU, uint32 op)
{
    bool match = false;
    bool discard = false;
    uint32 rD = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 rB = MAKE_RB(op);
    uint32 addr;
    uint32 cro = 0;
    if (rA == 0) {
        addr = rB;
    }
    else {
        addr = rA + rB;
    }
    uint32 number = (*gCPU.xer.byte_count);
    if (number == 0) {
        /*if (OPC_UPDATE_CRO(op)) {
            (*gCPU.cr) |= (0xF << 28);
            // or CR0 = Undefined 
        }*/
        return;
    }
    uint8 temp;
    int i = 0;
    int j = 4;
    while (i <= number) {
        if (!j) {
            j = 4;
            discard = false;
            rD ++;
            if (rD >= 32) {
                rD = 0;
            }
            if ((rD == rA && (rA != 0))) {
                discard = true;
            }
            if (rD == rB) {
                discard = true;
            }
        }
        temp = vm_read_memory_1(addr);
	if (!discard) {
            gCPU.gpr[rD] |= (temp << (j * 8));
	    gCPU.gpr[rD] &= (~(0xFF << (j * 8)));
	}
        if (temp == (((*gCPU.xer.reserved) >> 2) & 0xff)) {
            (*gCPU.xer.byte_count) = i;
             match = true;
             break;
        }
        addr ++;
        i ++;
        j --;
    }
    if (OPC_UPDATE_CRO(op)) {
        if ((*gCPU.xer.so)) {
            cro |= 1;
        }
        if (match) {
            cro |= 2;
        }
        (*gCPU.cr) |= (cro << 28);
    }
}

void power_opc_maskir(regpointer gCPU, uint32 op)
{
	uint32 rS = MAKE_RD(op);
	uint32 rA = MAKE_RA(op);
	uint32 rB = MAKE_RB(op);
	gCPU.gpr[rA] = use_mask(gCPU.gpr[rB], gCPU.gpr[rS], gCPU.gpr[rA]);
	if (OPC_UPDATE_CRO(op)) {
		record(gCPU, gCPU.gpr[rA]);
	}
}

void power_opc_nabs(regpointer gCPU, uint32 op)
{
    uint32 rD = MAKE_RD(op);
    power_opc_abs(gCPU, op);
    gCPU.gpr[rD] = (uint32)(-((int32)gCPU.gpr[rD]));
    if (OPC_UPDATE_CRO(op) && (*gCPU.cr & (12 << 28))) {
        if ((*gCPU.cr) & (4 << 28)) {
            (*gCPU.cr) = ((*gCPU.cr) & (~((4 << 28)))) | (8 << 28);
        }
        else if ((*gCPU.cr) & (8 << 28)) {
            (*gCPU.cr) = ((*gCPU.cr) & (~(8 << 28))) | (4 << 28);
        }
    }
}

void power_opc_rlmi(regpointer gCPU, uint32 op)
{
    uint32 rB = MAKE_RB(op);
    uint32 rA = MAKE_RA(op);
    uint32 rS = MAKE_RD(op);
    uint32 mask_begin = ((op & 0x000007C0) >> 6);
    uint32 mask_end =   ((op & 0x0000003E) >> 1);
    uint32 tmp = gCPU.gpr[rS];
    uint32 toRotate = (gCPU.gpr[rB] & 0x0000001F);
    uint32 mask = make_mask(mask_begin, mask_end);
    tmp = (tmp << toRotate) | (tmp >> (32-toRotate));
    gCPU.gpr[rA] = use_mask(mask, tmp, gCPU.gpr[rA]);
    if (OPC_UPDATE_CRO(op)) {
        record(gCPU, gCPU.gpr[rA]);
    }
}

void power_opc_rrib(regpointer gCPU, uint32 op)
{
    uint32 rS = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 rB = MAKE_RB(op);
    uint32 toShift = (gCPU.gpr[rB] & 0x0000001F);
    if (gCPU.gpr[rS] & 0x80000000) {
        gCPU.gpr[rA] |= (0x80000000 >> toShift);
    }
    else {
        gCPU.gpr[rA] &= (~(0x80000000 >> toShift));
    }
    if (OPC_UPDATE_CRO(op)) {
        record(gCPU, gCPU.gpr[rA]);
    }
}

void power_opc_div(regpointer gCPU, uint32 op)
{
    uint32 rD = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 rB = MAKE_RB(op);
    uint32 ov = 0;
    uint64 divizer = (((uint64)gCPU.gpr[rA] << 32)|(mq));
    uint64 kwoshint = divizer / gCPU.gpr[rB];
    if (kwoshint & 0xFFFFFFFF00000000) {
        ov = 1;
    }
    if (((int64)divizer==-2*1024*1024*1024/* -2^31 */) && (int32)gCPU.gpr[rB] == -1) {
        gCPU.gpr[rD] = (uint32)(((uint64)(-(((int64)2 * 1024 * 1024 * 1024)))) & 0x00000000ffffffff); /* -2^31 */
        mq = 0;
        ov = 1;
    }
    else {
        gCPU.gpr[rD] = (uint32)kwoshint;
        mq = divizer % gCPU.gpr[rB];
    }
    if (OPC_UPDATE_OV(op)) {
        if (ov) {
            SET_OV(gCPU);
        }
        else {
            CLEAR_OV(gCPU);
        }
    }
    if (OPC_UPDATE_CRO(op)) {
        if (ov) {
            CLEAR_CRO(*gCPU.cr);
            (*gCPU.cr) |= (1 << 28);
        }
        else {
            record(gCPU, gCPU.gpr[rD]);
        }
    }
}

void power_opc_divs(regpointer gCPU, uint32 op)
{
	uint32 rD = MAKE_RD(op);
	uint32 rA = MAKE_RA(op);
	uint32 rB = MAKE_RB(op);
	gCPU.gpr[rD] = (uint32)((int32)((int32)gCPU.gpr[rA] / (int32)gCPU.gpr[rB]));
	mq = (uint32)((int32)((int32)gCPU.gpr[rA] % (int32)gCPU.gpr[rB]));
	if (OPC_UPDATE_OV(op)) {
		CLEAR_OV(gCPU);
	}
	if (OPC_UPDATE_CRO(op)) {
		record(gCPU, gCPU.gpr[rD]);
	}
}

void power_opc_mul(regpointer gCPU, uint32 op)
{
	uint32 rD = MAKE_RD(op);
	uint32 rA = MAKE_RA(op);
	uint32 rB = MAKE_RB(op);
	int64 result = ((int32)gCPU.gpr[rA] * (int32)gCPU.gpr[rB]);
	gCPU.gpr[rD] = (uint32)((int32)(result >> 32));
	mq = (uint32)((int32)result);
	if (OPC_UPDATE_OV(op)) {
		if (gCPU.gpr[rD]) {
			SET_OV(gCPU);
		}
		else {
			CLEAR_OV(gCPU);
		}
	}
	if (OPC_UPDATE_CRO(op)) {
		record(gCPU, gCPU.gpr[rD]);
	}
}

void power_opc_sle(regpointer gCPU, uint32 op)
{
	uint32 rB = MAKE_RB(op);
	uint32 rA = MAKE_RA(op);
	uint32 rS = MAKE_RD(op);
	uint32 tmp = gCPU.gpr[rS];
	uint32 toRotate = (gCPU.gpr[rB] & 0x0000001F);
	uint32 mask = 0;
	int i;
	for (i = 31; i > toRotate; i --) {
		mask |= (1 << i);
	}
	tmp = ((tmp << toRotate) | (tmp >> (32 - toRotate)));
	mq = tmp;
	gCPU.gpr[rA] = (tmp & mask);
	if (OPC_UPDATE_CRO(op)) {
		record(gCPU, gCPU.gpr[rA]);
	}
}

void power_opc_sleq(regpointer gCPU, uint32 op)
{
    uint32 rS = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 rB = MAKE_RB(op);
    uint32 tmp = gCPU.gpr[rS];
    uint32 toRotate = (gCPU.gpr[rB] & 0x0000001F);
    uint32 mask = 0;
    int i;
    for (i = 31; i > toRotate; i --) {
        mask |= (1 << i);
    }
    tmp = (tmp << toRotate) | (tmp >> (toRotate - 31));
    gCPU.gpr[rA] = use_mask(mask, tmp, mq);
    mq = tmp;
    if (OPC_UPDATE_CRO(op)) {
        record(gCPU, gCPU.gpr[rA]);
    }
}

void power_opc_sliq(regpointer gCPU, uint32 op)
{
    uint32 rS = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 SH = MAKE_RB(op);
    uint32 mask = 0;
    uint32 tmp = gCPU.gpr[rS];
    int i;
    for (i = 31; i > SH; i --) {
        mask |= (1 << i);
    }
    tmp = (tmp << SH) | (tmp >> (SH - 31));
    mq = tmp;
    gCPU.gpr[rA] = (tmp & mask);
    if (OPC_UPDATE_CRO(op)) {
        record(gCPU, gCPU.gpr[rA]);
    }
}

void power_opc_sllq(regpointer gCPU, uint32 op)
{
    uint32 rS = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 rB = MAKE_RB(op);
    uint32 mask = 0;
    uint32 tmp = gCPU.gpr[rS];
    uint32 toRotate = (gCPU.gpr[rB] & 0x0000001F);
    int i;
    for (i = 31; i > toRotate; i --) {
        mask |= (1 << i);
    }
    //rol(tmp, toRotate);
    tmp = ((tmp << toRotate) | (tmp >> (32 - toRotate)));
    gCPU.gpr[rA] = use_mask(mask, tmp, mq);
    if (OPC_UPDATE_CRO(op)) {
        record(gCPU, gCPU.gpr[rA]);
    }
}

void power_opc_slq(regpointer gCPU, uint32 op)
{
    uint32 rS = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 rB = MAKE_RB(op);
    uint32 toRotate = (gCPU.gpr[rB] & 0x0000001F);
    uint32 tmp = gCPU.gpr[rS];
    int i;
    uint32 mask = 0;
    if (!(gCPU.gpr[rB] & 0x00000200)) {
        for (i = 31; i > toRotate; i--) {
            mask |= (1 << i);
        }
    }
    tmp = ((tmp << toRotate) | (tmp << (32 - toRotate)));
    mq = tmp;
    gCPU.gpr[rA] = (tmp & mask);
    if (OPC_UPDATE_CRO(op)) {
        record(gCPU, gCPU.gpr[rA]);
    }
}

void power_opc_sraiq(regpointer gCPU, uint32 op)
{
    uint32 rS = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 SH = MAKE_RB(op);
    uint32 tmp = gCPU.gpr[rS];
    uint32 temp;
    uint32 mask = 0xFFFFFFFF;
    tmp = (tmp >> SH) | (tmp << (32 - SH));
    for (int i = 31; i > SH; i--) {
        mask &= (~(1 << i));
    }
    mq = tmp;
    if (gCPU.gpr[rS] & 0x80000000) {
        gCPU.gpr[rA] = 0xFFFFFFFF;
    }
    else {
        gCPU.gpr[rA] = (tmp & mask);
    }
    temp = (tmp & (~(mask)));
    tmp |= temp;
    tmp &= gCPU.gpr[rS];
    if (tmp & 0x80000000) {
        (*gCPU.xer.ca) = 1;
    }
    else {
        (*gCPU.xer.ca) = 0;
    }
    if (OPC_UPDATE_CRO(op)) {
        record(gCPU, gCPU.gpr[rA]);
    }
}

void power_opc_sraq(regpointer gCPU, uint32 op)
{
    uint32 rS = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 rB = MAKE_RB(op);
    uint32 mask = 0xFFFFFFFF;
    uint32 tmp = gCPU.gpr[rS];
    uint32 temp;
    uint32 toRotate = (gCPU.gpr[rB] & 0x0000001F);
    tmp = (tmp >> toRotate) | (tmp << (32 - toRotate));
    if (gCPU.gpr[rB] & 0x00000020) {
        for (int i = 31; i > toRotate; i--) {
            mask &= (~(1 << i));
        }
    }
    else {
        mask = 0;
    }
    mq = tmp;
    if (gCPU.gpr[rS] & 0x80000000) {
        gCPU.gpr[rA] = 0xFFFFFFFF;
    }
    else {
        gCPU.gpr[rA] = (tmp & mask);
    }
    temp = (tmp & (~(mask)));
            tmp |= temp;
            tmp &= gCPU.gpr[rS];
            if (tmp & 0x80000000) {
                (*gCPU.xer.ca) = 1;
            }
            else {
                (*gCPU.xer.ca) = 0;
            }
            if (OPC_UPDATE_CRO(op)) {
                record(gCPU, gCPU.gpr[rA]);
            }
}

void power_opc_sre(regpointer gCPU, uint32 op)
{
    uint32 rS = MAKE_RD(op);
    uint32 rA = MAKE_RA(op);
    uint32 rB = MAKE_RB(op);
    uint32 tmp = gCPU.gpr[rS];
    uint32 toRotate = (rB & 0x0000001F);
    uint32 mask = 0;
    tmp = (tmp >> toRotate) | (tmp << (32 - toRotate));
    mq = tmp;
    for (int i = 31; i > toRotate; i--) {
        mask &= (~(1 << i));
    }
    gCPU.gpr[rA] = (tmp & mask);
    if (OPC_UPDATE_CRO(op)) {
        record(gCPU, gCPU.gpr[rA]);
    }
}

void power_opc_srea(regpointer gCPU, uint32 op)
{
	uint32 rS = MAKE_RD(op);
	uint32 rA = MAKE_RA(op);
	uint32 rB = MAKE_RB(op);
	uint32 mask = 0xFFFFFFFF;
	uint32 tmp = gCPU.gpr[rS];
	uint32 toRotate = (gCPU.gpr[rB] & 0x0000001F);
	tmp = (tmp >> toRotate) | (tmp << (32 - toRotate));
	for (int i = 31; i > toRotate; i--) {
		mask &= (~(1 << i));
	}
	mq = tmp;
	if (gCPU.gpr[rS] & 0x80000000) {
		gCPU.gpr[rA] = use_mask(0xFFFFFFFF, tmp, gCPU.gpr[rA]);
	}
	else {
		gCPU.gpr[rA] = use_mask(0, tmp, gCPU.gpr[rA]);
	}
	if (OPC_UPDATE_CRO(op)) {
		record(gCPU, gCPU.gpr[rA]);
	}
}

void power_opc_sreq(regpointer gCPU, uint32 op)
{
	uint32 rS = MAKE_RD(op);
	uint32 rA = MAKE_RA(op);
	uint32 rB = MAKE_RB(op);
	uint32 mask = 0xFFFFFFFF;
	uint32 tmp = gCPU.gpr[rS];
	uint32 toRotate = (gCPU.gpr[rB] & 0x0000001F);
	tmp = (tmp >> toRotate) | (tmp << (32 - toRotate));
	for (int i = 0; i > toRotate; i--) {
		mask &= (~(1 << i));
	}
	gCPU.gpr[rA] = use_mask(mask, tmp, mq);
	mq = tmp;
	if (OPC_UPDATE_CRO(gCPU.current_opc)) {
		record(gCPU, gCPU.gpr[rA]);
	}
}

void power_opc_srliq_sriq(regpointer gCPU, uint32 op)
{
	uint32 rS = MAKE_RD(op);
	uint32 rA = MAKE_RA(op);
	uint32 SH = MAKE_RB(op);
	uint32 mask = 0xFFFFFFFF;
	uint32 tmp = gCPU.gpr[rS];
	tmp = (tmp >> SH) | (tmp << (32 - SH));
	for (int i = 0; i > SH; i--) {
		mask &= (~(1 << i));
	}
	if ((gCPU.current_opc & 0x00000EFE >> 1) == 696){
		gCPU.gpr[rA] = (tmp & mask);
	}
	else {
		gCPU.gpr[rA] = use_mask(mask, tmp, mq);
	}
	mq = tmp;
	if(OPC_UPDATE_CRO(op)) {
		record(gCPU, gCPU.gpr[rA]);
	}
}
