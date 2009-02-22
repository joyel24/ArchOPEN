#include "MMU.h"
#include <kernel/kernel.h>

//#define DEBUG_MMU

#ifdef DEBUG_MMU
#define MMU_PRINT(s...)   printk(s)
#else
#define MMU_PRINT(s...)
#endif

unsigned int * MMU_GetTranslationTableBaseRef(){
  unsigned int * val;

  asm volatile(
      "mrc p15,0,%0,c2,c0,0\n"
  :"=r" (val));

  return (unsigned int *)((unsigned int)val & 0xFFFFC000);
}

unsigned int * MMU_GetFirstLevelDescriptorRef(unsigned int * address){
    return MMU_GetTranslationTableBaseRef()+((unsigned int)address>>20);
}

void MMU_SetSecondLevelDescriptor(unsigned int * virtaddr, unsigned int desc){
    unsigned int fld=*MMU_GetFirstLevelDescriptorRef(virtaddr);
    unsigned int type=desc&MMU_SLD_TYPE_MASK;
    unsigned int * descaddr;
    int i;

    switch (fld&MMU_FLD_TYPE_MASK){
        case MMU_FLD_TYPE_COARSE:
            descaddr=((unsigned int *)(fld&MMU_FLD_ADDRMASK_COARSE))+(((unsigned int)virtaddr&0xFFFFF)>>12);
            switch (type){
                case MMU_SLD_TYPE_FAULT:
                case MMU_SLD_TYPE_SMALL:
                    MMU_PRINT("[MMU] coarse fault/small mapping %x(%x) with %x\n",virtaddr,descaddr,desc);
                    *descaddr=desc;
                    return;
                case MMU_SLD_TYPE_LARGE:
                    MMU_PRINT("[MMU] coarse large mapping %x(%x) with %x\n",virtaddr,descaddr,desc);
                    for(i=0;i<16;++i) *(descaddr++)=desc;
                    return;
            }
            break;
        case MMU_FLD_TYPE_FINE:
            descaddr=((unsigned int *)(fld&MMU_FLD_ADDRMASK_FINE))+(((unsigned int)virtaddr&0xFFFFF)>>10);
            switch (type){
                case MMU_SLD_TYPE_FAULT:
                case MMU_SLD_TYPE_TINY:
                    MMU_PRINT("[MMU] fine fault/tiny mapping %x(%x) with %x\n",virtaddr,descaddr,desc);
                    *descaddr=desc;
                    return;
                case MMU_SLD_TYPE_SMALL:
                    MMU_PRINT("[MMU] fine small mapping %x(%x) with %x\n",virtaddr,descaddr,desc);
                    *(descaddr++)=desc;
                    *(descaddr++)=desc;
                    *(descaddr++)=desc;
                    *(descaddr++)=desc;
                    return;
                case MMU_SLD_TYPE_LARGE:
                    MMU_PRINT("[MMU] fine large mapping %x(%x) with %x\n",virtaddr,descaddr,desc);
                    for(i=0;i<64;++i) *(descaddr++)=desc;
                    return;
            }
            break;
    }

    printk("[MMU] %x : invalid descriptor for virtual address %p\n",desc,virtaddr);
}

unsigned int MMU_GetSecondLevelDescriptor(unsigned int * virtaddr){
    unsigned int fld=*MMU_GetFirstLevelDescriptorRef(virtaddr);
    unsigned int * descaddr;

    switch (fld&MMU_FLD_TYPE_MASK){
        case MMU_FLD_TYPE_COARSE:
            descaddr=((unsigned int *)(fld&MMU_FLD_ADDRMASK_COARSE))+(((unsigned int)virtaddr&0xFFFFF)>>12);
            return *descaddr;
        case MMU_FLD_TYPE_FINE:
            descaddr=((unsigned int *)(fld&MMU_FLD_ADDRMASK_FINE))+(((unsigned int)virtaddr&0xFFFFF)>>10);
            return *descaddr;
    }

    printk("[MMU] no second level descriptor for virtual address %p\n",virtaddr);
    return 0;
}


unsigned int GetDomainAccessControl(int domain){
    unsigned int res;

    asm volatile(
        "mrc p15,0,%0,c3,c0,0\n"
    : "=r" (res));

    MMU_PRINT("[MMU] DAC register %x\n",res);

    return (res>>(domain<<1))&0x3;
}

void SetDomainAccessControl(int domain,int ac){
    unsigned int acs;

    asm volatile(
        "mrc p15,0,%0,c3,c0,0\n"
    : "=r" (acs));

    acs&=~(0x3<<(domain<<1));

    acs|=ac<<(domain<<1);

    MMU_PRINT("[MMU] modified DAC register %x\n",acs);

    asm volatile(
        "mcr p15,0,%0,c3,c0,0\n"
    :: "r" (acs));
}
