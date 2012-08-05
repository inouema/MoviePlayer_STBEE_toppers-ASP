/********************************************************************************/
/*!
	@file			s6d0129.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.01.02
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					It can drive S95591-AAA TFT module(8bit-bus only).

    @section HISTORY
		2012.01.02	V1.00	Stable Release.
		
    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __S6D0129_H
#define __S6D0129_H	0x0100

#ifdef __cplusplus
 extern "C" {
#endif

/* basic includes */
#include <string.h>
#include <inttypes.h>

/* display includes */
#include "display_if_basis.h"

/* S6D0129 unique value */
/* mst be need for S6D0129 */
#define OFS_COL				0
#define OFS_RAW				0
#define MAX_X				240
#define MAX_Y				320

/* Display Contol Macros */
#define S6D0129_RES_SET()	DISPLAY_RES_SET()
#define S6D0129_RES_CLR()	DISPLAY_RES_CLR()
#define S6D0129_CS_SET()	DISPLAY_CS_SET()
#define S6D0129_CS_CLR()	DISPLAY_CS_CLR()
#define S6D0129_DC_SET()	DISPLAY_DC_SET()
#define S6D0129_DC_CLR()	DISPLAY_DC_CLR()
#define S6D0129_WR_SET()	DISPLAY_WR_SET()
#define S6D0129_WR_CLR()	DISPLAY_WR_CLR()
#define S6D0129_RD_SET()	DISPLAY_RD_SET()
#define S6D0129_RD_CLR()	DISPLAY_RD_CLR()


#if defined(GPIO_ACCESS_8BIT) | defined(GPIO_ACCESS_16BIT)
 #define S6D0129_WR()   	S6D0129_WR_CLR(); \
							S6D0129_WR_SET();
#else
 #define S6D0129_WR()
#endif

#define	S6D0129_DATA		DISPLAY_DATAPORT
#define S6D0129_CMD			DISPLAY_CMDPORT


/* Display Control Functions Prototype */
extern void S6D0129_reset(void);
extern void S6D0129_init(void);
extern void S6D0129_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height);
extern void S6D0129_wr_cmd(uint8_t cmd);
extern void S6D0129_wr_dat(uint16_t dat);
extern void S6D0129_wr_block(uint8_t* blockdata,unsigned int datacount);
extern void S6D0129_clear(void);
extern int  S6D0129_draw_bmp(const uint8_t* ptr);
extern uint16_t S6D0129_rd_cmd(uint8_t cmd);

/* For Display Module's Delay Routine */
#define Display_timerproc_if()	ticktime++
extern volatile uint32_t ticktime;

/* Macros From Application Layer */ 
#define Display_init_if			S6D0129_init
#define Display_rect_if 		S6D0129_rect
#define Display_wr_dat_if		S6D0129_wr_dat
#define Display_wr_cmd_if		S6D0129_wr_cmd
#define Display_wr_block_if		S6D0129_wr_block
#define Display_clear_if 		S6D0129_clear
#define Display_draw_bmp_if		S6D0129_draw_bmp

#ifdef __cplusplus
}
#endif

#endif /* __S6D0129_H */
