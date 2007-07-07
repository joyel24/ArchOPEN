/*
*   kernel/core/dsp.c
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>

#include <kernel/io.h>
#include <kernel/kernel.h>

#include <driver/dsp.h>

#include <fs/stdfs.h>

#include <kernel/malloc.h>

//#define DSP_PRINT

#ifdef DEBUG_DSP
#define DSP_PRINT(s...)   printk(s)
#else
#define DSP_PRINT(s...)
#endif

void dsp_on ()
{
	DSP_PRINT ("[dsp] Enabled...\n");
#if 0
    outw (inw(MOD1_REG) & 0x9FFF, MOD1_REG);
	outw (inw(MOD1_REG) | 0x6000, MOD1_REG);
	outw (inw(MOD2_REG) & 0xFF1F, MOD2_REG);
	outw (inw(MOD2_REG) | 0x00E0, MOD2_REG);
#endif
}

void dsp_off ()
{
	DSP_PRINT ("[dsp] Disabled...\n");
#if 0
	outw (inw(MOD1_REG) & 0x9FFF, MOD1_REG);
	outw (inw(MOD2_REG) & 0xFF1F, MOD2_REG);
#endif
}

void dsp_clear ()
{
	int i;
	for (i=0x00040000;i<0x00050000;i+=2)
		outw (0, i);
}

void dsp_reset ()
{
	DSP_PRINT ("[dsp] Resetting...\n");
	outw (inw (HPIBCTL) & (0xFFFFFFFF - 256), HPIBCTL);
	outw (inw (HPIBCTL) | 256, HPIBCTL);
}

void dsp_run ()
{
	DSP_PRINT ("[dsp] Sending INT0...\n");
	outw (inw (HPIBCTL) & (0xFFFFFFFF - 128), HPIBCTL);
	outw (inw (HPIBCTL) | 128, HPIBCTL);
}

MED_RET_T dsp_loadProgramFromHDD (char* pszFilename)
{
	unsigned char * pDSPCode = 0;
	int fDSPCode = open (pszFilename, O_RDONLY);
	int nSize;
	DSP_PRINT ("[dsp] Loading program from hdd into sdram...\n");
	if (fDSPCode < 0)
	{
		DSP_PRINT ("[dsp] Program not loaded.\n");
		return -MED_ENOENT;
	}
	else
	{
		int nReaded;
		nSize = filesize (fDSPCode);
		pDSPCode = malloc (nSize);
		nReaded = read (fDSPCode, pDSPCode, nSize);
		DSP_PRINT ("[dsp] Program loaded into sdram (%d bytes)\n", nReaded);
		close (fDSPCode);
	}

	dsp_loadProgramFromMemory (pDSPCode, nSize);

	free (pDSPCode);
	return MED_OK;
}

MED_RET_T dsp_loadProgramFromMemory (void* pDSPCode, int nSize)
{
	DSP_PRINT ("[dsp] Loading program into dsp...\n");

	dsp_on ();
	dsp_reset ();
	dsp_clear ();

	COFF_FILHDR* phdr = (COFF_FILHDR*)pDSPCode;
	DSP_PRINT ("[dsp] %i sections found...\r\n", phdr->f_nscns);

	int pos = sizeof(COFF_FILHDR) + phdr->f_opthdr + 2;

	int i, j;
	char sec_name [9];
	sec_name [8] = 0;

	for (i=0;i<phdr->f_nscns;i++)
	{
		COFF_SCNHDR* psec = (COFF_SCNHDR*)(pDSPCode + pos);
		pos += 0x30;

		sec_name[0] = psec->s_name[0];
		sec_name[1] = psec->s_name[1];
		sec_name[2] = psec->s_name[2];
		sec_name[3] = psec->s_name[3];
		sec_name[4] = psec->s_name[4];
		sec_name[5] = psec->s_name[5];
		sec_name[6] = psec->s_name[6];
		sec_name[7] = psec->s_name[7];

		unsigned long nVirtAddress = psec->s_vaddr_lo/* + (((int)psec->s_page) << 16)*/;
		unsigned long nSectSize    = psec->s_size_lo;

		if (!nVirtAddress || !nSectSize)
		{
			DSP_PRINT ("[dsp] Skipping invalid section...\n");
			continue;
		}

		DSP_PRINT ("[dsp] Loading section %s (addr: 0x%.8X, size: 0x%.4X words)...\n",
			sec_name,
			nVirtAddress,
			nSectSize);

		if (!strcmp(sec_name, ".bss"))
		{
			for (j=0;j<psec->s_size_lo;j++)
			{
				outw (0, 0x00040000 + (nVirtAddress + j)*2);
			}
		}
		else
		{
			unsigned long fileOffset=psec->s_scnptr_lo | psec->s_scnptr_hi<<16;
			unsigned char* pSrcCode = (unsigned char*)(pDSPCode + fileOffset);
			for (j=0;j<psec->s_size_lo*2;j+=2)
			  {
			    outw (pSrcCode[j] | pSrcCode[j+1]<<8 , 0x00040000 + nVirtAddress*2 + j);
			  }
		}
	}

	DSP_PRINT ("[dsp] Program loaded.\n");
	return MED_OK;
}

int dsp_getChipNum(){
#if defined(DM270)
    return 27;
#elif defined(DM320)
    return 32;
#elif defined(DSC25)
    return 25;
#elif defined(DSC21)
    return 21;
#else
    return 0;
#endif
}

void dsp_write32(volatile void * p, unsigned long value)
{
    unsigned short * ptr = ( unsigned short *) p;
    ptr[1] = value;
    ptr[0] = value>>16;
}
