#ifndef STRUCT_H
#define STRUCT_H

/*
 * ������֥�Υ����ɤ�C�Υ����ɤǻȤ���¤��
 *
 *
 */
#ifndef __ASSEMBLY__
struct emulator_opt {
  int n_prg_rom;
  int n_chr_rom;
  int mapper_num;
  int scroll_type;
  int has_sram;
  void *prg_rom_start;
  void *chr_rom_start;
  void *save_file_write;
  void *save_file_read;
};

extern struct emulator_opt emulator_opt;

#else

#define EMU_OPT_n_prg_rom	0x00
#define EMU_OPT_n_chr_rom	0x04
#define EMU_OPT_mapper_num	0x08
#define EMU_OPT_scroll_type	0x0C
#define EMU_OPT_has_sram	0x10
#define EMU_OPT_prg_rom_start	0x14
#define EMU_OPT_chr_rom_start	0x18
#define EMU_OPT_save_file_write	0x1C
#define EMU_OPT_save_file_read	0x20

#define EMU_OPT_size		0x24

.macro	ldopt	reg, opt
	ldr	\reg, = emulator_opt
	ldr	\reg, [\reg, #(\opt)]
.endm

#endif

#ifndef __ASSEMBLY__

struct L_menu {
  int reset;
  void *save_file_write;
  int exit;
};

#else

#define L_MENU_reset		0x00
#define L_MENU_save_file_write	0x04
#define L_MENU_exit		0x08

#define L_MENU_size		0x0C

#endif


#endif
