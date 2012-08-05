/********************************************************************************/
/*!
	@file			ili9225.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.04.20
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					Available TFT-LCM are listed below.							@n
					 -TXDT200A-15V13		(ILI9225)	  	8bit mode. 			@n
					 -KXM220HS-V05			(ILI9226)		8/16bit mode.		@n
					 -RX020C-1				(S6D0164X1)		8bit mode.

    @section HISTORY
		2012.04.20	V1.00	Revised from ili9225.c
		
    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __ILI9225_H
#define __ILI9225_H	0x0100

#ifdef __cplusplus
 extern "C" {
#endif

/* basic includes */
#include <string.h>
#include <inttypes.h>

/* display includes */
#include "display_if_basis.h"

/* ILI9225 unique value */
/* mst be need for ILI9225 */
#define OFS_COL				0
#define OFS_RAW				0
#define MAX_X				176
#define MAX_Y				220

/* Display Contol Macros */
#define ILI9225_RES_SET()	DISPLAY_RES_SET()
#define ILI9225_RES_CLR()	DISPLAY_RES_CLR()
#define ILI9225_CS_SET()	DISPLAY_CS_SET()
#define ILI9225_CS_CLR()	DISPLAY_CS_CLR()
#define ILI9225_DC_SET()	DISPLAY_DC_SET()
#define ILI9225_DC_CLR()	DISPLAY_DC_CLR()
#define ILI9225_WR_SET()	DISPLAY_WR_SET()
#define ILI9225_WR_CLR()	DISPLAY_WR_CLR()
#define ILI9225_RD_SET()	DISPLAY_RD_SET()
#define ILI9225_RD_CLR()	DISPLAY_RD_CLR()


#if defined(GPIO_ACCESS_8BIT) | defined(GPIO_ACCESS_16BIT)
 #define ILI9225_WR()   	ILI9225_WR_CLR(); \
							ILI9225_WR_SET();
#else
 #define ILI9225_WR()
#endif

#define	ILI9225_DATA		DISPLAY_DATAPORT
#define ILI9225_CMD			DISPLAY_CMDPORT


/* Display Control Functions Prototype */
extern void ILI9225_reset(void);
extern void ILI9225_init(void);
extern void ILI9225_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height);
extern void ILI9225_wr_cmd(uint8_t cmd);
extern void ILI9225_wr_dat(uint16_t dat);
extern void ILI9225_wr_block(uint8_t* blockdata,unsigned int datacount);
extern void ILI9225_clear(void);
extern int  ILI9225_draw_bmp(const uint8_t* ptr);
extern uint16_t ILI9225_rd_cmd(uint16_t cmd);

/* For Display Module's Delay Routine */
#define Display_timerproc_if()	ticktime++
extern volatile uint32_t ticktime;

/* Macros From Application Layer */
#define Display_init_if			ILI9225_init
#define Display_rect_if 		ILI9225_rect
#define Display_wr_dat_if		ILI9225_wr_dat
#define Display_wr_cmd_if		ILI9225_wr_cmd
#define Display_wr_block_if		ILI9225_wr_block
#define Display_clear_if 		ILI9225_clear
#define Display_draw_bmp_if		ILI9225_draw_bmp

#ifdef __cplusplus
}
#endif

#endif /* __ILI9225_H */
