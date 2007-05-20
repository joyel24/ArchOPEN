-stack 0x0100 /* size of .stack section */
-heap  0x0020 /* size of .sysmem section */

-o pico_dsp.out

MEMORY {
   PAGE 0: /* program memory */

	DARAM (RWX) : origin = 0x0100, length = 0x7E80

	/* boot interrupt vector table location */
	VECTORS (RWX): origin = 0x7F80, length = 0x80

   PAGE 0: /* heap memory */

	SARAM_D (RW) : origin = 0x8000, length = 0x3FFF

   PAGE 1:

	SARAM_C (X): origin = 0xC000, length = 0x3EFF

} /* MEMORY */

SECTIONS {
   .text    > DARAM   PAGE 0 /* code                     */
   .switch  > DARAM   PAGE 0 /* switch table info        */
   .cio     > DARAM   PAGE 0 /* C I/O                    */  
   .vectors > VECTORS PAGE 0 /* interrupt vectors        */
   .data    > SARAM_D PAGE 0 /* initialized data         */
   .bss     > SARAM_D   PAGE 0 /* global & static variables*/
   .const   > DARAM   PAGE 0 /* constant data            */
   .sysmem  > DARAM   PAGE 0 /* heap                     */
   .stack   > DARAM   PAGE 0 /* stack                    */
   .cinit   > DARAM   PAGE 0 /* cinit                    */
   .csldata > DARAM   PAGE 0
   .saram_d > SARAM_D PAGE 0 /* data saram               */
} /* SECTIONS */