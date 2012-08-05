/********************************************************************************/
/*!
	@file			hx8352a.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        6.00
    @date           2011.12.23
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					It can drive QD-ET3207ABT TFT module(HX8352A)	16bit mode.

    @section HISTORY
		2010.07.10	V1.00	Stable Release.
		2010.10.01	V2.00	Changed CTRL-Port Contol Procedure.
		2010.12.31  V3.00   Cleanup SourceCode.
		2011.03.10	V4.00	C++ Ready.
		2011.10.25	V5.00	Added DMA TransactionSupport.
		2011.12.23	V6.00	Optimize Some Codes.
		
    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __HX8352A_H
#define __HX8352A_H	0x0600

#ifdef __cplusplus
 extern "C" {
#endif

/* basic includes */
#include <string.h>
#include <inttypes.h>

/* display includes */
#include "display_if_basis.h"

/* HX8352A unique value */
/* mst be need for HX8352A */
#define OFS_COL				0
#define OFS_RAW				0
#define MAX_X				240
#define MAX_Y				400

/* Display Contol Macros */
#define HX8352A_RES_SET()	DISPLAY_RES_SET()
#define HX8352A_RES_CLR()	DISPLAY_RES_CLR()
#define HX8352A_CS_SET()	DISPLAY_CS_SET()
#define HX8352A_CS_CLR()	DISPLAY_CS_CLR()
#define HX8352A_DC_SET()	DISPLAY_DC_SET()
#define HX8352A_DC_CLR()	DISPLAY_DC_CLR()
#define HX8352A_WR_SET()	DISPLAY_WR_SET()
#define HX8352A_WR_CLR()	DISPLAY_WR_CLR()
#define HX8352A_RD_SET()	DISPLAY_RD_SET()
#define HX8352A_RD_CLR()	DISPLAY_RD_CLR()


#if defined(GPIO_ACCESS_8BIT) | defined(GPIO_ACCESS_16BIT)
 #define HX8352A_WR()   	HX8352A_WR_CLR(); \
							HX8352A_WR_SET();
#else
 #define HX8352A_WR()
#endif

#define	HX8352A_DATA		DISPLAY_DATAPORT
#define HX8352A_CMD			DISPLAY_CMDPORT


/* Display Control Functions Prototype */
extern void HX8352A_reset(void);
extern void HX8352A_init(void);
extern void HX8352A_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height);
extern void HX8352A_wr_cmd(uint8_t cmd);
extern void HX8352A_wr_dat(uint8_t dat);
extern void HX8352A_wr_block(uint8_t* blockdata,unsigned int datacount);
extern void HX8352A_clear(void);
extern int  HX8352A_draw_bmp(const uint8_t* ptr);
extern uint16_t HX8352A_rd_cmd(uint16_t cmd);
extern void HX8352A_wr_gram(uint16_t gram);

/* For Display Module's Delay Routine */
#define Display_timerproc_if()	ticktime++
extern volatile uint32_t ticktime;

/* Macros From Application Layer */ 
#define Display_init_if			HX8352A_init
#define Display_rect_if 		HX8352A_rect
#define Display_wr_dat_if		HX8352A_wr_gram
#define Display_wr_cmd_if		HX8352A_wr_cmd
#define Display_wr_block_if		HX8352A_wr_block
#define Display_clear_if 		HX8352A_clear
#define Display_draw_bmp_if		HX8352A_draw_bmp

#ifdef __cplusplus
}
#endif

#endif /* __HX8352A_H */
