/********************************************************************************/
/*!
	@file			c1e2_04.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.06.23
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					It can drive TM176220CNFWG1	TFT module(8bit bus only).		@n

    @section HISTORY
		2012.06.23	V1.00	Stable Release.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "c1e2_04.h"
/* check header file version for fool proof */
#if __C1E2_04_H != 0x0100
#error "header file version is not correspond!"
#endif

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**************************************************************************/
/*! 
    Abstract Layer Delay Settings.
*/
/**************************************************************************/
#ifndef __SYSTICK_H
volatile uint32_t ticktime;
static inline void _delay_ms(uint32_t ms)
{
	ms += ticktime;
	while (ticktime < ms);
}
#endif

/**************************************************************************/
/*! 
    Display Module Reset Routine.
*/
/**************************************************************************/
inline void C1E2_04_reset(void)
{
	C1E2_04_RES_SET();							/* RES=H, RD=H, WR=H   		*/
	C1E2_04_RD_SET();
	C1E2_04_WR_SET();
	_delay_ms(10);								/* wait 10ms     			*/

	C1E2_04_RES_CLR();							/* RES=L, CS=L   			*/
	C1E2_04_CS_CLR();
	_delay_ms(5);								/* wait 1ms at least     	*/
	
	C1E2_04_RES_SET();						  	/* RES=H					*/
	_delay_ms(10);				    			/* wait 10ms     			*/
}

/**************************************************************************/
/*! 
    Write LCD Command.
*/
/**************************************************************************/
inline void C1E2_04_wr_cmd(uint8_t cmd)
{
	C1E2_04_DC_CLR();							/* DC=L							*/

	C1E2_04_CMD = cmd;							/* command(8bit_Low or 16bit)	*/
	C1E2_04_WR();								/* WR=L->H						*/

	C1E2_04_DC_SET();							/* DC=H							*/
}

/**************************************************************************/
/*! 
    Write LCD Data and GRAM.
*/
/**************************************************************************/
inline void C1E2_04_wr_dat(uint8_t dat)
{
	C1E2_04_DATA = dat;							/* lower 8bit data			*/
	C1E2_04_WR();								/* WR=L->H					*/
}

/**************************************************************************/
/*! 
    Write LCD Data and GRAM.
*/
/**************************************************************************/
inline void C1E2_04_wr_gram(uint16_t dat)
{
	C1E2_04_DATA = (uint8_t)(dat>>8);			/* upper 8bit data			*/
	C1E2_04_WR();								/* WR=L->H					*/
	
	C1E2_04_DATA = (uint8_t)dat;				/* lower 8bit data			*/
	C1E2_04_WR();								/* WR=L->H					*/
}

/**************************************************************************/
/*! 
    Write LCD Block Data.
*/
/**************************************************************************/
inline void C1E2_04_wr_block(uint8_t *p, unsigned int cnt)
{

#ifdef  USE_DISPLAY_DMA_TRANSFER
   DMA_TRANSACTION(p, cnt);
#else
	int n;

	n = cnt % 4;
	cnt /= 4;

	while (cnt--) {
		C1E2_04_wr_dat(*p++);
		C1E2_04_wr_dat(*p++);
		C1E2_04_wr_dat(*p++);
		C1E2_04_wr_dat(*p++);
	}
	while (n--) {
		C1E2_04_wr_dat(*p++);
	}
#endif

}

/**************************************************************************/
/*! 
    Set Rectangle.
*/
/**************************************************************************/
inline void C1E2_04_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height)
{
	C1E2_04_wr_cmd(0x07);				/* SRAM Position X Set */
	C1E2_04_wr_dat(OFS_COL + x);
	C1E2_04_wr_cmd(0x08);				/* SRAM Position Y Set */
	C1E2_04_wr_dat(OFS_RAW + y);

	C1E2_04_wr_cmd(0x10);				/* SRAM Horizontal ADDR Start Set */
	C1E2_04_wr_dat(OFS_COL + x);
	C1E2_04_wr_cmd(0x11);				/* SRAM Vertical ADDR Start Set */
	C1E2_04_wr_dat(OFS_RAW + y);
	C1E2_04_wr_cmd(0x12);				/* SRAM Horizontal ADDR End Set */
	C1E2_04_wr_dat(OFS_COL + width);
	C1E2_04_wr_cmd(0x13);				/* SRAM Vertical ADDR End Set */
	C1E2_04_wr_dat(OFS_RAW + height);

}

/**************************************************************************/
/*! 
    Clear Display.
*/
/**************************************************************************/
inline void C1E2_04_clear(void)
{
	volatile uint32_t n;

	C1E2_04_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		C1E2_04_wr_gram(COL_BLACK);
	} while (--n);

}


/**************************************************************************/
/*! 
    Read LCD Register.
*/
/**************************************************************************/
inline uint8_t C1E2_04_rd_cmd(uint8_t cmd)
{
	uint8_t val;

	C1E2_04_wr_cmd(cmd);
	C1E2_04_WR_SET();

    ReadLCDData(val);

	return val;
}


/**************************************************************************/
/*! 
    TFT-LCD Module Initialize.
*/
/**************************************************************************/
void C1E2_04_init(void)
{
	uint16_t devicetype;
	
	Display_IoInit_If();

	C1E2_04_reset();

	/* Check Device Code */
	devicetype = C1E2_04_rd_cmd(0x00);  			/* Confirm Vaild LCD Controller */

	if((devicetype == 0x0A) || (devicetype == 0x09))
	{
		/* Initialize C1E2_04 */
		C1E2_04_wr_cmd(0x01);
		C1E2_04_wr_dat(0x00);

		C1E2_04_wr_cmd(0x02);
		C1E2_04_wr_dat(0x80);

		C1E2_04_wr_cmd(0x03);
		C1E2_04_wr_dat(0x10);

		C1E2_04_wr_cmd(0x04);
		C1E2_04_wr_dat(0x50);

		C1E2_04_wr_cmd(0x18);
		C1E2_04_wr_dat(0x00);

		C1E2_04_wr_cmd(0x35);
		C1E2_04_wr_dat(0x01);

		_delay_ms(200);

		C1E2_04_wr_cmd(0x07);
		C1E2_04_wr_dat(0x00);

		C1E2_04_wr_cmd(0x08);
		C1E2_04_wr_dat(0x00);

		C1E2_04_wr_cmd(0x05);
		C1E2_04_wr_dat(0x05);

		C1E2_04_wr_cmd(0x06);
		C1E2_04_wr_dat(0x05);

		C1E2_04_wr_cmd(0x09);
		C1E2_04_wr_dat(0x10);

		C1E2_04_wr_cmd(0x14);
		C1E2_04_wr_dat(0x00);

		C1E2_04_wr_cmd(0x15);
		C1E2_04_wr_dat(0x00);

		C1E2_04_wr_cmd(0x16);
		C1E2_04_wr_dat(0xAF);

		C1E2_04_wr_cmd(0x17);
		C1E2_04_wr_dat(0xDB);

		C1E2_04_wr_cmd(0x10);
		C1E2_04_wr_dat(0x00);

		C1E2_04_wr_cmd(0x11);
		C1E2_04_wr_dat(0x00);

		C1E2_04_wr_cmd(0x12);
		C1E2_04_wr_dat(0xAF);

		C1E2_04_wr_cmd(0x13);
		C1E2_04_wr_dat(0xDB);

		C1E2_04_wr_cmd(0x3F);
		C1E2_04_wr_dat(0x08);

		C1E2_04_wr_cmd(0x5F);
		C1E2_04_wr_dat(0xE0);

		C1E2_04_wr_cmd(0x5A);
		C1E2_04_wr_dat(0x1C);

		C1E2_04_wr_cmd(0x09);
		C1E2_04_wr_dat((1<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0));
	}

	else { for(;;);} /* Invalid Device Code!! */

	C1E2_04_clear();

#if 0	/* test code RED */
	volatile uint32_t n;

	C1E2_04_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		C1E2_04_wr_gram(COL_RED);
	} while (--n);
	
	_delay_ms(500);
	for(;;);
#endif

}


/**************************************************************************/
/*! 
    Draw Windows 24bitBMP File.
*/
/**************************************************************************/
int C1E2_04_draw_bmp(const uint8_t* ptr){

	uint32_t n, m, biofs, bw, iw, bh, w;
	uint32_t xs, xe, ys, ye, i;
	uint8_t *p;
	uint16_t d;

	/* Load BitStream Address Offset  */
	biofs = LD_UINT32(ptr+10);
	/* Check Plane Count "1" */
	if (LD_UINT16(ptr+26) != 1)  return 0;
	/* Check BMP bit_counts "24(windows bitmap standard)" */
	if (LD_UINT16(ptr+28) != 24) return 0;
	/* Check BMP Compression "BI_RGB(no compresstion)"*/
	if (LD_UINT32(ptr+30) != 0)  return 0;
	/* Load BMP width */
	bw = LD_UINT32(ptr+18);
	/* Load BMP height */
	bh = LD_UINT32(ptr+22);
	/* Check BMP width under 1280px */
	if (!bw || bw > 1280 || !bh) return 0;
	
	/* Calculate Data byte count per holizontal line */
	iw = ((bw * 3) + 3) & ~3;

	/* Centering */
	if (bw > MAX_X) {
		xs = 0; xe = MAX_X-1;
	} else {
		xs = (MAX_X - bw) / 2;
		xe = (MAX_X - bw) / 2 + bw - 1;
	}
	if (bh > MAX_Y) {
		ys = 0; ye = MAX_Y-1;
	} else {
		ys = (MAX_Y - bh) / 2;
		ye = (MAX_Y - bh) / 2 + bh - 1;
	}

	/* Clear Display */
	C1E2_04_clear();

    /* Limit to MAX_Y */
	m = (bh <= MAX_Y) ? biofs : biofs + (bh - MAX_Y) * iw;

	/* Set First data offset */  
	i = m % 512;

	/* Set BMP's Bottom-Left point */
	m = ye;

    /* Limit MAX_X */
	w = (bw > MAX_X) ? MAX_X : bw;

	do {
		C1E2_04_rect(xs, xe, m, m);
		n = 0;
		p = (uint8_t*)ptr + i;
		do {
			n++;
			/* 65k colour access */
			d = *p++ >> 3;
			d |= (*p++ >> 2) << 5;
			d |= (*p++ >> 3) << 11;
			C1E2_04_wr_gram(d);

		} while (n < w);
		i += iw;

	} while (m-- > ys);

	return 1;
}

/* End Of File ---------------------------------------------------------------*/
