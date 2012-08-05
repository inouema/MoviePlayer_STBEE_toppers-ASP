/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2010 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  �嵭����Ԥϡ��ʲ���(1)��(4)�ξ������������˸¤ꡤ�ܥ��եȥ���
 *  �����ܥ��եȥ���������Ѥ�����Τ�ޤࡥ�ʲ�Ʊ���ˤ���ѡ�ʣ������
 *  �ѡ������ۡʰʲ������ѤȸƤ֡ˤ��뤳�Ȥ�̵���ǵ������롥
 *  (1) �ܥ��եȥ������򥽡��������ɤη������Ѥ�����ˤϡ��嵭������
 *      ��ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ��꤬�����Τޤޤη��ǥ���
 *      ����������˴ޤޤ�Ƥ��뤳�ȡ�
 *  (2) �ܥ��եȥ������򡤥饤�֥������ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ�����Ǻ����ۤ�����ˤϡ������ۤ�ȼ���ɥ�����ȡ�����
 *      �ԥޥ˥奢��ʤɡˤˡ��嵭�����ɽ�����������Ѿ�浪��Ӳ���
 *      ��̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *  (3) �ܥ��եȥ������򡤵�����Ȥ߹���ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ��ʤ����Ǻ����ۤ�����ˤϡ����Τ����줫�ξ�����������
 *      �ȡ�
 *    (a) �����ۤ�ȼ���ɥ�����ȡ����Ѽԥޥ˥奢��ʤɡˤˡ��嵭����
 *        �ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *    (b) �����ۤη��֤��̤�������ˡ�ˤ�äơ�TOPPERS�ץ������Ȥ�
 *        ��𤹤뤳�ȡ�
 *  (4) �ܥ��եȥ����������Ѥˤ��ľ��Ū�ޤ��ϴ���Ū�������뤤���ʤ�»
 *      ������⡤�嵭����Ԥ����TOPPERS�ץ������Ȥ����դ��뤳�ȡ�
 *      �ޤ����ܥ��եȥ������Υ桼���ޤ��ϥ���ɥ桼������Τ����ʤ���
 *      ͳ�˴�Ť����ᤫ��⡤�嵭����Ԥ����TOPPERS�ץ������Ȥ�
 *      ���դ��뤳�ȡ�
 * 
 *  �ܥ��եȥ������ϡ�̵�ݾڤ��󶡤���Ƥ����ΤǤ��롥�嵭����Ԥ�
 *  ���TOPPERS�ץ������Ȥϡ��ܥ��եȥ������˴ؤ��ơ�����λ�����Ū
 *  ���Ф���Ŭ������ޤ�ơ������ʤ��ݾڤ�Ԥ�ʤ����ޤ����ܥ��եȥ���
 *  �������Ѥˤ��ľ��Ū�ޤ��ϴ���Ū�������������ʤ�»���˴ؤ��Ƥ⡤��
 *  ����Ǥ�����ʤ���
 * 
 *  @(#) $Id: test_tex1.c 2007 2010-12-31 05:48:15Z ertl-hiro $
 */

/* 
 *		�������㳰�����˴ؤ���ƥ���(1)
 *
 * �ڥƥ��Ȥ���Ū��
 *
 *  ���������饿�����㳰�����롼�����ư�������������Ū�˥ƥ��Ȥ��롥
 *
 * �ڥƥ��ȹ��ܡ�
 *
 *	(A) ras_tex�Υ��顼����
 *		(A-1) �оݥ��������ٻ߾���
 *		(A-2) �оݥ������Υ������㳰�����롼�����������Ƥ��ʤ�
 *	(B) dis_tex�Υ��顼����
 *		(B-1) ���������Υ������㳰�����롼�����������Ƥ��ʤ�
 *	(C) ena_tex�Υ��顼����
 *		(C-1) ���������Υ������㳰�����롼�����������Ƥ��ʤ�
 *	(D) ref_tex�Υ��顼����
 *		(D-1) �оݥ��������ٻ߾���
 *		(D-2) �оݥ������Υ������㳰�����롼�����������Ƥ��ʤ�
 *	(E) ras_tex���������
 *		(E-1) �оݥ������������������������㳰�������ġ����ĳ����ͥ��
 *			  �٥ޥ�����������֤ǡ������˼¹Գ���
 *		(E-2) �оݥ����������������Ǥʤ�
 *		(E-3) �оݥ����������������������������㳰�����ػ�
 *		(E-4) �оݥ����������������ǥ������㳰�������Ĥ����������ͥ��
 *			  �٥ޥ�����������Ǥʤ�
 *	(F) ena_tex���������
 *		(F-1) �������㳰�����׵᤬���ꡤ���ĳ����ͥ���٥ޥ����������
 *			  �֤ǡ������˼¹Գ���
 *		(F-2) �������㳰�����׵᤬�ʤ�
 *		(F-3) �������㳰�����׵᤬���뤬�������ͥ���٥ޥ������������
 *			  �ʤ�
 *	(G) chg_ipm���������
 *		(G-1) �������㳰�����׵᤬���ꡤ���ĥ������㳰�������Ĥǡ�����
 *			  �˼¹Գ���
 *		(G-2) �������㳰�����׵᤬�ʤ�
 *		(G-3) �������㳰�����׵᤬���뤬���������㳰�����ػ�
 *	(H) �������ǥ����ѥå���ˤ�뵯ư
 *		(H-1) �ǥ����ѥå���Υ��������������㳰���Ĥǥ������㳰������
 *			  �ᤢ��
 *	(I) �������㳰�����롼���󤫤�Υ꥿����ˤ�뵯ư��Ϣ³��ư��
 *	(J) �������㳰�����롼���󤫤�������ˤ���������
 *		(J-1) ����������äƤ����
 *		(J-2) �������㳰�����롼����Ϣ³��ư������
 *	(K) �������㳰�����롼�����¿�ŵ�ư
 *	(L) �������㳰�����롼���󤫤�������Υ������ڴ���
 *
 * �ڻ��ѥ꥽������
 *
 *	TASK1: �ᥤ��Υ������������������Ф��ƥ������㳰�������׵᤹��
 *	TASK2: ¾���������Ф��ƥ������㳰�������׵᤹���оݥ�����
 *	TASK3: �������㳰�����롼�����������Ƥ��ʤ�������
 *	TASK4: �ٻ߾��֤Υ�����
 *
 * �ڥƥ��ȥ������󥹡�
 *
 *	== TASK1��ͥ���١�10��==
 *	1:	������֤Υ����å�
 *		ref_tex(TSK_SELF, &rtex)
 *		ras_tex(TASK3, 0x0001) -> E_OBJ		... (A-2)
 *		ras_tex(TASK4, 0x0001) -> E_OBJ		... (A-1)
 *		ref_tex(TASK3, &rtex) -> E_OBJ		... (D-2)
 *		ref_tex(TASK4, &rtex) -> E_OBJ		... (D-1)
 *	2:	ena_tex()							... (F-2)
 *		ref_tex(TSK_SELF, &rtex)
 *	3:	ras_tex(TSK_SELF, 0x0001)			... (E-1)
 *	== TASK1�������㳰�����롼�����1���ܡ�==
 *	4:	������֤Υ����å�
 *	5:	dis_dsp() ... 4�Ĥξ��֤򤽤줾���Ѳ�������
 *		chg_ipm(TMAX_INTPRI)
 *		ena_tex()
 *		loc_cpu()
 *		�꥿����
 *	== TASK1��³����==
 *	6:	��äƤ������֤Υ����å�			... (J-1)
 *	7:	dis_dsp() ... �ǥ����ѥå��ػߡ��������㳰�����ػ�
 *		dis_tex()
 *	8:	ras_tex(TASK1, 0x0002)				... (E-3)
 *		ref_tex(TSK_SELF, &rtex)
 *	9:	ena_tex()							... (F-1)
 *	== TASK1�������㳰�����롼�����2���ܡ�==
 *	10:	������֤Υ����å�
 *	11:	ras_tex(TASK1, 0x0001)				... (E-3)
 *		ras_tex(TASK1, 0x0002)				... (E-3)
 *	12:	ena_dsp() ... 3�Ĥξ��֤򤽤줾���Ѳ�������
 *		chg_ipm(TMAX_INTPRI)
 *		loc_cpu()
 *		�꥿����							... (I)
 *	== TASK1�������㳰�����롼�����3���ܡ�==
 *	13:	������֤Υ����å�
 *	14:	ena_dsp() ... �ǥ����ѥå����ġ��������㳰����
 *		chg_ipm(TMAX_INTPRI)
 *		ena_tex()
 *		chg_ipm(TIPM_ENAALL)				... (G-2)
 *		chg_ipm(TMAX_INTPRI)
 *	15: ras_tex(TSK_SELF, 0x0004)			... (E-4)
 *		dis_tex()
 *		chg_ipm(TIPM_ENAALL)				... (G-3)
 *		chg_ipm(TMAX_INTPRI)
 *		ena_tex()							... (F-3)
 *	16:	chg_ipm(TIPM_ENAALL)				... (G-1)(K)
 *	== TASK1�������㳰�����롼�����4���ܡ�==
 *	17:	������֤Υ����å�
 *	18:	dis_dsp() ... 3�Ĥξ��֤򤽤줾���Ѳ�������
 *		chg_ipm(TMAX_INTPRI)
 *		loc_cpu()
 *		�꥿����
 *	== TASK1�������㳰�����롼�����3����³����==
 *	19:	��äƤ������֤Υ����å�			... (J-2)
 *		�꥿����
 *	== TASK1��³����==
 *	20:	��äƤ������֤Υ����å�			... (J-1)
 *	21: ena_dsp()
 *		rot_rdq(TPRI_SELF)
 *	== TASK2��ͥ���١�10��	==
 *	22:	������֤Υ����å�
 *	23:	ena_tex()
 *		rot_rdq(TPRI_SELF)
 *	== TASK3��ͥ���١�10��	==
 *	24:	������֤Υ����å�
 *	25:	ena_tex() -> E_OBJ					... (C-1)
 *	26:	dis_tex() -> E_OBJ					... (B-1)
 *		ext_tsk()
 *	== TASK1��³����==
 *	27: ras_tex(TASK2, 0x0001)				... (E-2)
 *		ref_tex(TASK2, &rtex)
 *	28:	rot_rdq(TPRI_SELF)					... (H-1)
 *	== TASK2�������㳰�����롼�����1���ܡ�==
 *	29:	������֤Υ����å�
 *		�꥿����
 *	== TASK2��³����==
 *	30: ras_tex(TSK_SELF, 0x0002)
 *	== TASK2�������㳰�����롼�����2���ܡ�==
 *	31:	������֤Υ����å�
 *	32:	dis_dsp()
 *		rot_rdq(TPRI_SELF)
 *	33:	�꥿����							... (L)
 *	== TASK1��³����==
 *	34:	�꥿����ʥ�������λ��
 *	== TASK2��³����==
 *	35:	�ƥ��Ƚ�λ
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_tex1.h"

void
tex_task1(TEXPTN texptn, intptr_t exinf)
{
	ER		ercd;

	switch (texptn) {
	case 0x0001:
		check_point(4);
		check_state(false, false, TIPM_ENAALL, false, false, true);

		/*
		 *  �ǥ����ѥå��ػߡ������ͥ���٥ޥ����ѹ����������㳰��������
		 */
		check_point(5);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = ena_tex();
		check_ercd(ercd, E_OK);
		ercd = loc_cpu();
		check_ercd(ercd, E_OK);
		check_state(false, true, TMAX_INTPRI, true, true, false);
		break;

	case 0x0002:
		check_point(10);
		check_state(false, false, TIPM_ENAALL, true, true, true);

		/*
		 *  �������㳰�������׵�
		 */
		check_point(11);
		ercd = ras_tex(TSK_SELF, 0x0001);
		check_ercd(ercd, E_OK);
		ercd = ras_tex(TSK_SELF, 0x0002);
		check_ercd(ercd, E_OK);

		/*
		 *  �ǥ����ѥå����ġ������ͥ���٥ޥ����ѹ���CPU��å�
		 */
		check_point(12);
		ercd = ena_dsp();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = loc_cpu();
		check_ercd(ercd, E_OK);
		check_state(false, true, TMAX_INTPRI, false, true, true);
		break;

	case 0x0003:
		check_point(13);
		check_state(false, false, TIPM_ENAALL, true, true, true);

		/*
		 *  �ǥ����ѥå����ġ������ͥ���٥ޥ����ѹ����������㳰����
		 */
		check_point(14);
		ercd = ena_dsp();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = ena_tex();
		check_ercd(ercd, E_OK);
		check_state(false, false, TMAX_INTPRI, false, true, false);

		ercd = chg_ipm(TIPM_ENAALL);
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);

		/*
		 *  �������㳰�������׵�
		 */
		check_point(15);
		ercd = ras_tex(TSK_SELF, 0x0004);

		ercd = dis_tex();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TIPM_ENAALL);
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = ena_tex();
		check_ercd(ercd, E_OK);

		check_point(16);
		ercd = chg_ipm(TIPM_ENAALL);
		/* �����ǥ������㳰�����롼����ư��� */
		check_ercd(ercd, E_OK);

		/*
		 *  �������㳰��������Υ꥿����ˤ�긵�ξ��֤���äƤ��뤳��
		 *  ������å�
		 */
		check_point(19);
		check_state(false, false, TIPM_ENAALL, false, false, false);
		break;

	case 0x0004:
		check_point(17);
		check_state(false, false, TIPM_ENAALL, false, false, true);

		/*
		 *  �ǥ����ѥå��ػߡ������ͥ���٥ޥ����ѹ���CPU��å�
		 */
		check_point(18);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = loc_cpu();
		check_ercd(ercd, E_OK);
		check_state(false, true, TMAX_INTPRI, true, true, true);
		break;

	default:
		check_point(0);
		break;
	}
}

void
task1(intptr_t exinf)
{
	ER		ercd;
	T_RTEX	rtex;

	/*
	 *  ������֤Υ����å�
	 */
	check_point(1);
	check_state(false, false, TIPM_ENAALL, false, false, true);
	ercd = ref_tex(TSK_SELF, &rtex);
	check_ercd(ercd, E_OK);
	check_assert((rtex.texstat & TTEX_DIS) != 0);
	check_assert(rtex.pndptn == 0);

	/*
	 *  ras_tex�Υ��顼����
	 */
	ercd = ras_tex(TASK3, 0x0001);
	check_ercd(ercd, E_OBJ);
	ercd = ras_tex(TASK4, 0x0001);
	check_ercd(ercd, E_OBJ);

	/*
	 *  ref_tex�Υ��顼����
	 */
	ercd = ref_tex(TASK3, &rtex);
	check_ercd(ercd, E_OBJ);
	ercd = ref_tex(TASK4, &rtex);
	check_ercd(ercd, E_OBJ);

	/*
	 *  �������㳰�����ε���
	 */
	check_point(2);
	ercd = ena_tex();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, false, false, false);
	ercd = ref_tex(TSK_SELF, &rtex);
	check_ercd(ercd, E_OK);
	check_assert((rtex.texstat & TTEX_ENA) != 0);
	check_assert(rtex.pndptn == 0);

	/*
	 *  �������㳰�������׵�
	 */
	check_point(3);
	ercd = ras_tex(TSK_SELF, 0x0001);
	/* �����ǥ������㳰�����롼����ư��� */
	check_ercd(ercd, E_OK);

	/*
	 *  �������㳰��������Υ꥿����ˤ�긵�ξ��֤���äƤ��뤳�Ȥ�
	 *  �����å�
	 */
	check_point(6);
	check_state(false, false, TIPM_ENAALL, false, false, false);

	/*
	 *  �ǥ����ѥå��ػߡ��������㳰�����ػ�
	 */
	check_point(7);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);
	ercd = dis_tex();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, true, true, true);

	/*
	 *  �������㳰�������׵�
	 */
	check_point(8);
	ercd = ras_tex(TASK1, 0x0002);
	check_ercd(ercd, E_OK);
	ercd = ref_tex(TSK_SELF, &rtex);
	check_ercd(ercd, E_OK);
	check_assert((rtex.texstat & TTEX_DIS) != 0);
	check_assert(rtex.pndptn == 0x0002);

	/*
	 *  �������㳰���������
	 */
	check_point(9);
	ercd = ena_tex();
	/* �����ǥ������㳰�����롼����ư��� */
	check_ercd(ercd, E_OK);

	/*
	 *  �������㳰��������Υ꥿����ˤ�긵�ξ��֤���äƤ��뤳�Ȥ�
	 *  �����å�
	 */
	check_point(20);
	check_state(false, false, TIPM_ENAALL, true, true, false);

	/*
	 *  ������2���ڤ괹����
	 */
	check_point(21);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);
	ercd = rot_rdq(TPRI_SELF);
	/* ������¾�Υ�������ư��� */
	check_ercd(ercd, E_OK);

	/*
	 *  ������2���Ф��ƥ������㳰�������׵�
	 */
	check_point(27);
	ercd = ras_tex(TASK2, 0x0001);
	check_ercd(ercd, E_OK);
	ercd = ref_tex(TASK2, &rtex);
	check_ercd(ercd, E_OK);
	check_assert((rtex.texstat & TTEX_ENA) != 0);
	check_assert(rtex.pndptn == 0x0001);

	/*
	 *  ������2���ڤ괹����
	 */
	check_point(28);
	ercd = rot_rdq(TPRI_SELF);
	/* ������¾�Υ�������ư��� */
	check_ercd(ercd, E_OK);

	/*
	 *  ��������λ
	 */
	check_point(34);
}

void
tex_task2(TEXPTN texptn, intptr_t exinf)
{
	ER		ercd;

	switch (texptn) {
	case 0x0001:
		check_point(29);
		check_state(false, false, TIPM_ENAALL, false, false, true);
		break;

	case 0x0002:
		check_point(31);
		check_state(false, false, TIPM_ENAALL, false, false, true);

		/*
		 *  �ǥ����ѥå���ػߤ��ơ��������ڴ������׵᤹�롥
		 */
		check_point(32);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);
		ercd = rot_rdq(TPRI_SELF);
		check_ercd(ercd, E_OK);

		/*
		 *  �������㳰�����롼���󤫤�Υ꥿����ǡ��������ڴ�����ȯ��
		 *  ���롥
		 */
		check_point(33);
		break;

	default:
		check_point(0);
		break;
	}
}

void
task2(intptr_t exinf)
{
	ER		ercd;

	/*
	 *  ������֤Υ����å�
	 */
	check_point(22);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  �������㳰�����ε���
	 */
	check_point(23);
	ercd = ena_tex();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, false, false, false);

	/*
	 *  ������3���ڤ괹����
	 */
	ercd = rot_rdq(TPRI_SELF);
	/* ������¾�Υ�������ư��� */
	check_ercd(ercd, E_OK);

	/*
	 *  �������㳰�������׵�
	 */
	check_point(30);
	ercd = ras_tex(TSK_SELF, 0x0002);
	/* �����ǥ������㳰�����롼����ư��� */
	check_ercd(ercd, E_OK);

	/*
	 *  �ƥ��Ƚ�λ
	 */
	check_finish(35);
}

void
task3(intptr_t exinf)
{
	ER		ercd;

	/*
	 *  ������֤Υ����å�
	 */
	check_point(24);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  �������㳰�����ε���
	 */
	check_point(25);
	ercd = ena_tex();
	check_ercd(ercd, E_OBJ);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  �������㳰�����ζػ�
	 */
	check_point(26);
	ercd = dis_tex();
	check_ercd(ercd, E_OBJ);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  ��������λ
	 */
	ercd = ext_tsk();
	check_point(0);
}

void
task4(intptr_t exinf)
{
	check_point(0);
}

void
tex_task4(TEXPTN texptn, intptr_t exinf)
{
	check_point(0);
}
