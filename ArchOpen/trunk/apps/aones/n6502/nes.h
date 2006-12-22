#ifndef NES_H
#define NES_H

#ifndef DM320
    #define SECTION_FAST .section ".core"//.section emu.fast, "awx"
#else
    #define SECTION_FAST .section ".text"//.section emu.fast, "awx"
#endif

#define SECTION_SLOW .section ".text"//.section emu.slow, "awx"
#define SECTION_HEAD .section ".text"//.section emu.head, "awx"

#define ALIGN .align 4

/*
 * 6502 �Υ쥸����
 *
 * A �Ͼ�� 24 �ӥåȥ��եȤ��Ƥ���
 * P ��2�Ĥ�ʬ���롣
 * NZ�ϤۤȤ�ɤ�̿�᤬�ѹ�����Τǡ�NZ�θ��ˤʤ��ͤ���¸
 * �Ĥ��¾�ξ����֤�
 *
 * S ��24�ӥåȥ��եȤ��Ƥ���
 * �����;�äƲ��ΥӥåȤ� P �λĤ���֤�(VBDI)
 * �����;�äƤ�Ȥ���ˤϳ����ߤ˴ؤ���ե饰���֤�
 *
 * PC ��ROM��Υ��ɥ쥹���Ѵ����Ƥ���
 * ���ξ��϶��������å��򤷤��ۤ��������Τ�������ñ�ǤϤʤ�
 * ������ PC �����뤿��ˤϸ��ߤΥХ󥯤���Ƭ���ɥ쥹������Ф褤
 */
#define REG_A  r4
#define REG_X  r5
#define REG_Y  r6
#define REG_PC r7
#define REG_S  r8
#define REG_NZ  r9
#define REG_P_REST REG_S

#if 0
/*
 * REG_P �ǻȤ��ե饰
 */
#define ARM_N_FLAG 0x80000000
#define ARM_Z_FLAG 0x40000000
#define ARM_C_FLAG 0x20000000
#define ARM_V_FLAG 0x10000000
#endif

/*
 * REG_P_REST �ˤϳƼ�ե饰���֤�
 *
 * 0
 * FEDCBA98 76543210
 * |||||||| |||||||+---C ����꡼�ޤ��ϥܥ��ʤ�
 * |||||||| ||||||+---NMI��ȯ��
 * |||||||| |||||+---I �����ߵ���
 * |||||||| ||||+---D ���ʥ⡼��
 * |||||||| |||+---B
 * |||||||| ||+---IRQ��ȯ��
 * |||||||| |+---V �����С��ե�
 * |||||||| +---$2005/$2006 �ȥ���ե饰
 * ||||||||
 * |||||||+---$2000[2] 32���󥯥����
 * ||||||+---$2000[3] ���ץ饤�ȥ��ɥ쥹����
 * |||||+---$2000[4] �Хå����饦��ɥ��ɥ쥹����
 * ||||+---$2000[5] ���ץ饤�ȥ�����
 * |||+---̤����
 * ||+---$2000[7] NMI����
 * |+---$2001[3] �Хå����饦��ɤ�����
 * +---$2001[4] ���ץ饤�Ȥ�����
 *
 * 1
 * FEDCBA9876543210
 * |||||||||||||||+---$2002[4] VRAM�˽񤭹��߲ġ�
 * ||||||||||||||+---$2002[5] ���ץ饤�Ȥ�¿����
 * |||||||||||||+---$2002[6] ���ץ饤�ȥҥå�(�����Ǥ���Τ���)
 * ||||||||||||+---$2002[7] VBL
 * |||||||||||+---DMC������ȯ��
 * ||||||||||+---VRAM����äƤ���
 * ||||||||++---̤����
 * ++++++++---S �����å��ݥ���
 *
 */


/*
 * REG_P_REST �ǻȤ��ե饰������Ū��P��Ʊ�����֤ˤ���
 */
#define P_REST_V_FLAG 0x40
#define P_REST_B_FLAG 0x10
#define P_REST_D_FLAG 0x08
#define P_REST_I_FLAG 0x04
#define P_REST_C_FLAG 0x01
#define P_REST_FLAGS  0x5D

/*
 * �����ߤ�ȯ������ȥ��åȤ����
 */
#define P_REST_INT_PENDING 0x20
#define P_REST_NMI_PENDING 0x02

#define ASSERT_NMI	orr	REG_P_REST, REG_P_REST, #P_REST_NMI_PENDING
#define ASSERT_INT	orr	REG_P_REST, REG_P_REST, #P_REST_INT_PENDING


/*
 * 6502 �������Υե饰
 */
#define P_N_FLAG 0x80
#define P_V_FLAG 0x40
#define P_R_FLAG 0x20
#define P_B_FLAG 0x10
#define P_D_FLAG 0x08
#define P_I_FLAG 0x04
#define P_Z_FLAG 0x02
#define P_C_FLAG 0x01

#define NMI_VECTOR   6
#define RESET_VECTOR 4
#define IRQ_VECTOR   2

/*
 * �������줿���ɥ쥹
 */
#define REG_ADDR r10

/*
 * �Ĥꥵ������
 */
#define REG_CYCLE r11

/*
 * ̿��Υ����ץơ��֥�
 */
#define REG_OP_TABLE r12

#endif
