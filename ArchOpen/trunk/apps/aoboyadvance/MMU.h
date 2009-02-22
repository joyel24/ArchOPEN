#ifndef __MMU_H_
#define __MMU_H_

#define MMU_CACHABLE                   0x8
#define MMU_BUFFERABLE                 0x4

#define MMU_FLD_BASE                   0x10

#define MMU_FLD_TYPE_MASK              0x3
#define MMU_FLD_TYPE_FAULT             0x0
#define MMU_FLD_TYPE_COARSE            0x1
#define MMU_FLD_TYPE_SECTION           0x2
#define MMU_FLD_TYPE_FINE              0x3

#define MMU_FLD_ADDRMASK_COARSE        0xFFFFFC00
#define MMU_FLD_ADDRMASK_SECTION       0xFFF00000
#define MMU_FLD_ADDRMASK_FINE          0xFFFFF000

#define MMU_DOMAIN(dom) ((dom)<<5)

#define MMU_SLD_TYPE_MASK              0x3
#define MMU_SLD_TYPE_FAULT             0x0
#define MMU_SLD_TYPE_LARGE             0x1
#define MMU_SLD_TYPE_SMALL             0x2
#define MMU_SLD_TYPE_TINY              0x3

#define MMU_SLD_ADDRMASK_LARGE         0xFFFF0000
#define MMU_SLD_ADDRMASK_SMALL         0xFFFFF000
#define MMU_SLD_ADDRMASK_TINY          0xFFFFFC00

#define MMU_DOMAIN_NOACCESS            0x0
#define MMU_DOMAIN_CLIENT              0x1
#define MMU_DOMAIN_MANAGER             0x3


unsigned int * MMU_GetTranslationTableBaseRef();
unsigned int * MMU_GetFirstLevelDescriptorRef(unsigned int * address);
void MMU_SetSecondLevelDescriptor(unsigned int * virtaddr, unsigned int desc);
unsigned int MMU_GetSecondLevelDescriptor(unsigned int * virtaddr);
unsigned int GetDomainAccessControl(int domain);
void SetDomainAccessControl(int domain,int ac);

#endif
