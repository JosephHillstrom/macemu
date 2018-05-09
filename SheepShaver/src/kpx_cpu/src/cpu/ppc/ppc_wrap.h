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
void bcctr_idiot(powerpc_cpu * cpu, uint32 op)
{
    bcctr(c_registers, op);
    (*(c_registers.pc)) += 4;
    
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
#endif
#endif
