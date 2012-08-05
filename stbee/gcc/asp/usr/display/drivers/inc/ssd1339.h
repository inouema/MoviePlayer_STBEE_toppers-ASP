/********************************************************************************/
/*!
	@file			ssd1339.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2011.10.31
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					It can drive UG-2828GFEFF01 OLED module.

    @section HISTORY
		2011.10.31	V1.00	Renewal from SED1339.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __SSD1339_H
#define __SSD1339_H	0x0100

#ifdef __cplusplus
 extern "C" {
#endif

/* basic includes */
#include <string.h>
#include <inttypes.h>

/* display includes */
#include "display_if_basis.h"

/* SSD1339 Unique Value		*/
/* MUST be need for SSD1339 */
#define OFS_RAW				0
#define OFS_COL				0
#define MAX_X				128
#define MAX_Y				128

/* Display Contol Macros */
#define SSD1339_RES_SET()	DISPLAY_RES_SET()
#define SSD1339_RES_CLR()	DISPLAY_RES_CLR()
#define SSD1339_CS_SET()	DISPLAY_CS_SET()
#define SSD1339_CS_CLR()	DISPLAY_CS_CLR()
#define SSD1339_DC_SET()	DISPLAY_DC_SET()
#define SSD1339_DC_CLR()	DISPLAY_DC_CLR()
#define SSD1339_WR_SET()	DISPLAY_WR_SET()
#define SSD1339_WR_CLR()	DISPLAY_WR_CLR()
#define SSD1339_RD_SET()	DISPLAY_RD_SET()
#define SSD1339_RD_CLR()	DISPLAY_RD_CLR()
#define SSD1339_SCK_SET()	DISPLAY_SCK_SET()
#define SSD1339_SCK_CLR()	DISPLAY_SCK_CLR()


#if defined(GPIO_ACCESS_8BIT) | defined(GPIO_ACCESS_16BIT)
 #define SSD1339_WR()   	SSD1339_WR_CLR(); \
							SSD1339_WR_SET();
#else
 #define SSD1339_WR()
#endif

#define	SSD1339_DATA		DISPLAY_DATAPORT
#define SSD1339_CMD			DISPLAY_CMDPORT


/* Display Control Functions Prototype */
extern void SSD1339_reset(void);
extern void SSD1339_init(void);
extern void SSD1339_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height);
extern void SSD1339_wr_cmd(uint8_t cmd);
extern void SSD1339_wr_dat(uint8_t dat);
extern void SSD1339_wr_block(uint8_t* blockdata,unsigned int datacount);
extern void SSD1339_clear(void);
extern int  SSD1339_draw_bmp(const uint8_t* ptr);
extern void SSD1339_wr_gram(uint16_t gram);

/* For Display Module's Delay Routine */
#define Display_timerproc_if()	ticktime++
extern volatile uint32_t ticktime;

/* Macros From Application Layer */ 
#define Display_init_if			SSD1339_init
#define Display_rect_if 		SSD1339_rect
#define Display_wr_dat_if		SSD1339_wr_gram
#define Display_wr_cmd_if		SSD1339_wr_cmd
#define Display_wr_block_if		SSD1339_wr_block
#define Display_clear_if 		SSD1339_clear
#define Display_draw_bmp_if		SSD1339_draw_bmp

#ifdef __cplusplus
}
#endif

#endif /* __SSD1339_H */
