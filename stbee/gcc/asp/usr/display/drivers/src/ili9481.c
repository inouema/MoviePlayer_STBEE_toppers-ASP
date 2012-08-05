/********************************************************************************/
/*!
	@file			ili9481.c
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

/* Includes ------------------------------------------------------------------*/
#include "ili9481.h"
/* check header file version for fool proof */
#if __ILI9481_H != 0x0500
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
inline void ILI9481_reset(void)
{
	ILI9481_RES_SET();							/* RES=H, RD=H, WR=H   		*/
	ILI9481_RD_SET();
	ILI9481_WR_SET();
	_delay_ms(1);								/* wait 1ms     			*/

	ILI9481_RES_CLR();							/* RES=L, CS=L   			*/
	ILI9481_CS_CLR();
	_delay_ms(10);								/* wait 10ms     			*/
	
	ILI9481_RES_SET();						  	/* RES=H					*/
	_delay_ms(100);				    			/* wait 100ms     			*/
}

/**************************************************************************/
/*! 
    Write LCD Command.
*/
/**************************************************************************/
inline void ILI9481_wr_cmd(uint8_t cmd)
{
	ILI9481_DC_CLR();							/* DC=L						*/

	ILI9481_CMD = cmd;							/* cmd						*/
	ILI9481_WR();								/* WR=L->H					*/

	ILI9481_DC_SET();							/* DC=H						*/
}

/**************************************************************************/
/*! 
    Write LCD Data.
*/
/**************************************************************************/
inline void ILI9481_wr_dat(uint8_t dat)
{
	ILI9481_DATA = dat;							/* data						*/
	ILI9481_WR();								/* WR=L->H					*/
}

/**************************************************************************/
/*! 
    Write LCD GRAM.
*/
/**************************************************************************/
inline void ILI9481_wr_gram(uint16_t gram)
{
#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	ILI9481_DATA = (uint8_t)(gram>>8);			/* upper 8bit data			*/
	ILI9481_WR();								/* WR=L->H					*/
	ILI9481_DATA = (uint8_t)gram;				/* lower 8bit data			*/
#else
	ILI9481_DATA = gram;						/* 16bit data 				*/
#endif
	ILI9481_WR();								/* WR=L->H					*/
}

/**************************************************************************/
/*! 
    Write LCD Block Data.
*/
/**************************************************************************/
inline void ILI9481_wr_block(uint8_t *p, unsigned int cnt)
{

#ifdef  USE_DISPLAY_DMA_TRANSFER
   DMA_TRANSACTION(p, cnt);
#else

	cnt /= 4;
	
	while (cnt--) {
		/* avoid -Wsequence-point's warning */
		ILI9481_wr_gram(*(p+1)|*(p)<<8);
		p++;p++;
		ILI9481_wr_gram(*(p+1)|*(p)<<8);
		p++;p++;
	}
#endif

}

/**************************************************************************/
/*! 
    Set Rectangle.
*/
/**************************************************************************/
inline void ILI9481_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height)
{

	ILI9481_wr_cmd(0x2A);				/* Horizontal RAM Start ADDR */
	ILI9481_wr_dat((OFS_COL + x)>>8);
	ILI9481_wr_dat(OFS_COL + x);
	ILI9481_wr_dat((OFS_COL + width)>>8);
	ILI9481_wr_dat(OFS_COL + width);

	ILI9481_wr_cmd(0x2B);				/* Horizontal RAM Start ADDR */
	ILI9481_wr_dat((OFS_RAW + y)>>8);
	ILI9481_wr_dat(OFS_RAW + y);
	ILI9481_wr_dat((OFS_RAW + height)>>8);
	ILI9481_wr_dat(OFS_RAW + height);

	ILI9481_wr_cmd(0x2C);				/* Write Data to GRAM */

}

/**************************************************************************/
/*! 
    Clear Display.
*/
/**************************************************************************/
inline void ILI9481_clear(void)
{
	volatile uint32_t n;

	ILI9481_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		ILI9481_wr_dat(COL_BLACK);
	} while (--n);

}


/**************************************************************************/
/*! 
    Read LCD Register.
*/
/**************************************************************************/
inline uint16_t ILI9481_rd_cmd(uint8_t cmd)
{
	uint8_t temp,i;
	uint16_t val;


	ILI9481_wr_cmd(cmd);
	ILI9481_WR_SET();

	for(i=0;i<4;i++){
		ReadLCDData(temp);
	}

    ReadLCDData(val);

	val &= 0x00FF;
	val |= temp<<8;

	return val;
}


/**************************************************************************/
/*! 
    TFT-LCD Module Initialize.
*/
/**************************************************************************/
void ILI9481_init(void)
{
	volatile uint16_t devicetype;
	
	Display_IoInit_If();

	ILI9481_reset();

	/* Check Device Code */
	devicetype = ILI9481_rd_cmd(0xBF);  	/* Confirm Vaild LCD Controller */

	if(devicetype == 0x9481)
	{
		/* Initialize ILI9481 */
		ILI9481_wr_cmd(0x11);				/* Exit Sleep Mode */
		_delay_ms(100);						/* Wait Stability */
		
		ILI9481_wr_cmd(0x13);				/* Entering Nomal Displaymode */

		ILI9481_wr_cmd(0xD0);				/* LCD Power Settings */
		ILI9481_wr_dat(0x07);
		ILI9481_wr_dat(0x40);/* 41 */   
		ILI9481_wr_dat(0x1C);/* 1e */

		ILI9481_wr_cmd(0xD1);				/* VCOM Control */
		ILI9481_wr_dat(0x00);
		ILI9481_wr_dat(0x18);
		ILI9481_wr_dat(0x1D);

		ILI9481_wr_cmd(0xD2);				/* Power_Setting for Normal Mode */
		ILI9481_wr_dat(0x01);
		ILI9481_wr_dat(0x11);

		ILI9481_wr_cmd(0xC0);				/* Panel Driving Settings */
		ILI9481_wr_dat(0x00);
		ILI9481_wr_dat(0x3B);
		ILI9481_wr_dat(0x00);
		ILI9481_wr_dat(0x02);
		ILI9481_wr_dat(0x11);

		ILI9481_wr_cmd(0xC1);				/* Display_Timing_Setting for Normal Mode */
		ILI9481_wr_dat(0x10);
		ILI9481_wr_dat(0x0B);
		ILI9481_wr_dat(0x88);

		ILI9481_wr_cmd(0xC5);				/* Frame Rate and Inversion Control */
		ILI9481_wr_dat(0x01);

#if !defined(USE_TFT1P2797_E)
		ILI9481_wr_cmd(0xC8);				/* Gamma Settings */
		ILI9481_wr_dat(0x00);                       
		ILI9481_wr_dat(0x30);
		ILI9481_wr_dat(0x36);
		ILI9481_wr_dat(0x45);
		ILI9481_wr_dat(0x04);
		ILI9481_wr_dat(0x16);
		ILI9481_wr_dat(0x37);
		ILI9481_wr_dat(0x75);
		ILI9481_wr_dat(0x77);
		ILI9481_wr_dat(0x54);
		ILI9481_wr_dat(0x0F);
		ILI9481_wr_dat(0x00);
#endif

		ILI9481_wr_cmd(0xE4);				/* ??? */
		ILI9481_wr_dat(0xA0);

		ILI9481_wr_cmd(0xF0);				/* ??? */
		ILI9481_wr_dat(0x01);

		ILI9481_wr_cmd(0xF3);				/* ??? */
		ILI9481_wr_dat(0x40);
		ILI9481_wr_dat(0x0A);

		ILI9481_wr_cmd(0xF7);				/* ??? */
		ILI9481_wr_dat(0x80);

		ILI9481_wr_cmd(0x36);				/* Set_address_mode */
		ILI9481_wr_dat(0x0A);				/* BGR-order,Holizontal-Flip */

		ILI9481_wr_cmd(0x3A);				/* Set_pixel_format */
		ILI9481_wr_dat(0x55);				/* RGB565(16dpp) */

		/*
		ILI9481_wr_cmd(0xB3);
		ILI9481_wr_dat(0x00);
		ILI9481_wr_dat(0x00);
		ILI9481_wr_dat(0x00);
		ILI9481_wr_dat(0x11);
		_delay_ms(120);
		*/
#if defined(USE_TFT1P2797_E)
		ILI9481_wr_cmd(0x21);				/* Invert Mode On */
#endif
		ILI9481_wr_cmd(0x29);				/* Display ON */

	}

	else { for(;;);} /* Invalid Device Code!! */

	ILI9481_clear();

#if 0	/* test code RED */
	volatile uint32_t n;

	ILI9481_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		ILI9481_wr_dat(COL_RED);
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
int ILI9481_draw_bmp(const uint8_t* ptr){

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
	ILI9481_clear();

    /* Limit to MAX_Y */
	m = (bh <= MAX_Y) ? biofs : biofs + (bh - MAX_Y) * iw;

	/* Set First data offset */  
	i = m % 512;

	/* Set BMP's Bottom-Left point */
	m = ye;

    /* Limit MAX_X */
	w = (bw > MAX_X) ? MAX_X : bw;

	do {
		ILI9481_rect(xs, xe, m, m);
		n = 0;
		p = (uint8_t*)ptr + i;
		do {
			n++;
			/* 65k colour access */
			d = *p++ >> 3;
			d |= (*p++ >> 2) << 5;
			d |= (*p++ >> 3) << 11;
			/* 262k colour access */
			//d2 = *p << 14;	  		/* Blue  */
			//d  = (*p++ >> 4);			/* Blue  */
			//d |= ((*p++>>2)<<4);		/* Green */
			//d |= ((*p++>>2)<<10);		/* Red   */
			/* 262k +16M dither colour access */
			//d2 = *p++ << 8;			/* Blue  */
			//d  = *p++;				/* Green */
			//d |= *p++ << 8;			/* Red   */
			ILI9481_wr_gram(d);

		} while (n < w);
		i += iw;

	} while (m-- > ys);

	return 1;
}

/* End Of File ---------------------------------------------------------------*/
