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
regpointer make_regptr(struct powerpc_registers * regs);
static void bcctr_idiot(powerpc_cpu * cpu, uint32 op)
{
	(*(c_registers.pc)) += 4;
	bcctr(c_registers, op);
    
}
void init_c_registers(powerpc_registers & regs);
struct idiot : nv_mem_fun1_t< void, powerpc_cpu, uint32 > {
public:

    idiot()
    {
        pf = bcctr_idiot;
    }
};
idiot bcctr_cpp;

static void dozi_wrapper(powerpc_cpu * cpu, uint32 op)
{
	(*(c_registers.pc)) += 4;
	dozi(c_registers, op);
}
struct dozi_struct : nv_mem_fun1_t<void, powerpc_cpu, uint32> {
public:
	dozi_struct()
	{
		pf = dozi_wrapper;
	}
};
dozi_struct dozi_cpp;

static void maskg_wrapper(powerpc_cpu * cpu, uint32 op)
{
    (*(c_registers.pc)) += 4;
    maskg(c_registers, op);
}
struct maskg_struct : nv_mem_fun1_t<void, powerpc_cpu, uint32> {
public:
    maskg_struct()
    {
        pf = maskg_wrapper;
    }
};
maskg_struct maskg_cpp;

static void abs_wrapper(powerpc_cpu * cpu, uint32 op)
{
    (*(c_registers.pc)) += 4;
    power_opc_abs(c_registers, op);
}
struct abs_struct : nv_mem_fun1_t<void, powerpc_cpu, uint32> {
public:
    abs_struct()
    {
        pf = abs_wrapper;
    }
};
abs_struct abs_cpp;

static void clcs_wrapper(powerpc_cpu * cpu, uint32 op)
{
    (*(c_registers.pc)) += 4;
    power_opc_clcs(c_registers, op);
}
struct clcs_struct : nv_mem_fun1_t<void, powerpc_cpu, uint32> {
public:
    clcs_struct()
    {
        pf = clcs_wrapper;
    }
};
clcs_struct clcs_cpp;

static void doz_wrapper(powerpc_cpu * cpu, uint32 op)
{
    (*(c_registers.pc)) += 4;
    power_opc_doz(c_registers, op);
}
struct doz_struct : nv_mem_fun1_t<void, powerpc_cpu, uint32> {
public:
    doz_struct()
    {
        pf = doz_wrapper;
    }
};
doz_struct doz_cpp;

static void lscbx_wrapper(powerpc_cpu * cpu, uint32 op)
{
	(*(c_registers.pc)) += 4;
	power_opc_lscbx(c_registers, op);
}
struct lscbx_struct : nv_mem_fun1_t<void, powerpc_cpu, uint32> {
public:
	doz_struct()
	{
		pf = lscbx_wrapper;
	}
};
lscbx_struct lscbx_cpp;
#endif
#endif
