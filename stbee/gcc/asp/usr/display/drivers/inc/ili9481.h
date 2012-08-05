/********************************************************************************/
/*!
	@file			ili9481.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        5.00
    @date           2012.01.02
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					It can drive S95517-AAA		TFT module(16bit bus).			@n
					It can drive S95517-A035	TFT module(16bit bus).			@n
					It can drive TFT1P2797-E	TFT module(16/8bit bus).		@n

    @section HISTORY
		2010.10.01	V1.00	Stable Release.
		2010.12.31	V2.00	Cleanup SourceCode.
		2011.03.10	V3.00	C++ Ready.
		2011.10.25	V4.00	Added DMA TransactionSupport.
		2012.01.02	V5.00	Add TFT1P2797-E Support.
		
    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __ILI9481_H
#define __ILI9481_H	0x0500

#ifdef __cplusplus
 extern "C" {
#endif

/* basic includes */
#include <string.h>
#include <inttypes.h>

/* display includes */
#include "display_if_basis.h"

/* ILI9481 unique value */
/* mst be need for ILI9481 */
#define OFS_COL				0
#define OFS_RAW				0
#define MAX_X				320
#define MAX_Y				480

#if defined(USE_ADS7843)			/* For TFT1P2797 (with TouchPanel model) module */
	#define USE_TFT1P2797_E
#else 								/* For S95517 module */
#endif

/* Display Contol Macros */
#define ILI9481_RES_SET()	DISPLAY_RES_SET()
#define ILI9481_RES_CLR()	DISPLAY_RES_CLR()
#define ILI9481_CS_SET()	DISPLAY_CS_SET()
#define ILI9481_CS_CLR()	DISPLAY_CS_CLR()
#define ILI9481_DC_SET()	DISPLAY_DC_SET()
#define ILI9481_DC_CLR()	DISPLAY_DC_CLR()
#define ILI9481_WR_SET()	DISPLAY_WR_SET()
#define ILI9481_WR_CLR()	DISPLAY_WR_CLR()
#define ILI9481_RD_SET()	DISPLAY_RD_SET()
#define ILI9481_RD_CLR()	DISPLAY_RD_CLR()


#if defined(GPIO_ACCESS_8BIT) | defined(GPIO_ACCESS_16BIT)
 #define ILI9481_WR()   	ILI9481_WR_CLR();ILI9481_WR_CLR(); \
							ILI9481_WR_SET();
#else
 #define ILI9481_WR()
#endif

#define	ILI9481_DATA		DISPLAY_DATAPORT
#define ILI9481_CMD			DISPLAY_CMDPORT


/* Display Control Functions Prototype */
extern void ILI9481_reset(void);
extern void ILI9481_init(void);
extern void ILI9481_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height);
extern void ILI9481_wr_cmd(uint8_t cmd);
extern void ILI9481_wr_dat(uint8_t dat);
extern void ILI9481_wr_block(uint8_t* blockdata,unsigned int datacount);
extern void ILI9481_clear(void);
extern int  ILI9481_draw_bmp(const uint8_t* ptr);
extern uint16_t ILI9481_rd_cmd(uint8_t cmd);
extern void ILI9481_wr_gram(uint16_t gram);

/* For Display Module's Delay Routine */
#define Display_timerproc_if()	ticktime++
extern volatile uint32_t ticktime;

/* Macros From Application Layer */ 
#define Display_init_if			ILI9481_init
#define Display_rect_if 		ILI9481_rect
#define Display_wr_dat_if		ILI9481_wr_gram
#define Display_wr_cmd_if		ILI9481_wr_cmd
#define Display_wr_block_if		ILI9481_wr_block
#define Display_clear_if 		ILI9481_clear
#define Display_draw_bmp_if		ILI9481_draw_bmp

#ifdef __cplusplus
}
#endif

#endif /* __ILI9481_H */
