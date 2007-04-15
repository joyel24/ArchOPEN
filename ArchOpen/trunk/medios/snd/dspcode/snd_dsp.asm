;***************************************************************
;* TMS320C54x C/C++ Codegen                          PC v4.1.0 *
;* Date/Time created: Sat Mar 31 16:55:52 2007                 *
;***************************************************************
	.compiler_opts --mem_model:code=near --mem_model:data=flat --silicon_version=549 
	.mmregs
FP	.set	AR7
	.c_mode

DW$CU	.dwtag  DW_TAG_compile_unit
	.dwattr DW$CU, DW_AT_name("snd_dsp.c")
	.dwattr DW$CU, DW_AT_producer("TMS320C54x C/C++ Codegen PC v4.1.0 Copyright (c) 1996-2005 Texas Instruments Incorporated")
	.dwattr DW$CU, DW_AT_stmt_list(0x00)
	.dwattr DW$CU, DW_AT_TI_VERSION(0x01)
;**************************************************************
;* CINIT RECORDS                                              *
;**************************************************************
	.sect	".cinit"
	.align	1
	.field  	1,16
	.field  	_aic23_port+0,16
	.field  	65535,16			; _aic23_port @ 0

	.sect	".cinit"
	.align	1
	.field  	1,16
	.field  	_aic23_dma+0,16
	.field  	65535,16			; _aic23_dma @ 0

	.sect	".cinit"
	.align	1
	.field  	1,16
	.field  	_outBuf+0,16
	.field  	_outBufData,16		; _outBuf @ 0

	.sect	".cinit"
	.align	1
	.field  	1,16
	.field  	_outBuf2+0,16
	.field  	_outBuf2Data,16		; _outBuf2 @ 0

	.sect	".cinit"
	.align	1
	.field  	1,16
	.field  	_outBufSize+0,16
	.field  	0,16			; _outBufSize @ 0

	.sect	".cinit"
	.align	1
	.field  	1,16
	.field  	_outBuf2Size+0,16
	.field  	0,16			; _outBuf2Size @ 0

	.sect	".cinit"
	.align	1
	.field  	1,16
	.field  	_dmaActive+0,16
	.field  	0,16			; _dmaActive @ 0


DW$1	.dwtag  DW_TAG_subprogram, DW_AT_name("_IRQ_dat2Prg"), DW_AT_symbol_name("__IRQ_dat2Prg")
	.dwattr DW$1, DW_AT_declaration(0x01)
	.dwattr DW$1, DW_AT_external(0x01)
DW$2	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$32)
DW$3	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$24)
	.dwendtag DW$1


DW$4	.dwtag  DW_TAG_subprogram, DW_AT_name("IRQ_globalDisable"), DW_AT_symbol_name("_IRQ_globalDisable")
	.dwattr DW$4, DW_AT_type(*DW$T$10)
	.dwattr DW$4, DW_AT_declaration(0x01)
	.dwattr DW$4, DW_AT_external(0x01)

DW$5	.dwtag  DW_TAG_subprogram, DW_AT_name("IRQ_globalEnable"), DW_AT_symbol_name("_IRQ_globalEnable")
	.dwattr DW$5, DW_AT_type(*DW$T$10)
	.dwattr DW$5, DW_AT_declaration(0x01)
	.dwattr DW$5, DW_AT_external(0x01)

DW$6	.dwtag  DW_TAG_subprogram, DW_AT_name("IRQ_globalRestore"), DW_AT_symbol_name("_IRQ_globalRestore")
	.dwattr DW$6, DW_AT_declaration(0x01)
	.dwattr DW$6, DW_AT_external(0x01)
DW$7	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$10)
	.dwendtag DW$6


DW$8	.dwtag  DW_TAG_subprogram, DW_AT_name("CSL_init"), DW_AT_symbol_name("_CSL_init")
	.dwattr DW$8, DW_AT_declaration(0x01)
	.dwattr DW$8, DW_AT_external(0x01)

DW$9	.dwtag  DW_TAG_subprogram, DW_AT_name("DMA_open"), DW_AT_symbol_name("_DMA_open")
	.dwattr DW$9, DW_AT_type(*DW$T$43)
	.dwattr DW$9, DW_AT_declaration(0x01)
	.dwattr DW$9, DW_AT_external(0x01)
DW$10	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$10)
DW$11	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$19)
	.dwendtag DW$9


DW$12	.dwtag  DW_TAG_subprogram, DW_AT_name("armInt_trigger"), DW_AT_symbol_name("_armInt_trigger")
	.dwattr DW$12, DW_AT_declaration(0x01)
	.dwattr DW$12, DW_AT_external(0x01)

DW$13	.dwtag  DW_TAG_subprogram, DW_AT_name("hpiDma_start"), DW_AT_symbol_name("_hpiDma_start")
	.dwattr DW$13, DW_AT_declaration(0x01)
	.dwattr DW$13, DW_AT_external(0x01)
DW$14	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$13)
DW$15	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$3)
DW$16	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$9)
DW$17	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$8)
	.dwendtag DW$13


DW$18	.dwtag  DW_TAG_subprogram, DW_AT_name("hpiDma_pending"), DW_AT_symbol_name("_hpiDma_pending")
	.dwattr DW$18, DW_AT_type(*DW$T$8)
	.dwattr DW$18, DW_AT_declaration(0x01)
	.dwattr DW$18, DW_AT_external(0x01)

DW$19	.dwtag  DW_TAG_subprogram, DW_AT_name("aic23_openPort"), DW_AT_symbol_name("_aic23_openPort")
	.dwattr DW$19, DW_AT_type(*DW$T$40)
	.dwattr DW$19, DW_AT_declaration(0x01)
	.dwattr DW$19, DW_AT_external(0x01)

DW$20	.dwtag  DW_TAG_subprogram, DW_AT_name("libDsp_init"), DW_AT_symbol_name("_libDsp_init")
	.dwattr DW$20, DW_AT_declaration(0x01)
	.dwattr DW$20, DW_AT_external(0x01)
DW$21	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$10)
	.dwendtag DW$20

	.global	_dspComBuffer
	.bss	_dspComBuffer,270,0,2
DW$22	.dwtag  DW_TAG_variable, DW_AT_name("dspComBuffer"), DW_AT_symbol_name("_dspComBuffer")
	.dwattr DW$22, DW_AT_location[DW_OP_addr _dspComBuffer]
	.dwattr DW$22, DW_AT_type(*DW$T$102)
	.dwattr DW$22, DW_AT_external(0x01)
	.global	_dspCom
	.bss	_dspCom,1,0,0
DW$23	.dwtag  DW_TAG_variable, DW_AT_name("dspCom"), DW_AT_symbol_name("_dspCom")
	.dwattr DW$23, DW_AT_location[DW_OP_addr _dspCom]
	.dwattr DW$23, DW_AT_type(*DW$T$103)
	.dwattr DW$23, DW_AT_external(0x01)
	.global	_aic23_port
	.bss	_aic23_port,1,0,0
DW$24	.dwtag  DW_TAG_variable, DW_AT_name("aic23_port"), DW_AT_symbol_name("_aic23_port")
	.dwattr DW$24, DW_AT_location[DW_OP_addr _aic23_port]
	.dwattr DW$24, DW_AT_type(*DW$T$40)
	.dwattr DW$24, DW_AT_external(0x01)
	.global	_aic23_dma
	.bss	_aic23_dma,1,0,0
DW$25	.dwtag  DW_TAG_variable, DW_AT_name("aic23_dma"), DW_AT_symbol_name("_aic23_dma")
	.dwattr DW$25, DW_AT_location[DW_OP_addr _aic23_dma]
	.dwattr DW$25, DW_AT_type(*DW$T$43)
	.dwattr DW$25, DW_AT_external(0x01)
	.global	_processBuf
_processBuf:	.usect	".saram_d",2048,0,0
DW$26	.dwtag  DW_TAG_variable, DW_AT_name("processBuf"), DW_AT_symbol_name("_processBuf")
	.dwattr DW$26, DW_AT_location[DW_OP_addr _processBuf]
	.dwattr DW$26, DW_AT_type(*DW$T$60)
	.dwattr DW$26, DW_AT_external(0x01)
	.global	_outBufData
	.bss	_outBufData,4096,0,0
DW$27	.dwtag  DW_TAG_variable, DW_AT_name("outBufData"), DW_AT_symbol_name("_outBufData")
	.dwattr DW$27, DW_AT_location[DW_OP_addr _outBufData]
	.dwattr DW$27, DW_AT_type(*DW$T$61)
	.dwattr DW$27, DW_AT_external(0x01)
	.global	_outBuf2Data
	.bss	_outBuf2Data,4096,0,0
DW$28	.dwtag  DW_TAG_variable, DW_AT_name("outBuf2Data"), DW_AT_symbol_name("_outBuf2Data")
	.dwattr DW$28, DW_AT_location[DW_OP_addr _outBuf2Data]
	.dwattr DW$28, DW_AT_type(*DW$T$61)
	.dwattr DW$28, DW_AT_external(0x01)
	.global	_outBuf
	.bss	_outBuf,1,0,0
DW$29	.dwtag  DW_TAG_variable, DW_AT_name("outBuf"), DW_AT_symbol_name("_outBuf")
	.dwattr DW$29, DW_AT_location[DW_OP_addr _outBuf]
	.dwattr DW$29, DW_AT_type(*DW$T$62)
	.dwattr DW$29, DW_AT_external(0x01)
	.global	_outBuf2
	.bss	_outBuf2,1,0,0
DW$30	.dwtag  DW_TAG_variable, DW_AT_name("outBuf2"), DW_AT_symbol_name("_outBuf2")
	.dwattr DW$30, DW_AT_location[DW_OP_addr _outBuf2]
	.dwattr DW$30, DW_AT_type(*DW$T$62)
	.dwattr DW$30, DW_AT_external(0x01)
	.global	_outBufSize
	.bss	_outBufSize,1,0,0
DW$31	.dwtag  DW_TAG_variable, DW_AT_name("outBufSize"), DW_AT_symbol_name("_outBufSize")
	.dwattr DW$31, DW_AT_location[DW_OP_addr _outBufSize]
	.dwattr DW$31, DW_AT_type(*DW$T$9)
	.dwattr DW$31, DW_AT_external(0x01)
	.global	_outBuf2Size
	.bss	_outBuf2Size,1,0,0
DW$32	.dwtag  DW_TAG_variable, DW_AT_name("outBuf2Size"), DW_AT_symbol_name("_outBuf2Size")
	.dwattr DW$32, DW_AT_location[DW_OP_addr _outBuf2Size]
	.dwattr DW$32, DW_AT_type(*DW$T$9)
	.dwattr DW$32, DW_AT_external(0x01)
	.global	_dmaActive
	.bss	_dmaActive,1,0,0
DW$33	.dwtag  DW_TAG_variable, DW_AT_name("dmaActive"), DW_AT_symbol_name("_dmaActive")
	.dwattr DW$33, DW_AT_location[DW_OP_addr _dmaActive]
	.dwattr DW$33, DW_AT_type(*DW$T$8)
	.dwattr DW$33, DW_AT_external(0x01)
;	C:\CCStudio_v3.3\C5400\cgtools\bin\opt500.exe --gen_opt_info=2 C:\DOCUME~1\thomas\LOCALS~1\Temp\TI2362 C:\DOCUME~1\thomas\LOCALS~1\Temp\TI2364 --opt_info_filename=Z:/Av320/ArchOpen/trunk/medios/snd/dspcode/Release/snd_dsp.nfo 
	.sect	".text"
	.global	_startDma

DW$34	.dwtag  DW_TAG_subprogram, DW_AT_name("startDma"), DW_AT_symbol_name("_startDma")
	.dwattr DW$34, DW_AT_low_pc(_startDma)
	.dwattr DW$34, DW_AT_high_pc(0x00)
	.dwattr DW$34, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$34, DW_AT_begin_line(0xab)
	.dwattr DW$34, DW_AT_begin_column(0x06)
	.dwattr DW$34, DW_AT_frame_base[DW_OP_breg17 8]
	.dwattr DW$34, DW_AT_skeletal(0x01)
	.dwpsn	"snd_dsp.c",171,16

;***************************************************************
;* FUNCTION DEF: _startDma                                     *
;***************************************************************
_startDma:
;** 174	-----------------------    dmaActive = 1;
;** 179	-----------------------    hDma = aic23_dma;
;** 179	-----------------------    dmsrc = (void (*)())((unsigned)outBuf+2u);
;** 179	-----------------------    dmctr = (outBufSize-2u>>1)-1u;
;* 1094	-----------------------    old_intm = IRQ_globalDisable();  // [15]
;* 1096	-----------------------    C$5 = 1u<<((*(struct $$fake4 *)hDma).ChaNum&0xfu)+8u;  // [15]
;* 1096	-----------------------    *(volatile unsigned *)0x54u = *(volatile unsigned *)0x54u&~C$5|C$5;  // [15]
;* 1097	-----------------------    *(volatile unsigned *)0x55u = (*(struct $$fake4 *)hDma).DmsrcAddr;  // [15]
;* 1098	-----------------------    *(volatile unsigned *)0x56u = (unsigned)dmsrc;  // [15]
;* 1099	-----------------------    *(volatile unsigned *)0x56u = (unsigned)(dmdst = (void (*)())0x22u);  // [15]
;* 1100	-----------------------    *(volatile unsigned *)0x56u = dmctr;  // [15]
;* 1101	-----------------------    *(volatile unsigned *)0x56u = 10240u;  // [15]
;* 1102	-----------------------    *(volatile unsigned *)0x56u = 16705u;  // [15]
;* 1120	-----------------------    IRQ_globalRestore(old_intm);  // [15]
;** 205	-----------------------    hMcbsp = aic23_port;
;* 1156	-----------------------    spsd = (*(struct $$fake3 *)hMcbsp).SpsdAddr;  // [11]
;* 1159	-----------------------    *(volatile unsigned *)(*(struct $$fake3 *)hMcbsp).SpsaAddr = 1u;  // [11]
;* 1159	-----------------------    if ( *(volatile unsigned *)spsd&2u ) goto g4;  // [11]
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
        PSHM      AR1
        PSHM      AR6
        SSBX      SXM
        PSHM      AR7
        LD        *(_outBuf),A
        ADD       #2,A,A                ; |179| 
        STLM      A,AR1
        LD        *(_outBufSize),A
        FRAME     #-4
        ADD       #-2,A
        ST        #1,*(_dmaActive)      ; |174| 
        LDM       AL,A                  ; |179| 
        MVDM      *(_aic23_dma),AR7
        SFTA      A,#-1,A               ; |179| 
        SUB       #1,A,A                ; |179| 
        CALLD     #_IRQ_globalDisable   ; |1094| 
        STLM      A,AR6
        NOP
        ; call occurs [#_IRQ_globalDisable] ; |1094| 
        LD        #15,B
        AND       *AR7(1),B             ; |1096| 
        ADD       #8,B
        LD        *(BL),ASM             ; |1096| 
        STLM      A,AR2
        LD        #1,B
        LD        B,ASM,A               ; |1096| 
        STM       #84,AR3
        CMPL      A,B                   ; |1096| 
        AND       *AR3,B                ; |1096| 
        OR        A,B                   ; |1096| 
        STL       B,*AR3
        STM       #85,AR3
        LD        *AR7(2),A
        STL       A,*AR3
        STM       #86,AR3
        MVKD      *(AR1),*AR3
        LD        #34,A
        STM       #86,AR1
        STL       A,*AR1
        MVKD      *(AR6),*AR1
        ST        #10240,*AR1           ; |1101| 
        ST        #16705,*AR1           ; |1102| 
        CALLD     #_IRQ_globalRestore   ; |1120| 
        NOP
        LDM       AR2,A
        ; call occurs [#_IRQ_globalRestore] ; |1120| 
        MVDM      *(_aic23_port),AR1
        NOP
        MVDK      *AR1(6),*(AR2)
        MVDK      *AR1(5),*(AR1)
        ST        #1,*AR1               ; |1159| 
        BIT       *AR2,14               ; |1159| 
        BC        L2,TC                 ; |1159| 
        ; branchcc occurs ; |1159| 
L1:    
DW$L$_startDma$2$B:
;**	-----------------------g3:
;** 205	-----------------------    hMcbsp = aic23_port;
;* 1156	-----------------------    spsd = (*(struct $$fake3 *)hMcbsp).SpsdAddr;  // [11]
;* 1159	-----------------------    *(volatile unsigned *)(*(struct $$fake3 *)hMcbsp).SpsaAddr = 1u;  // [11]
;* 1159	-----------------------    if ( !(*(volatile unsigned *)spsd&2u) ) goto g3;  // [11]
        MVDM      *(_aic23_port),AR1
        NOP
        MVDK      *AR1(6),*(AR2)
        MVDK      *AR1(5),*(AR1)
        ST        #1,*AR1               ; |1159| 
        BIT       *AR2,14               ; |1159| 
        BC        L1,NTC                ; |1159| 
        ; branchcc occurs ; |1159| 
DW$L$_startDma$2$E:
L2:    
;**	-----------------------g4:
;** 206	-----------------------    hMcbsp = aic23_port;
;** 206	-----------------------    Val = C$4 = (long)outBuf[1]|(long)*outBuf<<16;
;* 1149	-----------------------    C$3 = (*(struct $$fake3 *)hMcbsp).Port;  // [10]
;* 1149	-----------------------    *(volatile unsigned *)(((C$3 == 2u) ? 0xffd0u : 0u)+(C$3<<5)+34u) = (unsigned long)C$4>>16;  // [10]
        MVDM      *(_outBuf),AR1
        MVDM      *(_aic23_port),AR2
        LDU       *AR1,A
        LDU       *AR1(1),B
        SFTL      A,#15,A               ; |206| 
        SFTL      A,#1,A                ; |206| 
        OR        B,A                   ; |206| 
        DST       A,*SP(0)              ; |206| 
        MVDK      *AR2,*(AR1)
        BANZD     L3,*AR1(-2)           ; |1149| 
        SFTL      A,#-16,A              ; |1149| 
        DST       A,*SP(2)              ; |1149| 
        ; branchcc occurs ; |1149| 
        RSBX      SXM
        BD        L4                    ; |1149| 
        LD        #65488,A
        ; branch occurs ; |1149| 
L3:    
        LD        #0,A
L4:    
;* 1150	-----------------------    C$2 = (*(struct $$fake3 *)hMcbsp).Port;  // [10]
;* 1150	-----------------------    *(volatile unsigned *)(((C$2 == 2u) ? 0xffd0u : 0u)+(C$2<<5)+35u) = Val;  // [10]
        LDM       AR1,B
        ADD       B,#5,A                ; |1149| 
        SSBX      SXM
        STLM      A,AR1
        NOP
        DLD       *SP(2),A
        STL       A,*AR1(34)            ; |1149| 
        MVDK      *AR2,*(AR1)
        BANZ      L5,*AR1(-2)           ; |1150| 
        ; branchcc occurs ; |1150| 
        RSBX      SXM
        BD        L6                    ; |1150| 
        LD        #65488,A
        ; branch occurs ; |1150| 
L5:    
        LD        #0,A
L6:    
;* 1150	-----------------------    hDma = aic23_dma;  // [10]
;** 928	-----------------------    C$1 = 1u<<(*(struct $$fake4 *)hDma).ChaNum;  // [14]
;** 928	-----------------------    *(volatile unsigned *)0x54u = *(volatile unsigned *)0x54u&~C$1|C$1;  // [14]
;** 928	-----------------------    return;  // [14]
        LDM       AR1,B
        ADD       B,#5,A                ; |1150| 
        SSBX      SXM
        STLM      A,AR1
        NOP
        DLD       *SP(0),A
        STL       A,*AR1(35)            ; |1150| 
        MVDM      *(_aic23_dma),AR1
        NOP
        LD        *AR1(1),ASM           ; |928| 
        LD        #1,A
        LD        A,ASM,B               ; |928| 
        STM       #84,AR1
        CMPL      B,A                   ; |928| 
        FRAME     #4
        AND       *AR1,A                ; |928| 
        OR        B,A                   ; |928| 
        STL       A,*AR1
        POPM      AR7
        POPM      AR6
        POPM      AR1
	.dwpsn	"snd_dsp.c",210,1
        RET
        ; return occurs

DW$35	.dwtag  DW_TAG_loop
	.dwattr DW$35, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L1:1:1175352952")
	.dwattr DW$35, DW_AT_begin_file("C:/CCStudio_v3.3/C5400/csl/include/csl_mcbsp.h")
	.dwattr DW$35, DW_AT_begin_line(0xcd)
	.dwattr DW$35, DW_AT_end_line(0x487)
DW$36	.dwtag  DW_TAG_loop_range
	.dwattr DW$36, DW_AT_low_pc(DW$L$_startDma$2$B)
	.dwattr DW$36, DW_AT_high_pc(DW$L$_startDma$2$E)
	.dwendtag DW$35

	.dwattr DW$34, DW_AT_end_file("snd_dsp.c")
	.dwattr DW$34, DW_AT_end_line(0xd2)
	.dwattr DW$34, DW_AT_end_column(0x01)
	.dwendtag DW$34

	.sect	".text"
	.global	_debug

DW$37	.dwtag  DW_TAG_subprogram, DW_AT_name("debug"), DW_AT_symbol_name("_debug")
	.dwattr DW$37, DW_AT_low_pc(_debug)
	.dwattr DW$37, DW_AT_high_pc(0x00)
	.dwattr DW$37, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$37, DW_AT_begin_line(0xd4)
	.dwattr DW$37, DW_AT_begin_column(0x06)
	.dwattr DW$37, DW_AT_frame_base[DW_OP_breg17 2]
	.dwattr DW$37, DW_AT_skeletal(0x01)
	.dwpsn	"snd_dsp.c",212,28

;***************************************************************
;* FUNCTION DEF: _debug                                        *
;***************************************************************
_debug:
;* A     assigned to _msg
DW$38	.dwtag  DW_TAG_formal_parameter, DW_AT_name("msg"), DW_AT_symbol_name("_msg")
	.dwattr DW$38, DW_AT_type(*DW$T$53)
	.dwattr DW$38, DW_AT_location[DW_OP_reg0]
;** 213	-----------------------    if ( dspCom == NULL ) goto g8;
        PSHM      AR1
        LD        *(_dspCom),B          ; |213| 
        BCD       L10,BEQ               ; |213| 
        STLM      A,AR1
        NOP
        ; branchcc occurs ; |213| 
;** 117	-----------------------    d = (char *)(*&dspCom+14)-1;  // [0]
;** 118	-----------------------    s = msg;  // [0]
;** 120	-----------------------    *(++d) = T$1 = *s;  // [0]
;** 120	-----------------------    if ( !T$1 ) goto g5;  // [0]
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
        LD        *(_dspCom),B
        ADD       #13,B,B               ; |117| 
        STLM      B,AR2
        NOP
        LD        *AR1,A
        STL       A,*+AR2
        LD        *(AL),A               ; |120| 
        BC        L8,AEQ                ; |120| 
        ; branchcc occurs ; |120| 
L7:    
DW$L$_debug$3$B:
;**	-----------------------g4:
;** 120	-----------------------    *(++d) = T$2 = *(++s);  // [0]
;** 120	-----------------------    if ( T$2 ) goto g4;  // [0]
        MVDK      *+AR1(1),*(AR3)
        BANZD     L7,*AR3               ; |120| 
        MVKD      *(AR3),*+AR2
        ; branchcc occurs ; |120| 
DW$L$_debug$3$E:
L8:    
;**	-----------------------g5:
;** 216	-----------------------    (*dspCom).hasDbgMsg = 1u;
;** 218	-----------------------    armInt_trigger();
;** 220	-----------------------    if ( !(*dspCom).hasDbgMsg ) goto g8;
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
        MVDM      *(_dspCom),AR1
        NOP
        ST        #1,*AR1(13)           ; |216| 
        CALL      #_armInt_trigger      ; |218| 
        ; call occurs [#_armInt_trigger] ; |218| 
        MVDM      *(_dspCom),AR1
        NOP
        LD        *AR1(13),A            ; |220| 
        BC        L10,AEQ               ; |220| 
        ; branchcc occurs ; |220| 
L9:    
DW$L$_debug$5$B:
;**	-----------------------g7:
;** 220	-----------------------    if ( (*dspCom).hasDbgMsg ) goto g7;
;**	-----------------------g8:
;**  	-----------------------    return;
        LD        *AR1(13),A            ; |220| 
        BC        L9,ANEQ               ; |220| 
        ; branchcc occurs ; |220| 
DW$L$_debug$5$E:
L10:    
        POPM      AR1
	.dwpsn	"snd_dsp.c",221,1
        RET
        ; return occurs

DW$39	.dwtag  DW_TAG_loop
	.dwattr DW$39, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L9:1:1175352952")
	.dwattr DW$39, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$39, DW_AT_begin_line(0xdc)
	.dwattr DW$39, DW_AT_end_line(0xdc)
DW$40	.dwtag  DW_TAG_loop_range
	.dwattr DW$40, DW_AT_low_pc(DW$L$_debug$5$B)
	.dwattr DW$40, DW_AT_high_pc(DW$L$_debug$5$E)
	.dwendtag DW$39


DW$41	.dwtag  DW_TAG_loop
	.dwattr DW$41, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L7:1:1175352952")
	.dwattr DW$41, DW_AT_begin_file("C:/CCStudio_v3.3/C5400/cgtools/include/string.h")
	.dwattr DW$41, DW_AT_begin_line(0x78)
	.dwattr DW$41, DW_AT_end_line(0x78)
DW$42	.dwtag  DW_TAG_loop_range
	.dwattr DW$42, DW_AT_low_pc(DW$L$_debug$3$B)
	.dwattr DW$42, DW_AT_high_pc(DW$L$_debug$3$E)
	.dwendtag DW$41

	.dwattr DW$37, DW_AT_end_file("snd_dsp.c")
	.dwattr DW$37, DW_AT_end_line(0xdd)
	.dwattr DW$37, DW_AT_end_column(0x01)
	.dwendtag DW$37

	.sect	".text"
	.global	_dmaEnd

DW$43	.dwtag  DW_TAG_subprogram, DW_AT_name("dmaEnd"), DW_AT_symbol_name("_dmaEnd")
	.dwattr DW$43, DW_AT_low_pc(_dmaEnd)
	.dwattr DW$43, DW_AT_high_pc(0x00)
	.dwattr DW$43, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$43, DW_AT_begin_line(0x50)
	.dwattr DW$43, DW_AT_begin_column(0x10)
	.dwattr DW$43, DW_AT_frame_base[DW_OP_breg17 28]
	.dwattr DW$43, DW_AT_skeletal(0x01)
	.dwpsn	"snd_dsp.c",80,24

;***************************************************************
;* FUNCTION DEF: _dmaEnd                                       *
;***************************************************************
_dmaEnd:
        NOP
;
; BRC	assigned to temp var  L$1
;
;** 90	-----------------------    tmpBuf = outBuf;
;** 91	-----------------------    outBuf = outBuf2;
;** 92	-----------------------    outBuf2 = tmpBuf;
;** 94	-----------------------    tmpSize = outBufSize;
;** 95	-----------------------    outBufSize = y$6 = outBuf2Size;
;** 96	-----------------------    outBuf2Size = tmpSize;
;** 101	-----------------------    if ( !y$6 ) goto g3;
        PSHM      AL
        PSHM      AH
        PSHM      AG
        PSHM      BL
        PSHM      BH
        PSHM      BG
        PSHM      AR0
        PSHM      AR1
        PSHM      AR2
        PSHM      AR3
        PSHM      AR4
        PSHM      AR5
        PSHM      AR6
        PSHM      AR7
        PSHM      FP
        PSHM      BK
        PSHM      T
        PSHM      ST0
        PSHM      ST1
        PSHM      BRC
        PSHM      PMST
        PSHM      RSA
        PSHM      REA
        ANDM      #32063,*(ST1)
        ORM       #16384,*(ST1)
        ANDM      #-4,*(PMST)
        LDM       SP,A
        ANDM      #-2,*(SP)
        NOP
        NOP
        PSHM      AL
        LD        *(_outBuf),A
        MVDK      *(_outBuf2),*(_outBuf) ; |91| 
        STL       A,*(_outBuf2)
        LD        *(_outBufSize),B
        LDU       *(_outBuf2Size),A
        FRAME     #-5
        STL       B,*(_outBuf2Size)
        BCD       L11,AEQ               ; |101| 
        STL       A,*(_outBufSize)
        ; branchcc occurs ; |101| 
;** 103	-----------------------    startDma();
        CALL      #_startDma            ; |103| 
        ; call occurs [#_startDma] ; |103| 
L11:    
;**	-----------------------g3:
;** 108	-----------------------    (*dspCom).decodeDone = 0u;
;** 109	-----------------------    (*dspCom).decodeRequest = 1u;
;** 110	-----------------------    armInt_trigger();
;** 112	-----------------------    if ( !(*dspCom).decodeRequest ) goto g6;
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
        MVDM      *(_dspCom),AR1
        NOP
        ST        #0,*AR1(8)            ; |108| 
        MVDM      *(_dspCom),AR1
        NOP
        ST        #1,*AR1(7)            ; |109| 
        CALL      #_armInt_trigger      ; |110| 
        ; call occurs [#_armInt_trigger] ; |110| 
        MVDM      *(_dspCom),AR1
        NOP
        LD        *AR1(7),A             ; |112| 
        BC        L13,AEQ               ; |112| 
        ; branchcc occurs ; |112| 
L12:    
DW$L$_dmaEnd$4$B:
;**	-----------------------g5:
;** 112	-----------------------    if ( (*dspCom).decodeRequest ) goto g5;
        LD        *AR1(7),A             ; |112| 
        BC        L12,ANEQ              ; |112| 
        ; branchcc occurs ; |112| 
DW$L$_dmaEnd$4$E:
L13:    
;**	-----------------------g6:
;** 114	-----------------------    if ( !(*dspCom).decodeDone ) goto g22;
        LD        *AR1(8),A             ; |114| 
        BC        L21,AEQ               ; |114| 
        ; branchcc occurs ; |114| 
;** 118	-----------------------    outBuf2Size = (*dspCom).bufSize>>1;
;** 120	-----------------------    if ( (*dspCom).stereo ) goto g10;
        RSBX      SXM
        LD        *AR1(12),#-1,A        ; |118| 
        STL       A,*(_outBuf2Size)     ; |118| 
        LD        *AR1(6),A             ; |120| 
        BC        L14,ANEQ              ; |120| 
        ; branchcc occurs ; |120| 
;** 122	-----------------------    hasProcess = 1;
;** 125	-----------------------    if ( (*dspCom).stereo ) goto g11;
        MVDM      *(_dspCom),AR2
        NOP
        LD        *AR2(6),A             ; |125| 
        BCD       L15,ANEQ              ; |125| 
        STM       #1,AR1
        ; branchcc occurs ; |125| 
;** 126	-----------------------    outBuf2Size *= 2u;
;** 126	-----------------------    goto g11;
        LD        *(_outBuf2Size),A
        STL       A,#1,*(_outBuf2Size)  ; |126| 
        B         L15                   ; |126| 
        ; branch occurs ; |126| 
L14:    
;**	-----------------------g10:
;** 129	-----------------------    hasProcess = 0;
        STM       #0,AR1
L15:    
;**	-----------------------g11:
;** 134	-----------------------    dmaSize = outBuf2Size*2u+3u&0xfffcu;
;** 136	-----------------------    if ( hasProcess ) goto g15;
        LD        #3,A
        ADD       *(_outBuf2Size),#1,A,A ; |134| 
        BANZD     L17,*AR1              ; |136| 
        AND       #65532,A,A            ; |134| 
        ; branchcc occurs ; |136| 
;** 138	-----------------------    hpiDma_start((*dspCom).bufAddr, *&outBuf2, dmaSize, 0);
;** 139	-----------------------    if ( !hpiDma_pending() ) goto g24;
;**  	-----------------------    #pragma LOOP_FLAGS(5120u)
        MVKD      *(_outBuf2),*SP(0)    ; |138| 
        STL       A,*SP(1)
        MVDM      *(_dspCom),AR1
        ST        #0,*SP(2)             ; |138| 
        DLD       *AR1(10),A            ; |138| 
        CALL      #_hpiDma_start        ; |138| 
        ; call occurs [#_hpiDma_start] ; |138| 
        CALL      #_hpiDma_pending      ; |139| 
        ; call occurs [#_hpiDma_pending] ; |139| 
        LD        *(AL),A               ; |139| 
        BC        L23,AEQ               ; |139| 
        ; branchcc occurs ; |139| 
L16:    
DW$L$_dmaEnd$12$B:
;**	-----------------------g14:
;** 139	-----------------------    if ( hpiDma_pending() ) goto g14;
        CALL      #_hpiDma_pending      ; |139| 
        ; call occurs [#_hpiDma_pending] ; |139| 
        STLM      A,AR1
        NOP
        NOP
        BANZ      L16,*AR1              ; |139| 
        ; branchcc occurs ; |139| 
DW$L$_dmaEnd$12$E:
;** 139	-----------------------    goto g24;
        B         L23                   ; |139| 
        ; branch occurs ; |139| 
L17:    
;**	-----------------------g15:
;** 144	-----------------------    hpiDma_start((*dspCom).bufAddr, &processBuf, dmaSize, 0);
;** 145	-----------------------    if ( !hpiDma_pending() ) goto g18;
;**  	-----------------------    #pragma LOOP_FLAGS(5120u)
        ST        #_processBuf,*SP(0)   ; |144| 
        STL       A,*SP(1)
        MVDM      *(_dspCom),AR1
        ST        #0,*SP(2)             ; |144| 
        DLD       *AR1(10),A            ; |144| 
        CALL      #_hpiDma_start        ; |144| 
        ; call occurs [#_hpiDma_start] ; |144| 
        CALL      #_hpiDma_pending      ; |145| 
        ; call occurs [#_hpiDma_pending] ; |145| 
        LD        *(AL),A               ; |145| 
        BC        L19,AEQ               ; |145| 
        ; branchcc occurs ; |145| 
L18:    
DW$L$_dmaEnd$15$B:
;**	-----------------------g17:
;** 145	-----------------------    if ( hpiDma_pending() ) goto g17;
        CALL      #_hpiDma_pending      ; |145| 
        ; call occurs [#_hpiDma_pending] ; |145| 
        STLM      A,AR1
        NOP
        NOP
        BANZ      L18,*AR1              ; |145| 
        ; branchcc occurs ; |145| 
DW$L$_dmaEnd$15$E:
L19:    
;**	-----------------------g18:
;** 150	-----------------------    if ( (*dspCom).stereo ) goto g24;
        MVDM      *(_dspCom),AR1
        NOP
        LD        *AR1(6),A             ; |150| 
        BC        L23,ANEQ              ; |150| 
        ; branchcc occurs ; |150| 
;** 153	-----------------------    out = outBuf2;
;** 154	-----------------------    if ( !outBuf2Size ) goto g24;
        LD        *(_outBuf2Size),A     ; |154| 
        BCD       L23,AEQ               ; |154| 
        MVDM      *(_outBuf2),AR3
        ; branchcc occurs ; |154| 
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;** 144	-----------------------    K$25 = &processBuf[0];
;** 152	-----------------------    in = K$25;
	.dwpsn	"snd_dsp.c",154,0
        STM       #_processBuf,AR2
L20:    
DW$L$_dmaEnd$19$B:
;**	-----------------------g21:
;** 155	-----------------------    *out++ = *in;
;** 156	-----------------------    *out++ = *in++;
;** 154	-----------------------    if ( outBuf2Size+outBuf2 > out ) goto g21;
        MVDD      *AR2,*AR3+            ; |155| 
        MVDD      *AR2+,*AR3+           ; |156| 
        LD        *(_outBuf2),A
        ADD       *(_outBuf2Size),A     ; |154| 
        STLM      A,AR1
        NOP
        MVMM      AR3,AR0
        CMPR      2,AR1                 ; |154| 
	.dwpsn	"snd_dsp.c",157,0
        BC        L20,TC                ; |154| 
        ; branchcc occurs ; |154| 
DW$L$_dmaEnd$19$E:
;** 154	-----------------------    goto g24;
        B         L23                   ; |154| 
        ; branch occurs ; |154| 
L21:    
;**	-----------------------g22:
;** 163	-----------------------    outBuf2Size = 4096u;
;** 274	-----------------------    m = (char *)(*&outBuf2)-1;  // [1]
;**  	-----------------------    #pragma MUST_ITERATE(4096, 4096, 4096)
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g23:
;**  	-----------------------    L$1 = 4095;
;**	-----------------------g29:
        LD        *(_outBuf2),A
        SUB       #1,A,A                ; |274| 
        STLM      A,AR1
        ST        #4096,*(_outBuf2Size) ; |163| 
	.dwpsn	"snd_dsp.c",276,0
        RPT       #4095
        ; loop starts
L22:    
DW$L$_dmaEnd$22$B:
;** 276	-----------------------    *(++m) = 0;  // [1]
;** 276	-----------------------    if ( --L$1 != -1 ) goto g29;  // [1]
        ST        #0,*+AR1              ; |276| 
        ; loop ends ; |276| 
DW$L$_dmaEnd$22$E:
L23:    
;**	-----------------------g24:
;**  	-----------------------    return;
        FRAME     #5
        POPM      SP
        NOP
        POPM      REA
        POPM      RSA
        POPM      PMST
        POPM      BRC
        POPM      ST1
        POPM      ST0
        POPM      T
        POPM      BK
        POPM      FP
        POPM      AR7
        POPM      AR6
        POPM      AR5
        POPM      AR4
        POPM      AR3
        POPM      AR2
        POPM      AR1
        POPM      AR0
        POPM      BG
        POPM      BH
        POPM      BL
        POPM      AG
        POPM      AH
        POPM      AL
	.dwpsn	"snd_dsp.c",168,1
        RETE
        ; branch occurs

DW$44	.dwtag  DW_TAG_loop
	.dwattr DW$44, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L22:1:1175352952")
	.dwattr DW$44, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$44, DW_AT_begin_line(0x114)
	.dwattr DW$44, DW_AT_end_line(0x114)
DW$45	.dwtag  DW_TAG_loop_range
	.dwattr DW$45, DW_AT_low_pc(DW$L$_dmaEnd$22$B)
	.dwattr DW$45, DW_AT_high_pc(DW$L$_dmaEnd$22$E)
	.dwendtag DW$44


DW$46	.dwtag  DW_TAG_loop
	.dwattr DW$46, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L20:1:1175352952")
	.dwattr DW$46, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$46, DW_AT_begin_line(0x9a)
	.dwattr DW$46, DW_AT_end_line(0x9d)
DW$47	.dwtag  DW_TAG_loop_range
	.dwattr DW$47, DW_AT_low_pc(DW$L$_dmaEnd$19$B)
	.dwattr DW$47, DW_AT_high_pc(DW$L$_dmaEnd$19$E)
	.dwendtag DW$46


DW$48	.dwtag  DW_TAG_loop
	.dwattr DW$48, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L18:1:1175352952")
	.dwattr DW$48, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$48, DW_AT_begin_line(0x91)
	.dwattr DW$48, DW_AT_end_line(0x91)
DW$49	.dwtag  DW_TAG_loop_range
	.dwattr DW$49, DW_AT_low_pc(DW$L$_dmaEnd$15$B)
	.dwattr DW$49, DW_AT_high_pc(DW$L$_dmaEnd$15$E)
	.dwendtag DW$48


DW$50	.dwtag  DW_TAG_loop
	.dwattr DW$50, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L16:1:1175352952")
	.dwattr DW$50, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$50, DW_AT_begin_line(0x8b)
	.dwattr DW$50, DW_AT_end_line(0x8b)
DW$51	.dwtag  DW_TAG_loop_range
	.dwattr DW$51, DW_AT_low_pc(DW$L$_dmaEnd$12$B)
	.dwattr DW$51, DW_AT_high_pc(DW$L$_dmaEnd$12$E)
	.dwendtag DW$50


DW$52	.dwtag  DW_TAG_loop
	.dwattr DW$52, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L12:1:1175352952")
	.dwattr DW$52, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$52, DW_AT_begin_line(0x70)
	.dwattr DW$52, DW_AT_end_line(0x70)
DW$53	.dwtag  DW_TAG_loop_range
	.dwattr DW$53, DW_AT_low_pc(DW$L$_dmaEnd$4$B)
	.dwattr DW$53, DW_AT_high_pc(DW$L$_dmaEnd$4$E)
	.dwendtag DW$52

	.dwattr DW$43, DW_AT_end_file("snd_dsp.c")
	.dwattr DW$43, DW_AT_end_line(0xa8)
	.dwattr DW$43, DW_AT_end_column(0x01)
	.dwendtag DW$43

	.sect	".text"
	.global	_main

DW$54	.dwtag  DW_TAG_subprogram, DW_AT_name("main"), DW_AT_symbol_name("_main")
	.dwattr DW$54, DW_AT_low_pc(_main)
	.dwattr DW$54, DW_AT_high_pc(0x00)
	.dwattr DW$54, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$54, DW_AT_begin_line(0x27)
	.dwattr DW$54, DW_AT_begin_column(0x06)
	.dwattr DW$54, DW_AT_frame_base[DW_OP_breg17 6]
	.dwattr DW$54, DW_AT_skeletal(0x01)
	.dwpsn	"snd_dsp.c",39,12

;***************************************************************
;* FUNCTION DEF: _main                                         *
;***************************************************************
_main:
;** 42	-----------------------    *(volatile unsigned *)0x90u = 0u;
;** 45	-----------------------    dspCom = C$2 = &dspComBuffer;
;** 276	-----------------------    *(char *)(*&dspCom) = 0;  // [1]
;** 47	-----------------------    *(volatile unsigned *)0x90u = (unsigned)C$2;
;** 50	-----------------------    if ( (*dspCom).armInitFinished ) goto g4;
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
        PSHM      AR1
        LD        #_dspComBuffer,A
        STM       #144,AR1
        PSHM      AR6
        PSHM      AR7
        ST        #0,*AR1               ; |42| 
        STLM      A,AR1
        STL       A,*(_dspCom)
        ST        #0,*AR1               ; |276| 
        STM       #144,AR1
        STL       A,*AR1
        MVDM      *(_dspCom),AR1
        NOP
        LD        *AR1,A                ; |50| 
        BCD       L25,ANEQ              ; |50| 
        NOP
        FRAME     #-2
        ; branchcc occurs ; |50| 
L24:    
DW$L$_main$2$B:
;**	-----------------------g3:
;** 50	-----------------------    if ( !(*dspCom).armInitFinished ) goto g3;
        LD        *AR1,A                ; |50| 
        BC        L24,AEQ               ; |50| 
        ; branchcc occurs ; |50| 
DW$L$_main$2$E:
L25:    
;**	-----------------------g4:
;** 53	-----------------------    CSL_init();
;** 54	-----------------------    libDsp_init((int)(*dspCom).chipNum);
;** 57	-----------------------    aic23_port = aic23_openPort();
;** 58	-----------------------    if ( *&aic23_port != (void *)0xffffu ) goto g6;
        CALL      #_CSL_init            ; |53| 
        ; call occurs [#_CSL_init] ; |53| 
        MVDM      *(_dspCom),AR1
        NOP
        LD        *AR1(1),A
        CALL      #_libDsp_init         ; |54| 
        ; call occurs [#_libDsp_init] ; |54| 
        CALL      #_aic23_openPort      ; |57| 
        ; call occurs [#_aic23_openPort] ; |57| 
        STL       A,*(_aic23_port)
        CMPM      *(_aic23_port),#65535 ; |58| 
        BC        L26,NTC               ; |58| 
        ; branchcc occurs ; |58| 
;** 58	-----------------------    debug((char *)"Data Port NOK");
        CALLD     #_debug               ; |58| 
        LD        #SL1,A
        ; call occurs [#_debug] ; |58| 
L26:    
;**	-----------------------g6:
;** 61	-----------------------    aic23_dma = DMA_open(3, 1uL);
;** 62	-----------------------    hDma = aic23_dma;
;** 908	-----------------------    dmaEventID = (*(struct $$fake4 *)hDma).EventId;  // [13]
;** 908	-----------------------    *(volatile unsigned *)0x54u = *(volatile unsigned *)0x54u&0xff7fu|0x40u;  // [13]
;** 66	-----------------------    IRQ_globalDisable();
;** 267	-----------------------    bit = dmaEventID;  // [6]
;** 268	-----------------------    iptr = *(volatile unsigned *)0x1du>>7u;  // [6]
;** 269	-----------------------    VecLoc = (iptr<<5|bit)*4u;  // [6]
;** 272	-----------------------    old_intm = IRQ_globalDisable();  // [6]
;** 274	-----------------------    _IRQ_dat2Prg(&dmaEnd, VecLoc);  // [6]
;** 275	-----------------------    *(volatile unsigned *)0x1u = 1u<<bit;  // [6]
;** 277	-----------------------    IRQ_globalRestore((int)old_intm);  // [6]
;** 320	-----------------------    bit = bit-16u;  // [8]
;** 321	-----------------------    old_intm = IRQ_globalDisable();  // [8]
;** 322	-----------------------    *(volatile unsigned *)0x1u = C$1 = 1u<<bit;  // [8]
;** 323	-----------------------    IRQ_globalRestore((int)old_intm);  // [8]
;** 286	-----------------------    *NULL;  // [7]
;** 287	-----------------------    old_intm = IRQ_globalDisable();  // [7]
;** 288	-----------------------    *NULL = *NULL|C$1;  // [7]
;** 289	-----------------------    IRQ_globalRestore(old_intm);  // [7]
;** 290	-----------------------    IRQ_globalEnable();  // [7]
;** 72	-----------------------    debug((char *)"DSP START");
        LD        #1,A
        DST       A,*SP(0)              ; |61| 
        CALLD     #_DMA_open            ; |61| 
        NOP
        LD        #3,A
        ; call occurs [#_DMA_open] ; |61| 
        RSBX      SXM
        STLM      A,AR1
        STL       A,*(_aic23_dma)
        STM       #84,AR2
        LD        #65407,A
        AND       *AR2,A                ; |908| 
        MVDK      *AR1,*(AR1)
        OR        #64,A,A               ; |908| 
        STL       A,*AR2
        CALL      #_IRQ_globalDisable   ; |66| 
        ; call occurs [#_IRQ_globalDisable] ; |66| 
        STM       #29,AR2
        LDU       *AR2,A
        SFTA      A,#-7,A               ; |268| 
        SFTL      A,#5,B                ; |269| 
        LDM       AR1,A
        OR        A,B                   ; |269| 
        SFTL      B,#2,A                ; |269| 
        CALLD     #_IRQ_globalDisable   ; |272| 
        STLM      A,AR6
        NOP
        ; call occurs [#_IRQ_globalDisable] ; |272| 
        MVKD      *(AR6),*SP(0)
        STLM      A,AR7
        CALLD     #__IRQ_dat2Prg        ; |274| 
        LD        #_dmaEnd,A
        ; call occurs [#__IRQ_dat2Prg] ; |274| 
        LD        *(AR1),ASM            ; |275| 
        LD        #1,A
        STM       #1,AR2
        LD        A,ASM,A               ; |275| 
        STL       A,*AR2
        CALLD     #_IRQ_globalRestore   ; |277| 
        NOP
        LDM       AR7,A
        ; call occurs [#_IRQ_globalRestore] ; |277| 
        LDM       AR1,A
        SUB       #16,A,A               ; |320| 
        CALLD     #_IRQ_globalDisable   ; |321| 
        STLM      A,AR1
        NOP
        ; call occurs [#_IRQ_globalDisable] ; |321| 
        LD        *(AR1),ASM            ; |322| 
        LD        #1,B
        LD        B,ASM,B               ; |322| 
        STM       #1,AR2
        STLM      B,AR1
        MVKD      *(AR1),*AR2
        CALL      #_IRQ_globalRestore   ; |323| 
        ; call occurs [#_IRQ_globalRestore] ; |323| 
        STM       #0,AR2
        LD        *AR2,A
        CALL      #_IRQ_globalDisable   ; |287| 
        ; call occurs [#_IRQ_globalDisable] ; |287| 
        STM       #0,AR2
        LDM       AR1,B
        OR        *AR2,B                ; |288| 
        STM       #0,AR1
        STL       B,*AR1
        CALL      #_IRQ_globalRestore   ; |289| 
        ; call occurs [#_IRQ_globalRestore] ; |289| 
        CALL      #_IRQ_globalEnable    ; |290| 
        ; call occurs [#_IRQ_globalEnable] ; |290| 
        CALLD     #_debug               ; |72| 
        LD        #SL2,A
        ; call occurs [#_debug] ; |72| 
L27:    
DW$L$_main$6$B:
;**  	-----------------------    #pragma LOOP_FLAGS(5120u)
;**	-----------------------g7:
;**  	-----------------------    #pragma LOOP_FLAGS(4096u)
;**	-----------------------g8:
;** 76	-----------------------    if ( dmaActive ) goto g8;
        LD        *(_dmaActive),A       ; |76| 
        BC        L27,ANEQ              ; |76| 
        ; branchcc occurs ; |76| 
DW$L$_main$6$E:
DW$L$_main$7$B:
;** 76	-----------------------    debug((char *)"simdma");
;** 76	-----------------------    dmaEnd();
;** 76	-----------------------    goto g7;
        CALLD     #_debug               ; |76| 
        LD        #SL3,A
        ; call occurs [#_debug] ; |76| 
        CALL      #_dmaEnd              ; |76| 
        ; call occurs [#_dmaEnd] ; |76| 
	.dwpsn	"snd_dsp.c",78,1
        B         L27                   ; |76| 
        ; branch occurs ; |76| 
DW$L$_main$7$E:

DW$55	.dwtag  DW_TAG_loop
	.dwattr DW$55, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L27:1:1175352952")
	.dwattr DW$55, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$55, DW_AT_begin_line(0x4c)
	.dwattr DW$55, DW_AT_end_line(0x4e)
DW$56	.dwtag  DW_TAG_loop_range
	.dwattr DW$56, DW_AT_low_pc(DW$L$_main$6$B)
	.dwattr DW$56, DW_AT_high_pc(DW$L$_main$6$E)
DW$57	.dwtag  DW_TAG_loop_range
	.dwattr DW$57, DW_AT_low_pc(DW$L$_main$7$B)
	.dwattr DW$57, DW_AT_high_pc(DW$L$_main$7$E)
	.dwendtag DW$55


DW$58	.dwtag  DW_TAG_loop
	.dwattr DW$58, DW_AT_name("Z:\Av320\ArchOpen\trunk\medios\snd\dspcode\snd_dsp.asm:L24:1:1175352952")
	.dwattr DW$58, DW_AT_begin_file("snd_dsp.c")
	.dwattr DW$58, DW_AT_begin_line(0x32)
	.dwattr DW$58, DW_AT_end_line(0x32)
DW$59	.dwtag  DW_TAG_loop_range
	.dwattr DW$59, DW_AT_low_pc(DW$L$_main$2$B)
	.dwattr DW$59, DW_AT_high_pc(DW$L$_main$2$E)
	.dwendtag DW$58

	.dwattr DW$54, DW_AT_end_file("snd_dsp.c")
	.dwattr DW$54, DW_AT_end_line(0x4e)
	.dwattr DW$54, DW_AT_end_column(0x01)
	.dwendtag DW$54

;; Inlined function references:
;; [  0] strcpy
;; [  1] memset
;; [  6] IRQ_plug
;; [  7] IRQ_enable
;; [  8] IRQ_clear
;; [ 10] MCBSP_write32
;; [ 11] MCBSP_xrdy
;; [ 13] DMA_getEventId
;; [ 14] DMA_start
;; [ 15] DMA_configArgs
;***************************************************************
;* STRINGS                                                     *
;***************************************************************
	.sect	".const"
SL1:	.string	"Data Port NOK",0
SL2:	.string	"DSP START",0
SL3:	.string	"simdma",0
;***************************************************************
;* UNDEFINED EXTERNAL REFERENCES                               *
;***************************************************************
	.global	__IRQ_dat2Prg
	.global	_IRQ_globalDisable
	.global	_IRQ_globalEnable
	.global	_IRQ_globalRestore
	.global	_CSL_init
	.global	_DMA_open
	.global	_armInt_trigger
	.global	_hpiDma_start
	.global	_hpiDma_pending
	.global	_aic23_openPort
	.global	_libDsp_init

;***************************************************************
;* TYPE INFORMATION                                            *
;***************************************************************
DW$T$3	.dwtag  DW_TAG_pointer_type
	.dwattr DW$T$3, DW_AT_address_class(0x10)

DW$T$28	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*DW$T$3)
	.dwattr DW$T$28, DW_AT_language(DW_LANG_C)
DW$60	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$3)
DW$61	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$10)
DW$62	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$27)
	.dwendtag DW$T$28


DW$T$30	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$30, DW_AT_language(DW_LANG_C)
DW$T$32	.dwtag  DW_TAG_typedef, DW_AT_name("IRQ_IsrPtr"), DW_AT_type(*DW$T$31)
	.dwattr DW$T$32, DW_AT_language(DW_LANG_C)

DW$T$33	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$33, DW_AT_language(DW_LANG_C)
DW$63	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$32)
DW$64	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$24)
	.dwendtag DW$T$33


DW$T$35	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$35, DW_AT_language(DW_LANG_C)
DW$65	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$10)
	.dwendtag DW$T$35


DW$T$37	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$37, DW_AT_language(DW_LANG_C)
DW$66	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$24)
DW$67	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$32)
	.dwendtag DW$T$37


DW$T$38	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$38, DW_AT_language(DW_LANG_C)
DW$68	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$24)
	.dwendtag DW$T$38


DW$T$41	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$41, DW_AT_language(DW_LANG_C)
DW$69	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$40)
DW$70	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$19)
	.dwendtag DW$T$41


DW$T$44	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$44, DW_AT_language(DW_LANG_C)
DW$71	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$43)
	.dwendtag DW$T$44


DW$T$45	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$45, DW_AT_language(DW_LANG_C)

DW$T$48	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$48, DW_AT_language(DW_LANG_C)
DW$72	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$43)
DW$73	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$24)
DW$74	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$24)
DW$75	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$24)
DW$76	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$47)
DW$77	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$47)
DW$78	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$24)
	.dwendtag DW$T$48


DW$T$49	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$49, DW_AT_language(DW_LANG_C)
DW$79	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$13)
DW$80	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$3)
DW$81	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$9)
DW$82	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$8)
	.dwendtag DW$T$49


DW$T$54	.dwtag  DW_TAG_subroutine_type
	.dwattr DW$T$54, DW_AT_language(DW_LANG_C)
DW$83	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$53)
	.dwendtag DW$T$54

DW$T$8	.dwtag  DW_TAG_base_type, DW_AT_name("short")
	.dwattr DW$T$8, DW_AT_encoding(DW_ATE_signed)
	.dwattr DW$T$8, DW_AT_byte_size(0x01)

DW$T$56	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*DW$T$8)
	.dwattr DW$T$56, DW_AT_language(DW_LANG_C)
DW$T$9	.dwtag  DW_TAG_base_type, DW_AT_name("unsigned short")
	.dwattr DW$T$9, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr DW$T$9, DW_AT_byte_size(0x01)
DW$T$24	.dwtag  DW_TAG_typedef, DW_AT_name("Uint16"), DW_AT_type(*DW$T$9)
	.dwattr DW$T$24, DW_AT_language(DW_LANG_C)

DW$T$57	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*DW$T$24)
	.dwattr DW$T$57, DW_AT_language(DW_LANG_C)
DW$84	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$43)
	.dwendtag DW$T$57


DW$T$59	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*DW$T$58)
	.dwattr DW$T$59, DW_AT_language(DW_LANG_C)
DW$85	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$40)
	.dwendtag DW$T$59


DW$T$60	.dwtag  DW_TAG_array_type, DW_AT_type(*DW$T$9)
	.dwattr DW$T$60, DW_AT_language(DW_LANG_C)
	.dwattr DW$T$60, DW_AT_byte_size(0x800)
DW$86	.dwtag  DW_TAG_subrange_type
	.dwattr DW$86, DW_AT_upper_bound(0x7ff)
	.dwendtag DW$T$60


DW$T$61	.dwtag  DW_TAG_array_type, DW_AT_type(*DW$T$9)
	.dwattr DW$T$61, DW_AT_language(DW_LANG_C)
	.dwattr DW$T$61, DW_AT_byte_size(0x1000)
DW$87	.dwtag  DW_TAG_subrange_type
	.dwattr DW$87, DW_AT_upper_bound(0xfff)
	.dwendtag DW$T$61

DW$T$62	.dwtag  DW_TAG_pointer_type, DW_AT_type(*DW$T$9)
	.dwattr DW$T$62, DW_AT_address_class(0x10)
DW$T$10	.dwtag  DW_TAG_base_type, DW_AT_name("int")
	.dwattr DW$T$10, DW_AT_encoding(DW_ATE_signed)
	.dwattr DW$T$10, DW_AT_byte_size(0x01)

DW$T$63	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*DW$T$10)
	.dwattr DW$T$63, DW_AT_language(DW_LANG_C)

DW$T$65	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*DW$T$10)
	.dwattr DW$T$65, DW_AT_language(DW_LANG_C)
DW$88	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$24)
	.dwendtag DW$T$65

DW$T$27	.dwtag  DW_TAG_typedef, DW_AT_name("size_t"), DW_AT_type(*DW$T$11)
	.dwattr DW$T$27, DW_AT_language(DW_LANG_C)
DW$T$13	.dwtag  DW_TAG_base_type, DW_AT_name("unsigned long")
	.dwattr DW$T$13, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr DW$T$13, DW_AT_byte_size(0x02)
DW$T$19	.dwtag  DW_TAG_typedef, DW_AT_name("Uint32"), DW_AT_type(*DW$T$13)
	.dwattr DW$T$19, DW_AT_language(DW_LANG_C)
DW$T$40	.dwtag  DW_TAG_typedef, DW_AT_name("MCBSP_Handle"), DW_AT_type(*DW$T$39)
	.dwattr DW$T$40, DW_AT_language(DW_LANG_C)

DW$T$94	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*DW$T$40)
	.dwattr DW$T$94, DW_AT_language(DW_LANG_C)
DW$T$43	.dwtag  DW_TAG_typedef, DW_AT_name("DMA_Handle"), DW_AT_type(*DW$T$42)
	.dwattr DW$T$43, DW_AT_language(DW_LANG_C)

DW$T$98	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*DW$T$43)
	.dwattr DW$T$98, DW_AT_language(DW_LANG_C)
DW$89	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$10)
DW$90	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$19)
	.dwendtag DW$T$98

DW$T$102	.dwtag  DW_TAG_typedef, DW_AT_name("tDspCom"), DW_AT_type(*DW$T$101)
	.dwattr DW$T$102, DW_AT_language(DW_LANG_C)
DW$T$103	.dwtag  DW_TAG_pointer_type, DW_AT_type(*DW$T$102)
	.dwattr DW$T$103, DW_AT_address_class(0x10)
DW$T$53	.dwtag  DW_TAG_pointer_type, DW_AT_type(*DW$T$52)
	.dwattr DW$T$53, DW_AT_address_class(0x10)

DW$T$109	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*DW$T$67)
	.dwattr DW$T$109, DW_AT_language(DW_LANG_C)
DW$91	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$67)
DW$92	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*DW$T$53)
	.dwendtag DW$T$109

DW$T$31	.dwtag  DW_TAG_pointer_type, DW_AT_type(*DW$T$30)
	.dwattr DW$T$31, DW_AT_address_class(0x10)
DW$T$47	.dwtag  DW_TAG_typedef, DW_AT_name("DMA_AdrPtr"), DW_AT_type(*DW$T$46)
	.dwattr DW$T$47, DW_AT_language(DW_LANG_C)
DW$T$58	.dwtag  DW_TAG_typedef, DW_AT_name("CSLBool"), DW_AT_type(*DW$T$9)
	.dwattr DW$T$58, DW_AT_language(DW_LANG_C)
DW$T$11	.dwtag  DW_TAG_base_type, DW_AT_name("unsigned int")
	.dwattr DW$T$11, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr DW$T$11, DW_AT_byte_size(0x01)
DW$T$39	.dwtag  DW_TAG_pointer_type, DW_AT_type(*DW$T$20)
	.dwattr DW$T$39, DW_AT_address_class(0x10)
DW$T$42	.dwtag  DW_TAG_pointer_type, DW_AT_type(*DW$T$21)
	.dwattr DW$T$42, DW_AT_address_class(0x10)
DW$T$101	.dwtag  DW_TAG_volatile_type
	.dwattr DW$T$101, DW_AT_type(*DW$T$23)
DW$T$52	.dwtag  DW_TAG_const_type
	.dwattr DW$T$52, DW_AT_type(*DW$T$51)
DW$T$67	.dwtag  DW_TAG_pointer_type, DW_AT_type(*DW$T$51)
	.dwattr DW$T$67, DW_AT_address_class(0x10)
DW$T$46	.dwtag  DW_TAG_pointer_type, DW_AT_type(*DW$T$45)
	.dwattr DW$T$46, DW_AT_address_class(0x10)

DW$T$20	.dwtag  DW_TAG_structure_type
	.dwattr DW$T$20, DW_AT_byte_size(0x02)
DW$93	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$19)
	.dwattr DW$93, DW_AT_name("Private"), DW_AT_symbol_name("_Private")
	.dwattr DW$93, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr DW$93, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag DW$T$20


DW$T$21	.dwtag  DW_TAG_structure_type
	.dwattr DW$T$21, DW_AT_byte_size(0x02)
DW$94	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$19)
	.dwattr DW$94, DW_AT_name("Private"), DW_AT_symbol_name("_Private")
	.dwattr DW$94, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr DW$94, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag DW$T$21


DW$T$23	.dwtag  DW_TAG_structure_type
	.dwattr DW$T$23, DW_AT_byte_size(0x10e)
DW$95	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$9)
	.dwattr DW$95, DW_AT_name("armInitFinished"), DW_AT_symbol_name("_armInitFinished")
	.dwattr DW$95, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr DW$95, DW_AT_accessibility(DW_ACCESS_public)
DW$96	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$9)
	.dwattr DW$96, DW_AT_name("chipNum"), DW_AT_symbol_name("_chipNum")
	.dwattr DW$96, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr DW$96, DW_AT_accessibility(DW_ACCESS_public)
DW$97	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$13)
	.dwattr DW$97, DW_AT_name("dataSampleRate"), DW_AT_symbol_name("_dataSampleRate")
	.dwattr DW$97, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr DW$97, DW_AT_accessibility(DW_ACCESS_public)
DW$98	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$13)
	.dwattr DW$98, DW_AT_name("outputSampleRate"), DW_AT_symbol_name("_outputSampleRate")
	.dwattr DW$98, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr DW$98, DW_AT_accessibility(DW_ACCESS_public)
DW$99	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$9)
	.dwattr DW$99, DW_AT_name("stereo"), DW_AT_symbol_name("_stereo")
	.dwattr DW$99, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr DW$99, DW_AT_accessibility(DW_ACCESS_public)
DW$100	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$9)
	.dwattr DW$100, DW_AT_name("decodeRequest"), DW_AT_symbol_name("_decodeRequest")
	.dwattr DW$100, DW_AT_data_member_location[DW_OP_plus_uconst 0x7]
	.dwattr DW$100, DW_AT_accessibility(DW_ACCESS_public)
DW$101	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$9)
	.dwattr DW$101, DW_AT_name("decodeDone"), DW_AT_symbol_name("_decodeDone")
	.dwattr DW$101, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr DW$101, DW_AT_accessibility(DW_ACCESS_public)
DW$102	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$13)
	.dwattr DW$102, DW_AT_name("bufAddr"), DW_AT_symbol_name("_bufAddr")
	.dwattr DW$102, DW_AT_data_member_location[DW_OP_plus_uconst 0xa]
	.dwattr DW$102, DW_AT_accessibility(DW_ACCESS_public)
DW$103	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$9)
	.dwattr DW$103, DW_AT_name("bufSize"), DW_AT_symbol_name("_bufSize")
	.dwattr DW$103, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr DW$103, DW_AT_accessibility(DW_ACCESS_public)
DW$104	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$9)
	.dwattr DW$104, DW_AT_name("hasDbgMsg"), DW_AT_symbol_name("_hasDbgMsg")
	.dwattr DW$104, DW_AT_data_member_location[DW_OP_plus_uconst 0xd]
	.dwattr DW$104, DW_AT_accessibility(DW_ACCESS_public)
DW$105	.dwtag  DW_TAG_member, DW_AT_type(*DW$T$22)
	.dwattr DW$105, DW_AT_name("dbgMsg"), DW_AT_symbol_name("_dbgMsg")
	.dwattr DW$105, DW_AT_data_member_location[DW_OP_plus_uconst 0xe]
	.dwattr DW$105, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag DW$T$23

DW$T$51	.dwtag  DW_TAG_base_type, DW_AT_name("signed char")
	.dwattr DW$T$51, DW_AT_encoding(DW_ATE_signed_char)
	.dwattr DW$T$51, DW_AT_byte_size(0x01)

DW$T$22	.dwtag  DW_TAG_array_type, DW_AT_type(*DW$T$8)
	.dwattr DW$T$22, DW_AT_language(DW_LANG_C)
	.dwattr DW$T$22, DW_AT_byte_size(0xff)
DW$106	.dwtag  DW_TAG_subrange_type
	.dwattr DW$106, DW_AT_upper_bound(0xfe)
	.dwendtag DW$T$22


	.dwattr DW$37, DW_AT_external(0x01)
	.dwattr DW$43, DW_AT_external(0x01)
	.dwattr DW$54, DW_AT_external(0x01)
	.dwattr DW$34, DW_AT_external(0x01)
	.dwattr DW$CU, DW_AT_language(DW_LANG_C)

;***************************************************************
;* DWARF REGISTER MAP                                          *
;***************************************************************

DW$107	.dwtag  DW_TAG_assign_register, DW_AT_name("A")
	.dwattr DW$107, DW_AT_location[DW_OP_reg0]
DW$108	.dwtag  DW_TAG_assign_register, DW_AT_name("AH")
	.dwattr DW$108, DW_AT_location[DW_OP_reg1]
DW$109	.dwtag  DW_TAG_assign_register, DW_AT_name("AG")
	.dwattr DW$109, DW_AT_location[DW_OP_reg2]
DW$110	.dwtag  DW_TAG_assign_register, DW_AT_name("")
	.dwattr DW$110, DW_AT_location[DW_OP_reg3]
DW$111	.dwtag  DW_TAG_assign_register, DW_AT_name("")
	.dwattr DW$111, DW_AT_location[DW_OP_reg4]
DW$112	.dwtag  DW_TAG_assign_register, DW_AT_name("")
	.dwattr DW$112, DW_AT_location[DW_OP_reg5]
DW$113	.dwtag  DW_TAG_assign_register, DW_AT_name("B")
	.dwattr DW$113, DW_AT_location[DW_OP_reg6]
DW$114	.dwtag  DW_TAG_assign_register, DW_AT_name("BH")
	.dwattr DW$114, DW_AT_location[DW_OP_reg7]
DW$115	.dwtag  DW_TAG_assign_register, DW_AT_name("BG")
	.dwattr DW$115, DW_AT_location[DW_OP_reg8]
DW$116	.dwtag  DW_TAG_assign_register, DW_AT_name("AR0")
	.dwattr DW$116, DW_AT_location[DW_OP_reg9]
DW$117	.dwtag  DW_TAG_assign_register, DW_AT_name("AR1")
	.dwattr DW$117, DW_AT_location[DW_OP_reg10]
DW$118	.dwtag  DW_TAG_assign_register, DW_AT_name("AR2")
	.dwattr DW$118, DW_AT_location[DW_OP_reg11]
DW$119	.dwtag  DW_TAG_assign_register, DW_AT_name("AR3")
	.dwattr DW$119, DW_AT_location[DW_OP_reg12]
DW$120	.dwtag  DW_TAG_assign_register, DW_AT_name("AR4")
	.dwattr DW$120, DW_AT_location[DW_OP_reg13]
DW$121	.dwtag  DW_TAG_assign_register, DW_AT_name("AR5")
	.dwattr DW$121, DW_AT_location[DW_OP_reg14]
DW$122	.dwtag  DW_TAG_assign_register, DW_AT_name("AR6")
	.dwattr DW$122, DW_AT_location[DW_OP_reg15]
DW$123	.dwtag  DW_TAG_assign_register, DW_AT_name("AR7")
	.dwattr DW$123, DW_AT_location[DW_OP_reg16]
DW$124	.dwtag  DW_TAG_assign_register, DW_AT_name("SP")
	.dwattr DW$124, DW_AT_location[DW_OP_reg17]
DW$125	.dwtag  DW_TAG_assign_register, DW_AT_name("FP")
	.dwattr DW$125, DW_AT_location[DW_OP_reg18]
DW$126	.dwtag  DW_TAG_assign_register, DW_AT_name("BK")
	.dwattr DW$126, DW_AT_location[DW_OP_reg19]
DW$127	.dwtag  DW_TAG_assign_register, DW_AT_name("T")
	.dwattr DW$127, DW_AT_location[DW_OP_reg20]
DW$128	.dwtag  DW_TAG_assign_register, DW_AT_name("ASM")
	.dwattr DW$128, DW_AT_location[DW_OP_reg21]
DW$129	.dwtag  DW_TAG_assign_register, DW_AT_name("SXM")
	.dwattr DW$129, DW_AT_location[DW_OP_reg22]
DW$130	.dwtag  DW_TAG_assign_register, DW_AT_name("ST0")
	.dwattr DW$130, DW_AT_location[DW_OP_reg23]
DW$131	.dwtag  DW_TAG_assign_register, DW_AT_name("ST1")
	.dwattr DW$131, DW_AT_location[DW_OP_reg24]
DW$132	.dwtag  DW_TAG_assign_register, DW_AT_name("BRC")
	.dwattr DW$132, DW_AT_location[DW_OP_reg25]
DW$133	.dwtag  DW_TAG_assign_register, DW_AT_name("TC")
	.dwattr DW$133, DW_AT_location[DW_OP_reg26]
DW$134	.dwtag  DW_TAG_assign_register, DW_AT_name("PC")
	.dwattr DW$134, DW_AT_location[DW_OP_reg27]
DW$135	.dwtag  DW_TAG_assign_register, DW_AT_name("DB")
	.dwattr DW$135, DW_AT_location[DW_OP_reg28]
DW$136	.dwtag  DW_TAG_assign_register, DW_AT_name("PMST")
	.dwattr DW$136, DW_AT_location[DW_OP_reg29]
DW$137	.dwtag  DW_TAG_assign_register, DW_AT_name("RSA")
	.dwattr DW$137, DW_AT_location[DW_OP_reg30]
DW$138	.dwtag  DW_TAG_assign_register, DW_AT_name("REA")
	.dwattr DW$138, DW_AT_location[DW_OP_reg31]
DW$139	.dwtag  DW_TAG_assign_register, DW_AT_name("OVM")
	.dwattr DW$139, DW_AT_location[DW_OP_regx 0x20]
DW$140	.dwtag  DW_TAG_assign_register, DW_AT_name("FRCT")
	.dwattr DW$140, DW_AT_location[DW_OP_regx 0x21]
DW$141	.dwtag  DW_TAG_assign_register, DW_AT_name("CIE_RETA")
	.dwattr DW$141, DW_AT_location[DW_OP_regx 0x22]
	.dwendtag DW$CU

