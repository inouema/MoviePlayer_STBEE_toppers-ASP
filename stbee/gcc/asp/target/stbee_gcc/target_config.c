/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2011 by Embedded and Real-Time Systems Laboratory
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
 */

/*
 * �������åȰ�¸�⥸�塼���STBEE�ѡ�
 */
#include "kernel_impl.h"
#include <sil.h>
#include "stbee.h"
#include "stm32f.h"
#include "usart.h"

/*
 * �������åȰ�¸�������������
 */
void
target_initialize(void)
{
	/*
	 *  �ץ��å�����å�(RCC)�ν����
	 */
	/* HSI��ͭ���� */
	sil_orw((void*)RCC_CR, CR_HSI_ON);

	/* SW, HPRE, PPRE1, PPRE2, ADCPRE,MCO�Υ��ꥢ */
	sil_andw((void*)RCC_CFGR, 0xF8FF0000);

	/* HSEON, CSSON, PLLON �Υ��ꥢ */
	sil_andw((void*)RCC_CR, 0xFEF6FFFF);

	/* HSEBYP �Υ��ꥢ */
	sil_andw((void*)RCC_CR, 0xFFFBFFFF);

	/* PLLSRC, PLLXTPRE, PLLMUL, USBPRE/OTGFSPRE �Υ��ꥢ */
	sil_andw((void*)RCC_CFGR, 0xFF80FFFF);

	/*
	 *  72Mhz������
	 */
	/* HSE��ͭ���� */
	sil_orw((void*)RCC_CR, CR_HSE_ON);

	/* HSEͭ���Ԥ� */
	while ((sil_rew_mem((void*)RCC_CR) & CR_HSE_RDY) == 0) ;

	/* FLASH ROM�Υץ�ե��å��Хåե���ͭ���� */
	sil_orw((void*)FLASH_ACR, ACR_PRFTBE);

	/* FLASH ROM��2wait������ */
	sil_andw((void*)FLASH_ACR, ~ACR_LATENCY_MASK);
	sil_orw((void*)FLASH_ACR, ACR_LATENCY_TWO);

	/* HCLK = SYSCLK, PCLK2 = HCLK, PCLK1 = HCLK/2  */
	sil_orw((void*)RCC_CFGR, 0x00 | (0x00 << 11) | (0x04 << 8));

	/* PLLCLK = 12MHz x 6 = 72MHz, HSE as PLL clock */
	sil_andw((void*)RCC_CFGR, ~(CFGR_PLL_SRC|CFGR_PLL_XTPRE|CFGR_PLLMUL_MASK));
	sil_orw((void*)RCC_CFGR, CFGR_PLL_SRC | PLLMUL << 18);

	/* PLL��ͭ���� */
	sil_orw((void*)RCC_CR, CR_PLL_ON);

	/* PLLͭ���Ԥ� */
	while ((sil_rew_mem((void*)RCC_CR) & CR_PLL_RDY) == 0) ;

	/* PLL�򥷥��ƥ९��å������� */
	sil_andw((void*)RCC_CFGR, ~CFGR_SW_MASK);
	sil_orw((void*)RCC_CFGR, CFGR_SW_PLL);
  
	/* PLL�Υ����ƥ९��å������Ԥ� */
	while ((sil_rew_mem((void*)RCC_CFGR) & CFGR_SWS_MASK) != (CFGR_SW_PLL << 2)) ;

	/*
	 *  ���åװ�¸���ν����
	 */
	chip_initialize();

	/*
	 *  �ڥ�ե�����ͭ����
	 */
#if 0
	sil_orw((void*)RCC_APB2ENR, APB2ENR_USART1_EN | APB2ENR_IOPA_EN |
			APB2ENR_IOPD_EN | APB2ENR_AFIO_EN);
#endif
	sil_orw((void*)RCC_APB2ENR,
		APB2ENR_USART1_EN | 
		APB2ENR_IOPA_EN   | 
		APB2ENR_IOPD_EN   | 
		APB2ENR_IOPE_EN   | 
		APB2ENR_IOPF_EN   | 
		APB2ENR_IOPG_EN   | 
		APB2ENR_AFIO_EN);

	/* todo:AAAAA add by Masato INOUE */
	sil_orw((void*)RCC_AHBENR, 
		AHBENR_SRAM_EN  | 
		AHBENR_FLITF_EN |
		AHBENR_FSMC_EN);

#if (TNUM_PORT >= 2)
	sil_orw((void*)RCC_APB2ENR, APB2ENR_IOPD_EN);
	sil_orw((void*)RCC_APB1ENR, APB1ENR_USART2_EN);
#endif


	/*
	 *  I/O�ݡ��Ȥν����
	 */
	/* USART1(RX)  �ץ륢�å� */
	set_cr_mode(GPIOA_BASE, 10, MODE_INPUT);
	set_cr_cnf(GPIOA_BASE, 10, CNF_IN_FLOATING);

	/* USART1(TX) */
	set_cr_mode(GPIOA_BASE, 9, MODE_OUTPUT_50MHZ);
	set_cr_cnf(GPIOA_BASE, 9, CNF_OUT_AF_PP);

#if (TNUM_PORT >= 2)
	/* USART2(RX)  �ץ륢�å� */
	set_cr_mode(GPIOD_BASE, 6, MODE_INPUT);
	set_cr_cnf(GPIOD_BASE, 6, CNF_IN_FLOATING);

	/* USART2(TX) */
	set_cr_mode(GPIOD_BASE, 5, MODE_OUTPUT_50MHZ);
	set_cr_cnf(GPIOD_BASE, 5, CNF_OUT_AF_PP);

	/* USART2�ݡ��ȤΥ�ޥåס�PD5,6�� */
	sil_orw((void*)AFIO_MAPR, MAPR_USART2_REMAP);
#endif

#if 0
	/* LED�ݡ���(PD.4) */
	set_cr_mode(GPIOD_BASE, 4, MODE_OUTPUT_50MHZ);
	set_cr_cnf(GPIOD_BASE, 4, CNF_OUT_GP_PP);
#endif

	/*
	 *  �С��ʡ������ѤΥ��ꥢ������
	 */
	usart_init(SIO_PORTID);

}


/*
 *  �ܡ��ɾ��LED��ON/OFF
 */
void
stbee_led_on(void){
	sil_andw((void*)(GPIOD_BASE + 0x0c), ~(1 << 4));
}

void
stbee_led_off(void){
	sil_orw((void*)(GPIOD_BASE + 0x0c), (1 << 4));
}

/*
 * �������åȰ�¸�� ��λ����
 */
void
target_exit(void)
{
	/* ���åװ�¸���ν�λ���� */
	chip_exit();
	while(1);
}

/*
 * �����ƥ�������٥���ϤΤ����ʸ������
 */
void
target_fput_log(char_t c)
{
	if (c == '\n') {
		sio_pol_snd_chr('\r', SIO_PORTID);
	}
	sio_pol_snd_chr(c, SIO_PORTID);
}
