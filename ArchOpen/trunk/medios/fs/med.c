/*
*   kernel/fs/med.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <lib/string.h>
#include <sys_def/types.h>
#include <sys_def/errors.h>

#include <kernel/kernel.h>
#include <kernel/io.h>
#include <kernel/malloc.h>

#include <driver/cache.h>

#include <fs/med.h>
#include <fs/stdfs.h>

#include <snd/sound.h>

//#define DO_MED_DEBUG

#ifdef DO_MED_DEBUG
#define DEBUG_MED(s...)   printk(s)
#else
#define DEBUG_MED(s...)
#endif

char * SDRAM_SECTIONS[] = {
    ".texte",
    ".ctor",
    ".data",
    ".bss",
    ".rodata",
    NULL
};

char * IRAM_SECTIONS[] = {
    ".iram",
    ".core",
    ".cored",
    NULL
};

char * BSS_SECTIONS[] = {
    ".bss",
    NULL
};


int cmp_string(char * str1,char * str2)
{
    while(*str1 && *str2)
    {
        if(*str1 != *str2)
            return 0;
        str1++;
        str2++;
    }
    return 1;
}

int test_section(char * name,char ** name_list)
{
    int i;
    int res = 0;
    for(i=0;name_list[i];i++)
    {
        if(cmp_string(name,name_list[i]))
            res = 1;
    }
    return res;
}

MED_RET_T med_load(char * file_name)
{
    return med_loadParam(1,&file_name);
}

MED_RET_T med_loadParam(int argc,char**argv)
{
    char ** cpy_argv;
    char path[MAX_PATH];
    char * pos;
    MED_RET_T ret_val;
    med_t medInfo;
    int i;
    int use_iram;
    int stack_type;

    if(argc<1)
    {
        return -MED_EINVAL;
    }

#ifdef DM320
    // DM320 iram is small and not faster than sdram
    use_iram=0;
#else
    // sound codecs use iram so make sure we are not using them
    use_iram=sound_state==SS_STOPPED;
#endif

    if(use_iram)
    {
        stack_type=THREAD_USE_SYS_STACK;
    }
    else
    {
        stack_type=THREAD_USE_OTHER_STACK;
    }

    if((ret_val=med_loadMed(argv[0],&medInfo,0,use_iram))!=MED_OK)
        return ret_val;
        
    if(use_iram){
        med_copyToIram(&medInfo);
    }

    /* creating tmp argv struct */
    if(argc)
    {
        int len;
        cpy_argv = (char**)malloc(argc*sizeof(char*));
        for(i=0;i<argc;i++)
        {
            len=strlen(argv[i])+1;
            cpy_argv[i] = (char*)malloc(len*sizeof(char));
            strncpy(cpy_argv[i],argv[i],len);
        }
    }
    else
        cpy_argv = NULL;
    
    
    /* creating path variable */    
    strcpy(path,argv[0]);
    pos=strrchr(argv[0], '/');

    *(strrchr(path, '/'))='\0';

    thread_startMed((void*)medInfo.entry,medInfo.sdram_start,(void*)medInfo.iram_ptr,stack_type,
                     strrchr(argv[0],'/')+1,path,argc,cpy_argv);
    
    printk("back from med\n");
    
    /* need to clean args */
    if(argc)
    {
        for(i=0;i<argc;i++)
            kfree(cpy_argv[i]);
        kfree(cpy_argv);
    }
    
    return MED_OK;
}

MED_RET_T med_loadMed(char*fName,med_t * medInfo,int file_offset,int use_iram)
{
    
    int fd,ret,i,j,k,res,res2,res1;
        
    MED_RET_T ret_val=MED_OK;
    
    //int (*run_med)(int argc,char**argv);
    
    elf_hdr header;
    
    section_hdr section;
    char * sections_name;
    
    section_t * section_list;
    
    uint32_t sdram_size=0;
    char * sdram_ptr=NULL;
    char * iram_sdram_ptr=NULL;
    uint32_t iram_size=0;
    int first_sdram=-1;
    int first_iram=-1;
    
    unsigned int diff; 
    
    medInfo->sdram_start=NULL;
    medInfo->iram_ptr=NULL;
    medInfo->entry=-1;
    
    fd = open(fName,O_RDONLY);
    
    if(fd<0)
    {
        printk("[load_med] Can't open file %s\n",fName);
        return -MED_EINVAL;
    }
     
    lseek(fd,file_offset,SEEK_SET);
    
    /* reading elf header */
    if((ret=read(fd,(void*)&header,sizeof(elf_hdr)))<sizeof(elf_hdr))
    {
        printk("[load_med] Can't read completly the header (read %d)\n",ret);
        ret_val = -MED_EINVAL;
        goto exit_point;
    }
    
    /* checking ELF Magic*/
    if(header.e_ident[0] != 0x7F || header.e_ident[1] != 0x45 || header.e_ident[2] != 0x4c || header.e_ident[3] != 0x46)
    {
        header.e_ident[4]=0;
        printk("[load_med] Wrong magic (%s)\n",header.e_ident);
        ret_val = -MED_EINVAL;
        goto exit_point;
    }
    /* reading all section headers from file*/
    section_list = (section_t *)malloc(sizeof(section_t)*header.e_shnum);
    if(!section_list)
    {
        printk("Error can't malloc an array of section header of %d elements\n",header.e_shnum);
        ret_val = -MED_ENOMEM;
        goto exit_point;
    }

    /* creating our own list of section infos */ 
    for(i=0;i<header.e_shnum;i++)
    {
        lseek (fd,file_offset+header.e_shoff+i*header.e_shentsize,SEEK_SET);
        read(fd,(void*)&section,sizeof(section_hdr));
        /* init from section header*/
        section_list[i].name = (char*)section.sh_name;
        section_list[i].vaddr = section.sh_addr;
        section_list[i].offset = section.sh_offset;
        section_list[i].size = section.sh_size;
        
        /* default value */
        section_list[i].addr = 0;
        section_list[i].type = MED_DISCARD;
        section_list[i].rel = NULL;
        section_list[i].opt = 0;
        section_list[i].nb_ent = 0;
               
        /* special processing for rel section */
        if(section.sh_type == 9)
        {
            section_list[i].opt = section.sh_info;
            section_list[i].type = MED_REL;
            section_list[i].nb_ent =section.sh_size/section.sh_entsize;
        }
    }
    
    /* reading section's name section */
    lseek (fd,file_offset+section_list[header.e_shstrndx].offset,SEEK_SET);    
    sections_name = (char*)malloc(sizeof(char)*section_list[header.e_shstrndx].size);
    if(!sections_name)
    {
        printk("Error can't malloc an array of char of %d elements for sections' names\n",section_list[header.e_shstrndx].size);
        ret_val = -MED_ENOMEM;
        goto exit_point1;
    }    
    res=read(fd,(void*)sections_name,sizeof(char)*section_list[header.e_shstrndx].size);
    
    /* parsing section list according to name */
    
    for(i=0;i<header.e_shnum;i++)
    {
        /* getting the real name */
        section_list[i].name=&sections_name[(uint32_t)section_list[i].name];
            
        /* discard all 0 sized section */
        if(section_list[i].size == 0x0)
        {
            section_list[i].type = MED_DISCARD;
            DEBUG_MED("Section %s will not be loaded, size=0\n",section_list[i].name);
            continue;
        }
            
        /* linking rel section to its section */
        if(section_list[i].type == MED_REL) /* rel section */
        {
            section_list[section_list[i].opt].rel = &section_list[i];
            DEBUG_MED("Section %s will not be loaded, rel section\n",section_list[i].name);
            continue;
        }
        
        /* is this a bss section ?*/
        if(test_section(section_list[i].name,BSS_SECTIONS))
            section_list[i].type = MED_BSS;
        
        /* standard processing */
        
        /* is this a SDRAM section */
        if(test_section(section_list[i].name,SDRAM_SECTIONS) || (!use_iram && test_section(section_list[i].name,IRAM_SECTIONS)))
        {
            DEBUG_MED("Section %s will be loaded in SDRAM size=%x\n",section_list[i].name,section_list[i].size);
            if(section_list[i].type!=MED_BSS)
                section_list[i].type = MED_BIT;
            section_list[i].dest = MED_DEST_SDRAM;
            sdram_size += section_list[i].size;
            diff = section_list[i].size % 32;
            if(diff)
                sdram_size+=(32-diff);
            if(first_sdram!=-1)
                first_sdram=i;
        }
        else if(test_section(section_list[i].name,IRAM_SECTIONS))
        {
            DEBUG_MED("Section %s will be loaded in IRAM size=%x\n",section_list[i].name,section_list[i].size);
            if(section_list[i].type!=MED_BSS)
                section_list[i].type = MED_BIT;
            section_list[i].dest = MED_DEST_IRAM;
            iram_size += section_list[i].size;
            diff = section_list[i].size % 32;
            if(diff)
                iram_size+=(32-diff);
            if(first_iram!=-1)
                first_iram=i;
        }
        else
        {
            DEBUG_MED("Section %s will not be loaded\n",section_list[i].name);
        }        
        
    }
        
    /* get mem space in IRAM and SDRAM */
        
    if((0x7000-(unsigned int)CORE_START)<iram_size)
    {        
        printk("Can't get a buffer for IRAM: avail %x need %x\n",(0x7000-(unsigned int)CORE_START),iram_size);
        ret_val = -MED_ENOMEM;
        goto exit_point2;
    }
    
    medInfo->iram_ptr=CORE_START;
    DEBUG_MED("Iram start: %x, size= %x\n",medInfo->iram_ptr,iram_size);
    
    sdram_ptr = medInfo->sdram_start = (char*)malloc(sdram_size+iram_size);
    
    iram_sdram_ptr = medInfo->iram_sdram_start = sdram_ptr+sdram_size;

    if(!sdram_ptr)
    {
        printk("Can't get a buffer in SDRAM size: %x\n",sdram_size);
        ret_val = -MED_ENOMEM;
        goto exit_point2;
    }
    else
        DEBUG_MED("buffer in SDRAM created; size: %x, start:%x\n",sdram_size,sdram_ptr);
    
    /* loading sections in mem */

    for(i=0;i<header.e_shnum;i++)
    {
        
        if(section_list[i].type == MED_DISCARD || section_list[i].type == MED_REL)
        {
            DEBUG_MED("[%d] %s not loaded (size=0x%x)\n",i,section_list[i].name,section_list[i].size);
            continue;
        }
        
        if(section_list[i].dest == MED_DEST_SDRAM)
        {
            section_list[i].addr = sdram_ptr;
            section_list[i].load_addr = sdram_ptr;
            sdram_ptr+=section_list[i].size;
            diff = section_list[i].size % 32;
            if(diff)
                sdram_ptr+=(32-diff);
        }
        else if(section_list[i].dest == MED_DEST_IRAM)
        {
            section_list[i].addr = medInfo->iram_ptr;
            medInfo->iram_ptr+=section_list[i].size;
            diff = section_list[i].size % 32;
            if(diff)
                medInfo->iram_ptr+=(32-diff);

            // iram data/code will be loaded first in sdram, then copied to iram when need be
            section_list[i].load_addr = iram_sdram_ptr;
            iram_sdram_ptr+=section_list[i].size;
            diff = section_list[i].size % 32;
            if(diff)
                iram_sdram_ptr+=(32-diff);
        }

        if(section_list[i].type == MED_BSS)
        {
            /* clearing bss */
            memset(section_list[i].addr,0,section_list[i].size);
            DEBUG_MED("[%d] %s created at 0x%x and cleared (size=0x%x)\n",i,section_list[i].name,
                section_list[i].addr,section_list[i].size);
        }
        else
        {
            /* loading section from disk */
            lseek (fd,file_offset+section_list[i].offset,SEEK_SET);
            res = read(fd,(void*)section_list[i].load_addr,section_list[i].size);
            DEBUG_MED("[%d] %s load at 0x%x, read %x/%x\n",i,section_list[i].name,
                section_list[i].addr,res,section_list[i].size);
        }           
        
    }
     
    /* now processing relocs */

    for(i=0;i<header.e_shnum;i++)
    {
        if(section_list[i].type != MED_DISCARD && section_list[i].type != MED_REL && section_list[i].rel)
        {
            rel_entry rel_data;
            uint32_t addr;
            uint32_t content;
            res=0;res2=0;res1=0;
            DEBUG_MED("we'll have to process %d relocs for %s - \n",section_list[i].rel->nb_ent,
                section_list[i].rel->name);
            for(j=0;j<section_list[i].rel->nb_ent;j++)
            {
                lseek(fd,file_offset+section_list[i].rel->offset+j*sizeof(rel_entry),SEEK_SET);
                read(fd,(void*)&rel_data,sizeof(rel_entry));

                addr = rel_data.r_offset-section_list[i].vaddr+(uint32_t)section_list[i].load_addr;
                content = inl(addr);

                if(ELF32_R_TYPE(rel_data.r_info) == 0x2)
                {
                    int found=0;
                    res++;

                    /* searching where is this addr */
                    for(k=0;k<header.e_shnum;k++)
                    {
                        if(section_list[k].type == MED_DISCARD || section_list[k].type == MED_REL)
                            continue;
                        if(section_list[k].vaddr<=content && (section_list[k].vaddr+section_list[k].size)>content)
                        {
                            outl(content-section_list[k].vaddr+(uint32_t)section_list[k].addr,addr);
                            res2++;
                            found=1;
                            /*DEBUG_MED("REL: from %x(%x), data %x changed to %x (in section %d start: %x(%x))\n",
                                rel_data.r_offset,addr,content,
                                content-section_list[k].vaddr+(uint32_t)section_list[k].addr,k,
                                section_list[k].vaddr,
                                (uint32_t)section_list[k].addr);*/
                        }
                    }
                    if(!found)
                        DEBUG_MED("REL: from %x(%x), data %x NOT FOUND\n",rel_data.r_offset,addr,content);
                }
                else if(ELF32_R_TYPE(rel_data.r_info)==1)
                {
                    int reladdr;

                    reladdr=content & 0x00ffffff;
                    if (reladdr & 0x00800000) reladdr |= 0xff000000; /* sign extend */
                    reladdr<<=2;

                    /* only process relocs that point out of the section */
                    if((rel_data.r_offset+reladdr+8)<(uint32_t)section_list[i].vaddr || (rel_data.r_offset+reladdr+8)>=((uint32_t)section_list[i].vaddr+section_list[i].size))
                    {
                        int found=0;

                        /* searching where is this addr */
                        for(k=0;k<header.e_shnum;k++)
                        {
                            if(section_list[k].type == MED_DISCARD || section_list[k].type == MED_REL)
                                continue;
                            if(section_list[k].vaddr<=(rel_data.r_offset+reladdr+8) && (section_list[k].vaddr+section_list[k].size)>(rel_data.r_offset+reladdr+8))
                            {
                                int srcaddr,loadsrcaddr,destaddr;

                                res2++;
                                found=1;

                                destaddr=rel_data.r_offset+reladdr-section_list[k].vaddr+(uint32_t)section_list[k].addr;

                                srcaddr=rel_data.r_offset-section_list[i].vaddr+(uint32_t)section_list[i].addr;

                                loadsrcaddr=rel_data.r_offset-section_list[i].vaddr+(uint32_t)section_list[i].load_addr;

                                reladdr=destaddr-srcaddr;

                                if((reladdr&3) || ((reladdr>0) && (reladdr&0xfc000000)) || ((reladdr<0) && (reladdr&0xfc000000)!=0xfc000000))
                                {
                                    DEBUG_MED("REL: from %x(%x), R_ARM_PC24 reloc out of range\n",rel_data.r_offset,addr);
                                }
                                else
                                {
                                    outl(((reladdr>>2)&0x00ffffff) | (content&0xff000000),loadsrcaddr);
                                }
                            }
                        }
                        if(!found)
                            DEBUG_MED("REL: from %x(%x), data %x NOT FOUND\n",rel_data.r_offset,addr,content);
                    }

                    res1++;
                }
                else
                    DEBUG_MED("REL: %x of type %d \n",rel_data.r_offset,ELF32_R_TYPE(rel_data.r_info));
            }
            DEBUG_MED("%d of type 1, %d were of type 2, %d were found, %d of other type\n",
                res1,res,res2,section_list[i].rel->nb_ent-res-res1);
        }
    }
    
    /* now finding where to start */
    for(k=0;k<header.e_shnum;k++) {
      //
      if (!strcmp(section_list[k].name, ".ctors")) {
 	DEBUG_MED( "CTOR %d", k);
	int j;
	for (j=0; j<section_list[k].size; j+=4) {
	  uint32_t * rout = *(uint32_t **)((section_list[k].addr)+j);
	  DEBUG_MED("CTOR -->%p [%p [%08x]]\n", section_list[k].addr, rout, *rout);
	  ((void (*)(void))rout)();
	}
      }

      if(section_list[k].vaddr<=header.e_entry && (section_list[k].vaddr+section_list[k].size)>header.e_entry)	{
        medInfo->entry=header.e_entry-section_list[k].vaddr+(uint32_t)section_list[k].addr;
        break;
      }
    }
    
    if(medInfo->entry==-1)
    {
        printk("can't find setion of entry point: 0x%x\n",header.e_entry);
        ret_val=-MED_EINVAL; 
        goto exit_point3;
    }
    else
        DEBUG_MED("entry point in section: %d\n",k);
           
    close(fd);
         
    DEBUG_MED("sdram  %x\n",medInfo->sdram_start); 
    DEBUG_MED("sections_name  %x\n",sections_name);
    DEBUG_MED("section_list  %x\n",section_list);
    
//    DEBUG_MED("calling app (entry %x)\n", run_med);

    /* invalidate caches */
    cache_invalidate(CACHE_ALL);

    /* freeing tmp malloc/struct */
    kfree(sections_name);
    kfree(section_list);
        
    return ret_val;

exit_point3:       
    kfree(medInfo->sdram_start);
    printk("sdram freed\n"); 
exit_point2:
    kfree(sections_name);
    printk("sections_name freed\n");
exit_point1:
    kfree(section_list);
    printk("sections_name freed\n");
exit_point:
    close(fd);
    return ret_val;
    
}

void med_copyToIram(med_t * medInfo)
{
    memcpy(CORE_START,medInfo->iram_sdram_start,medInfo->iram_ptr-CORE_START);

    med_iramTop=medInfo->iram_ptr;
}
