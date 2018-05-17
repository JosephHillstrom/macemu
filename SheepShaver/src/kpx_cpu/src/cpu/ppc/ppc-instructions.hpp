/*
 *  ppc-instructions.hpp - PowerPC instructions IDs
 *
 *  Kheperix (C) 2003-2005 Gwenole Beauchesne
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef PPC_INSTRUCTIONS_H
#define PPC_INSTRUCTIONS_H

/**
 *		Define PowerPC instruction types
 **/

#define PPC_I(X) powerpc_instruction_##X

enum powerpc_instruction {
	PPC_I(INVALID),
	PPC_I(ADD),
	PPC_I(ADDC),
	PPC_I(ADDE),
	PPC_I(ADDI),
	PPC_I(ADDIC),
	PPC_I(ADDIC_),
	PPC_I(ADDIS),
	PPC_I(ADDME),
	PPC_I(ADDZE),
	PPC_I(AND),
	PPC_I(ANDC),
	PPC_I(ANDI),
	PPC_I(ANDIS),
	PPC_I(B),
	PPC_I(BC),
	PPC_I(BCCTR),
	PPC_I(BCLR),
	PPC_I(CMP),
	PPC_I(CMPI),
	PPC_I(CMPL),
	PPC_I(CMPLI),
	PPC_I(CNTLZW),
	PPC_I(CRAND),
	PPC_I(CRANDC),
	PPC_I(CREQV),
	PPC_I(CRNAND),
	PPC_I(CRNOR),
	PPC_I(CROR),
	PPC_I(CRORC),
	PPC_I(CRXOR),
	PPC_I(DCBA),
	PPC_I(DCBF),
	PPC_I(DCBI),
	PPC_I(DCBST),
	PPC_I(DCBT),
	PPC_I(DCBTST),
	PPC_I(DCBZ),
	PPC_I(DIVW),
	PPC_I(DIVWU),
	PPC_I(DSS),
	PPC_I(DST),
	PPC_I(DSTST),
	PPC_I(ECIWX),
	PPC_I(ECOWX),
	PPC_I(EIEIO),
	PPC_I(EQV),
	PPC_I(EXTSB),
	PPC_I(EXTSH),
	PPC_I(FABS),
	PPC_I(FADD),
	PPC_I(FADDS),
	PPC_I(FCMPO),
	PPC_I(FCMPU),
	PPC_I(FCTIW),
	PPC_I(FCTIWZ),
	PPC_I(FDIV),
	PPC_I(FDIVS),
	PPC_I(FMADD),
	PPC_I(FMADDS),
	PPC_I(FMR),
	PPC_I(FMSUB),
	PPC_I(FMSUBS),
	PPC_I(FMUL),
	PPC_I(FMULS),
	PPC_I(FNABS),
	PPC_I(FNEG),
	PPC_I(FNMADD),
	PPC_I(FNMADDS),
	PPC_I(FNMSUB),
	PPC_I(FNMSUBS),
	PPC_I(FRSP),
	PPC_I(FSEL),
	PPC_I(FSUB),
	PPC_I(FSUBS),
	PPC_I(ICBI),
	PPC_I(ISYNC),
	PPC_I(LBZ),
	PPC_I(LBZU),
	PPC_I(LBZUX),
	PPC_I(LBZX),
	PPC_I(LFD),
	PPC_I(LFDU),
	PPC_I(LFDUX),
	PPC_I(LFDX),
	PPC_I(LFS),
	PPC_I(LFSU),
	PPC_I(LFSUX),
	PPC_I(LFSX),
	PPC_I(LHA),
	PPC_I(LHAU),
	PPC_I(LHAUX),
	PPC_I(LHAX),
	PPC_I(LHBRX),
	PPC_I(LHZ),
	PPC_I(LHZU),
	PPC_I(LHZUX),
	PPC_I(LHZX),
	PPC_I(LMW),
	PPC_I(LSWI),
	PPC_I(LSWX),
	PPC_I(LVEBX),
	PPC_I(LVEHX),
	PPC_I(LVEWX),
	PPC_I(LVSL),
	PPC_I(LVSR),
	PPC_I(LVX),
	PPC_I(LVXL),
	PPC_I(LWARX),
	PPC_I(LWBRX),
	PPC_I(LWZ),
	PPC_I(LWZU),
	PPC_I(LWZUX),
	PPC_I(LWZX),
	PPC_I(MCRF),
	PPC_I(MCRFS),
	PPC_I(MCRXR),
	PPC_I(MFCR),
	PPC_I(MFFS),
	PPC_I(MFMSR),
	PPC_I(MFSPR),
	PPC_I(MFTB),
	PPC_I(MFVSCR),
	PPC_I(MTCRF),
	PPC_I(MTFSB0),
	PPC_I(MTFSB1),
	PPC_I(MTFSF),
	PPC_I(MTFSFI),
	PPC_I(MTSPR),
	PPC_I(MTVSCR),
	PPC_I(MULHW),
	PPC_I(MULHWU),
	PPC_I(MULLI),
	PPC_I(MULLW),
	PPC_I(NAND),
	PPC_I(NEG),
	PPC_I(NOR),
	PPC_I(OR),
	PPC_I(ORC),
	PPC_I(ORI),
	PPC_I(ORIS),
	PPC_I(RLWIMI),
	PPC_I(RLWINM),
	PPC_I(RLWNM),
	PPC_I(SC),
	PPC_I(SLW),
	PPC_I(SRAW),
	PPC_I(SRAWI),
	PPC_I(SRW),
	PPC_I(STB),
	PPC_I(STBU),
	PPC_I(STBUX),
	PPC_I(STBX),
	PPC_I(STFD),
	PPC_I(STFDU),
	PPC_I(STFDUX),
	PPC_I(STFDX),
	PPC_I(STFS),
	PPC_I(STFSU),
	PPC_I(STFSUX),
	PPC_I(STFSX),
	PPC_I(STH),
	PPC_I(STHBRX),
	PPC_I(STHU),
	PPC_I(STHUX),
	PPC_I(STHX),
	PPC_I(STMW),
	PPC_I(STSWI),
	PPC_I(STSWX),
	PPC_I(STVEBX),
	PPC_I(STVEHX),
	PPC_I(STVEWX),
	PPC_I(STVX),
	PPC_I(STVXL),
	PPC_I(STW),
	PPC_I(STWBRX),
	PPC_I(STWCX),
	PPC_I(STWU),
	PPC_I(STWUX),
	PPC_I(STWX),
	PPC_I(SUBF),
	PPC_I(SUBFC),
	PPC_I(SUBFE),
	PPC_I(SUBFIC),
	PPC_I(SUBFME),
	PPC_I(SUBFZE),
	PPC_I(SYNC),
	PPC_I(XOR),
	PPC_I(XORI),
	PPC_I(XORIS),
	PPC_I(VADDCUW),
	PPC_I(VADDFP),
	PPC_I(VADDSBS),
	PPC_I(VADDSHS),
	PPC_I(VADDSWS),
	PPC_I(VADDUBM),
	PPC_I(VADDUBS),
	PPC_I(VADDUHM),
	PPC_I(VADDUHS),
	PPC_I(VADDUWM),
	PPC_I(VADDUWS),
	PPC_I(VAND),
	PPC_I(VANDC),
	PPC_I(VAVGSB),
	PPC_I(VAVGSH),
	PPC_I(VAVGSW),
	PPC_I(VAVGUB),
	PPC_I(VAVGUH),
	PPC_I(VAVGUW),
	PPC_I(VCFSX),
	PPC_I(VCFUX),
	PPC_I(VCMPBFP),
	PPC_I(VCMPEQFP),
	PPC_I(VCMPEQUB),
	PPC_I(VCMPEQUH),
	PPC_I(VCMPEQUW),
	PPC_I(VCMPGEFP),
	PPC_I(VCMPGTFP),
	PPC_I(VCMPGTSB),
	PPC_I(VCMPGTSH),
	PPC_I(VCMPGTSW),
	PPC_I(VCMPGTUB),
	PPC_I(VCMPGTUH),
	PPC_I(VCMPGTUW),
	PPC_I(VCTSXS),
	PPC_I(VCTUXS),
	PPC_I(VEXPTEFP),
	PPC_I(VLOGEFP),
	PPC_I(VMADDFP),
	PPC_I(VMAXFP),
	PPC_I(VMAXSB),
	PPC_I(VMAXSH),
	PPC_I(VMAXSW),
	PPC_I(VMAXUB),
	PPC_I(VMAXUH),
	PPC_I(VMAXUW),
	PPC_I(VMHADDSHS),
	PPC_I(VMHRADDSHS),
	PPC_I(VMINFP),
	PPC_I(VMINSB),
	PPC_I(VMINSH),
	PPC_I(VMINSW),
	PPC_I(VMINUB),
	PPC_I(VMINUH),
	PPC_I(VMINUW),
	PPC_I(VMLADDUHM),
	PPC_I(VMRGHB),
	PPC_I(VMRGHH),
	PPC_I(VMRGHW),
	PPC_I(VMRGLB),
	PPC_I(VMRGLH),
	PPC_I(VMRGLW),
	PPC_I(VMSUMMBM),
	PPC_I(VMSUMSHM),
	PPC_I(VMSUMSHS),
	PPC_I(VMSUMUBM),
	PPC_I(VMSUMUHM),
	PPC_I(VMSUMUHS),
	PPC_I(VMULESB),
	PPC_I(VMULESH),
	PPC_I(VMULEUB),
	PPC_I(VMULEUH),
	PPC_I(VMULOSB),
	PPC_I(VMULOSH),
	PPC_I(VMULOUB),
	PPC_I(VMULOUH),
	PPC_I(VNMSUBFP),
	PPC_I(VNOR),
	PPC_I(VOR),
	PPC_I(VPERM),
	PPC_I(VPKPX),
	PPC_I(VPKSHSS),
	PPC_I(VPKSHUS),
	PPC_I(VPKSWSS),
	PPC_I(VPKSWUS),
	PPC_I(VPKUHUM),
	PPC_I(VPKUHUS),
	PPC_I(VPKUWUM),
	PPC_I(VPKUWUS),
	PPC_I(VREFP),
	PPC_I(VRFIM),
	PPC_I(VRFIN),
	PPC_I(VRFIP),
	PPC_I(VRFIZ),
	PPC_I(VRLB),
	PPC_I(VRLH),
	PPC_I(VRLW),
	PPC_I(VRSQRTEFP),
	PPC_I(VSEL),
	PPC_I(VSL),
	PPC_I(VSLB),
	PPC_I(VSLDOI),
	PPC_I(VSLH),
	PPC_I(VSLO),
	PPC_I(VSLW),
	PPC_I(VSPLTB),
	PPC_I(VSPLTH),
	PPC_I(VSPLTISB),
	PPC_I(VSPLTISH),
	PPC_I(VSPLTISW),
	PPC_I(VSPLTW),
	PPC_I(VSR),
	PPC_I(VSRAB),
	PPC_I(VSRAH),
	PPC_I(VSRAW),
	PPC_I(VSRB),
	PPC_I(VSRH),
	PPC_I(VSRO),
	PPC_I(VSRW),
	PPC_I(VSUBCUW),
	PPC_I(VSUBFP),
	PPC_I(VSUBSBS),
	PPC_I(VSUBSHS),
	PPC_I(VSUBSWS),
	PPC_I(VSUBUBM),
	PPC_I(VSUBUBS),
	PPC_I(VSUBUHM),
	PPC_I(VSUBUHS),
	PPC_I(VSUBUWM),
	PPC_I(VSUBUWS),
	PPC_I(VSUMSWS),
	PPC_I(VSUM2SWS),
	PPC_I(VSUM4SBS),
	PPC_I(VSUM4SHS),
	PPC_I(VSUM4UBS),
	PPC_I(VUPKHPX),
	PPC_I(VUPKHSB),
	PPC_I(VUPKHSH),
	PPC_I(VUPKLPX),
	PPC_I(VUPKLSB),
	PPC_I(VUPKLSH),
	PPC_I(VXOR),
	power_dozi,
	power_maskg,
	power_abs,
	power_clcs,
	power_doz,
	power_lscbx,
	power_maskir,
	PPC_I(MAX) // Total number of instruction types
};

#endif /* PPC_INSTRUCTIONS_H */
